#!/usr/bin/env bash
# Print Environment master-ci names as seen in the runner shell.
# Does not dump the full environment (avoids secrets and PATH noise).
set -u

print_one() {
    local name="$1"
    local value="${!name-}"
    if [ -z "${value}" ]; then
        echo "${name}=<unset>"
    else
        echo "${name}=${value}"
    fi
}

echo "=== master-ci shell env (after job env map from vars) ==="
print_one GOLDEN_BOOTGEN_PATH
print_one LD_LIBRARY_PATH_BOOTGEN
print_one MASTER_BITBAKE_LOG_DIR
print_one MASTER_BITBAKE_CMD
print_one MASTER_INTERNAL_PATTERNS
print_one MASTER_RUN_INTERNAL_TESTS
print_one OPENSSL_35_HOME
echo "=== end master-ci shell env ==="
