# master-ci Environment

PRs into `master` run `.github/workflows/pr-master-ci.yml`. That workflow is
meant to land on the open-source branch, so **lab paths, tracker URLs, CR keys,
and internal device or code names must not appear in yaml or scripts**.

Create a GitHub Environment named **`master-ci`** on this repository and add
the values there (Settings → Environments). Jobs fail closed if a required
variable is missing.

Branch rules and the Main → master merge monitor:
[MASTER_BRANCH_RULES.md](MASTER_BRANCH_RULES.md).

## Required variables

| Variable | Used by | Purpose |
|----------|---------|---------|
| `GOLDEN_BOOTGEN_PATH` | Test suite | Absolute path to the golden bootgen binary on the runner |
| `LD_LIBRARY_PATH_BOOTGEN` | Test suite | Extra loader path prepended when running tests |
| `MASTER_BITBAKE_LOG_DIR` | Bitbake scan | Directory that must contain `*.bitbake.log` and a report (`summary.md`, `report.md`, or `*.summary.md`) |
| `MASTER_INTERNAL_PATTERNS` | Leak scan | Newline-separated tokens or regexes. One pattern per line. Lines starting with `#` are ignored. Put device names, code names, and host prefixes here — not in the workflow. |
| `OPENSSL_35_HOME` | Make build (OpenSSL 3.5), Test suite | Prefix with `include/` and `lib` or `lib64`. Test suite runs that binary. Do not put this path in yaml |

## Optional variables

| Variable | Used by | Purpose |
|----------|---------|---------|
| `BISON_HOME` | unused by CI | Local regenerate from `parser/` only. CI `make` unsets this and compiles checked-in `bisonflex/` |
| `FLEX_HOME` | unused by CI | Pair with `BISON_HOME` for a local regenerate. CI does not need flex |
| `M4_HOME` | unused by CI | Pair with bison 2.7 for a local regenerate |
| `OPENSSL_INCLUDE` | unused by default | Only if you later override Make `INCLUDE_USER`. Default `make` uses `/usr/include` |
| `OPENSSL_LIBS` | unused by default | Only if you later override Make `LIBS`. Default is `-lssl -lcrypto` |
| `MASTER_BITBAKE_CMD` | Bitbake scan | If set, run this command first to produce logs, then scan `MASTER_BITBAKE_LOG_DIR` |
| `MASTER_RUN_INTERNAL_TESTS` | Test suite | Set to `true` to also run `test_suite/internal_tests`. Default is off |

## Secrets

Add a secret instead of a variable only when the value must not be visible on
the Environment page (tokens, or a path you do not want listed as a var).
Reference it from the Environment; do not paste it into files on `master`.

## Runners

`runs-on` matches **labels**, not the runner display name. Main bitbake
pins this host the same way:

```yaml
runs-on:
  - self-hosted
  - bootgen-vm-154183
```

Both labels are ANDed. The host must advertise the `bootgen-vm-154183`
label (Settings → Actions → Runners → labels). Every job sets
`environment: master-ci`. GHE has no separate context for Environment
variables. Jobs copy them with `${{ vars.NAME }}` **after**
`environment: master-ci` is set. That `vars` lookup is the Environment
value, not a repository Actions variable.

To pick Environment instead of repo vars:

1. Settings → Environments → `master-ci` → Environment variables
2. Delete the same names from Settings → Secrets and variables → Actions
   → Variables (repo vars of the same name can still fill `vars`)
3. Leave protection off on `master-ci` (no required reviewers, wait
   timer, or deployment-branch limit) or jobs stay queued while the
   runner looks idle

## bison/flex

Default `make` (no `BISON_HOME` / `FLEX_HOME`) compiles checked-in `bisonflex/`
and never runs bison or flex. CI unsets those homes so Make does not need
bison or flex on the runner. When both homes are set locally, Make
regenerates from `parser/` if those sources are newer, then compiles. After
CI `make`, `bisonflex/` must still match the commit.

OpenSSL: CI runs plain `make`. The Makefile default `LIBS = -lssl -lcrypto`
and the compiler's default include path (`/usr/include`) are used. Do not
set `OPENSSL_*` unless you are intentionally overriding that.

## PR / commit rules enforced by CI

- Title: `#<BOOTGEN_BUILD_NUMBER> <summary>` with no CR issue key
- `BOOTGEN_BUILD_NUMBER` in `common/include/version.h` must be greater than
  the value on the base branch
- Every commit: non-empty subject, `Signed-off-by: Name <email>`, no CR issue key
