#!/usr/bin/env bash
# Require a bitbake log and a report under MASTER_BITBAKE_LOG_DIR.
# Fail if the log is missing, or if log/report contain FAIL, ERROR, or ERR.
# WARNING: lines are ignored. Optional MASTER_BITBAKE_CMD can produce the logs.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=require_env.sh
source "${SCRIPT_DIR}/require_env.sh"

require_var MASTER_BITBAKE_LOG_DIR

if [ -n "${MASTER_BITBAKE_CMD:-}" ]; then
    echo "Running MASTER_BITBAKE_CMD"
    bash -lc "${MASTER_BITBAKE_CMD}"
fi

LOG_DIR="${MASTER_BITBAKE_LOG_DIR}"
if [ ! -d "${LOG_DIR}" ]; then
    echo "::error::bitbake log is not there: MASTER_BITBAKE_LOG_DIR is not a directory"
    exit 1
fi

mapfile -t LOGS < <(find "${LOG_DIR}" -type f -name '*.bitbake.log' | sort)
if [ "${#LOGS[@]}" -eq 0 ]; then
    echo "::error::bitbake log is not there: no *.bitbake.log under MASTER_BITBAKE_LOG_DIR"
    exit 1
fi

mapfile -t REPORTS < <(find "${LOG_DIR}" -type f \( \
    -name 'summary.md' -o -name 'report.md' -o -name '*.summary.md' \
    \) | sort)
if [ "${#REPORTS[@]}" -eq 0 ]; then
    echo "::error::bitbake report is not there: need summary.md, report.md, or *.summary.md"
    exit 1
fi

echo "Logs:    ${LOGS[*]}"
echo "Reports: ${REPORTS[*]}"

# A snapshot produced for another bootgen revision must not fail this PR.
THIS_SHA="$(git rev-parse HEAD 2>/dev/null || true)"
PR_SHA="${GITHUB_SHA:-${THIS_SHA}}"
STALE=0
for report in "${REPORTS[@]}"; do
    rev="$(grep -E 'Bootgen source revision|Commit \(sha\)' "${report}" \
        | grep -oE '[0-9a-f]{40}' | head -1 || true)"
    if [ -n "${rev}" ] && [ "${rev}" != "${THIS_SHA}" ] && [ "${rev}" != "${PR_SHA}" ]; then
        echo "Report ${report} is for bootgen ${rev}, not this PR (${PR_SHA}); skipping ERROR scan of stale snapshot"
        STALE=1
    fi
done
if [ "${STALE}" -eq 1 ]; then
    echo "Bitbake log and report are present (stale snapshot; not failing this PR)"
    exit 0
fi

# Word-aware FAIL / ERROR / ERR. Skip WARNING lines.
# ERR is a whole token so ERROR is matched by ERROR, not by the ERR alt.
scan_file() {
    local file="$1"
    local hits
    hits="$(grep -nE '(^|[[:space:]])(ERROR|FAIL|ERR)(:|[[:space:]]|$)' "${file}" \
        | grep -vE 'WARNING' || true)"
    if [ -n "${hits}" ]; then
        echo "::error::${file}: bitbake log/report contains FAIL, ERROR, or ERR"
        echo "${hits}"
        return 1
    fi
    return 0
}

FAILED=0
for f in "${LOGS[@]}" "${REPORTS[@]}"; do
    echo "Scanning ${f}"
    if ! scan_file "${f}"; then
        FAILED=1
    fi
done

if [ "${FAILED}" -ne 0 ]; then
    echo "Bitbake log/report check failed"
    exit 1
fi

echo "Bitbake log and report are present and contain no FAIL/ERROR/ERR"
