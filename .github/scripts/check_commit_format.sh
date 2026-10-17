#!/usr/bin/env bash
# PR title is #<build> summary with no CR key; that is the hard gate.
# Per-commit DCO / CR-key findings are warnings only: master takes
# squashed ports from Main whose messages cannot be rewritten after push.
# The leak scan still blocks CR keys in the diff itself.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=require_env.sh
source "${SCRIPT_DIR}/require_env.sh"

ROOT="$(repo_root)"
cd "${ROOT}"
ensure_base_ref

REF="$(base_ref)"
RANGE="origin/${REF}..HEAD"

PR_TITLE="${PR_TITLE:-}"
if [ -z "${PR_TITLE}" ]; then
    echo "::error::PR_TITLE is empty"
    exit 1
fi

if echo "${PR_TITLE}" | grep -qE 'CR-[0-9]+'; then
    echo "::error::PR title must not contain a CR issue key"
    exit 1
fi

if ! echo "${PR_TITLE}" | grep -qE '^#[0-9]+ .+'; then
    echo "::error::PR title must match #<build_number> <summary>"
    exit 1
fi

COMMITS="$(git rev-list "${RANGE}")"
if [ -z "${COMMITS}" ]; then
    echo "::error::No commits in ${RANGE}"
    exit 1
fi

WARNED=0
while IFS= read -r sha; do
    [ -z "${sha}" ] && continue
    subject="$(git log -1 --format='%s' "${sha}")"
    body="$(git log -1 --format='%b' "${sha}")"
    echo "Checking ${sha:0:10} ${subject}"

    # Merge commits have no DCO trailer; skip them.
    if git rev-parse --verify --quiet "${sha}^2" >/dev/null; then
        echo "Skipping merge ${sha:0:10}"
        continue
    fi

    if [ -z "${subject//[[:space:]]/}" ]; then
        echo "::warning::${sha:0:10}: empty commit subject"
        WARNED=1
    fi

    if echo "${subject}" | grep -qE 'CR-[0-9]+' || echo "${body}" | grep -qE 'CR-[0-9]+'; then
        echo "::warning::${sha:0:10}: commit message contains a CR issue key"
        WARNED=1
    fi

    # Accept a trailer in the body, or a one-line subject that appended it.
    if ! printf '%s\n%s\n' "${subject}" "${body}" \
            | grep -qE '(^|[[:space:]])Signed-off-by: .+ <[^[:space:]@]+@[^[:space:]>]+>'; then
        echo "::warning::${sha:0:10}: missing Signed-off-by: Name <email>"
        WARNED=1
    fi
done <<< "${COMMITS}"

if [ "${WARNED}" -ne 0 ]; then
    echo "Commit format passed for ${RANGE} with warnings"
else
    echo "Commit format passed for ${RANGE}"
fi
