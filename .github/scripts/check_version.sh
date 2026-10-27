#!/usr/bin/env bash
# BOOTGEN_BUILD_NUMBER must increase vs the PR base and match #<n> in the title.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=require_env.sh
source "${SCRIPT_DIR}/require_env.sh"

ROOT="$(repo_root)"
cd "${ROOT}"
ensure_base_ref

VERSION_H="common/include/version.h"
if [ ! -f "${VERSION_H}" ]; then
    echo "::error::${VERSION_H} not found"
    exit 1
fi

extract_build() {
    local file="$1"
    grep 'BOOTGEN_BUILD_NUMBER' "${file}" | grep -o '"[0-9]*"' | tr -d '"' | head -1
}

PR_BUILD="$(extract_build "${VERSION_H}" || true)"
if [ -z "${PR_BUILD}" ]; then
    echo "::error::BOOTGEN_BUILD_NUMBER not found in ${VERSION_H}"
    exit 1
fi
if ! [[ "${PR_BUILD}" =~ ^[0-9]+$ ]]; then
    echo "::error::BOOTGEN_BUILD_NUMBER must be an integer string, got '${PR_BUILD}'"
    exit 1
fi

REF="$(base_ref)"
BASE_BUILD="0"
if git cat-file -e "origin/${REF}:${VERSION_H}" 2>/dev/null; then
    BASE_BUILD="$(git show "origin/${REF}:${VERSION_H}" | grep 'BOOTGEN_BUILD_NUMBER' | grep -o '"[0-9]*"' | tr -d '"' | head -1 || true)"
    if [ -z "${BASE_BUILD}" ]; then
        BASE_BUILD="0"
    fi
fi

echo "PR build number:   ${PR_BUILD}"
echo "Base build number: ${BASE_BUILD} (origin/${REF})"

if [ "${PR_BUILD}" -le "${BASE_BUILD}" ]; then
    echo "::error::Build number must be incremented. PR has ${PR_BUILD} but origin/${REF} has ${BASE_BUILD}"
    exit 1
fi

PR_TITLE="${PR_TITLE:-}"
if [ -z "${PR_TITLE}" ]; then
    echo "::error::PR_TITLE is empty"
    exit 1
fi

if echo "${PR_TITLE}" | grep -qE 'CR-[0-9]+'; then
    echo "::error::PR title must not contain a CR issue key"
    exit 1
fi

TITLE_NUM="$(echo "${PR_TITLE}" | grep -o '^#[0-9]\+' | tr -d '#' || true)"
if [ -z "${TITLE_NUM}" ]; then
    echo "::error::PR title must start with #<build_number> (e.g. #${PR_BUILD} summary)"
    exit 1
fi

if [ "${TITLE_NUM}" != "${PR_BUILD}" ]; then
    echo "::error::PR title build number (#${TITLE_NUM}) does not match ${VERSION_H} (${PR_BUILD})"
    exit 1
fi

{
    echo "### Version check"
    echo ""
    echo "| Field | Value |"
    echo "|-------|-------|"
    echo "| PR title | \`${PR_TITLE}\` |"
    echo "| Build number | \`${PR_BUILD}\` |"
    echo "| Base (${REF}) | \`${BASE_BUILD}\` |"
} >> "${GITHUB_STEP_SUMMARY:-/dev/null}"

echo "Build number validated: #${PR_BUILD} (incremented from ${BASE_BUILD})"
