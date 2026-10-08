#!/usr/bin/env bash
# Shared helpers for master-ci jobs. Fail closed when a required
# Environment value is missing so incomplete master-ci config cannot pass.

require_var() {
    local name="$1"
    if [ -z "${!name:-}" ]; then
        echo "::error::Environment master-ci is incomplete: ${name} is not set"
        exit 1
    fi
}

repo_root() {
    git rev-parse --show-toplevel
}

base_ref() {
    echo "${BASE_REF:-${GITHUB_BASE_REF:-master}}"
}

ensure_base_ref() {
    local ref
    ref="$(base_ref)"
    if ! git rev-parse --verify "origin/${ref}" >/dev/null 2>&1; then
        git fetch origin "${ref}"
    fi
}
