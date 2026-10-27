#!/usr/bin/env bash
# Makefile must not hardcode lab/toolchain paths. Optional bison/flex
# generate rules that read BISON_HOME / FLEX_HOME from the environment
# are allowed.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=require_env.sh
source "${SCRIPT_DIR}/require_env.sh"

ROOT="$(repo_root)"
cd "${ROOT}"

FAILED=0
FILES=()
if [ -f Makefile ]; then
    FILES+=(Makefile)
fi
if [ -f Makefile.win ]; then
    FILES+=(Makefile.win)
fi

if [ "${#FILES[@]}" -eq 0 ]; then
    echo "::error::No Makefile or Makefile.win at the repository root"
    exit 1
fi

fail() {
    echo "::error::$*"
    FAILED=1
}

for mk in "${FILES[@]}"; do
    echo "Checking ${mk}"

    if grep -nE '/(proj|wrk|home|build|tmp)/' "${mk}"; then
        fail "${mk}: hardcoded absolute lab/toolchain path (/proj /wrk /home /build /tmp)"
    fi
done

if [ "${FAILED}" -ne 0 ]; then
    echo "Makefile hygiene failed"
    exit 1
fi

echo "Makefile hygiene passed for: ${FILES[*]}"
