#!/usr/bin/env bash
# Fail if the PR would land CR keys, tracker hosts, Gradle/internal trees,
# or Environment-supplied internal patterns on the open-source branch.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=require_env.sh
source "${SCRIPT_DIR}/require_env.sh"

require_var MASTER_INTERNAL_PATTERNS

ROOT="$(repo_root)"
cd "${ROOT}"
ensure_base_ref

REF="$(base_ref)"
DIFF_RANGE="origin/${REF}...HEAD"

echo "Scanning ${DIFF_RANGE}"

FAILED=0
fail() {
    echo "::error::$*"
    FAILED=1
}

# Path prefixes / names that must not be added on master.
# Checked against added/renamed paths only, not script source text.
path_is_blocked() {
    local p="$1"
    case "${p}" in
        product|product/*|breck|breck/*|.cursor|.cursor/*)
            return 0
            ;;
        branch.json|*/branch.json)
            return 0
            ;;
        build.gradle|*/build.gradle|settings.gradle|*/settings.gradle|gradle.properties|*/gradle.properties)
            return 0
            ;;
        gradlew|gradlew.bat|gradle/*|*/gradle/*)
            return 0
            ;;
        gradle_build.csh|gradle_build_win.bat)
            return 0
            ;;
    esac
    return 1
}

while IFS=$'\t' read -r status path extra; do
    [ -z "${status}" ] && continue
    # Rename/copy: last field is the new path
    if [[ "${status}" == R* || "${status}" == C* ]]; then
        path="${extra:-${path}}"
    fi
    if path_is_blocked "${path}"; then
        fail "Path not allowed on master: ${path}"
    fi
done < <(git diff --name-status -M "${DIFF_RANGE}")

# Content needles that are safe to hardcode (not product/device names).
# Built from parts so this file does not contain the literal tracker host.
CR_RE='CR-[0-9]{5,}'
TRACKER_HOST="$(printf '%s.%s.%s' jira xilinx com)"

DIFF_FILE="$(mktemp "${TMPDIR:-/tmp}/master-diff.XXXXXX")"
DENY_DIFF="$(mktemp "${TMPDIR:-/tmp}/master-deny-diff.XXXXXX")"
trap 'rm -f "${DIFF_FILE}" "${DENY_DIFF}"' EXIT
git diff "${DIFF_RANGE}" > "${DIFF_FILE}"
# Denylist is not applied to the CI helper text (path blocks still apply).
git diff "${DIFF_RANGE}" -- . \
    ':(exclude).github/scripts/scan_opensource_leaks.sh' \
    ':(exclude).github/master-ci-environment.md' \
    ':(exclude).github/MASTER_BRANCH_RULES.md' > "${DENY_DIFF}"

# Only added lines. Removing a denylisted token is not a leak, and git
# context / deleted lines would otherwise keep failing after a rewrite.
if grep -nE "^\+.*${CR_RE}" "${DIFF_FILE}"; then
    fail "Diff contains a CR issue key (${CR_RE})"
fi

if grep '^+' "${DIFF_FILE}" | grep -F "${TRACKER_HOST}"; then
    fail "Diff contains the internal issue tracker host"
fi

# Environment denylist: one token or regex per line. Blank lines and
# lines starting with # are ignored.
while IFS= read -r pat || [ -n "${pat}" ]; do
    pat="${pat%$'\r'}"
    [[ -z "${pat}" || "${pat}" =~ ^[[:space:]]*# ]] && continue
    if grep -nE "^\+.*${pat}" "${DENY_DIFF}"; then
        fail "Diff matches master-ci internal pattern"
    fi
done <<< "${MASTER_INTERNAL_PATTERNS}"

# If Make files changed, they must still pass consumer hygiene.
if git diff --name-only "${DIFF_RANGE}" | grep -qxE 'Makefile|Makefile.win'; then
    echo "Makefile changed; re-running hygiene"
    "${SCRIPT_DIR}/check_makefile_hygiene.sh" || fail "Makefile hygiene failed on the PR diff"
fi

if [ "${FAILED}" -ne 0 ]; then
    echo "Open-source leak scan failed"
    exit 1
fi

echo "Open-source leak scan passed"
