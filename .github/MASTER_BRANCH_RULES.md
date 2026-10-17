# Master branch rules

Use this file to **gate every port from `Main` into `master`**. Do not `git merge Main` into `master`. Cherry-pick or otherwise port source only, then rewrite commits so they satisfy these rules.

`master` is the open-source Make line. `Main` is the internal Gradle line and the repository default branch. GitHub PR templates load only from the default branch, so master gates are Actions (`.github/workflows/pr-master-ci.yml`), not a template on `master`.

Env-specific values (tool homes, golden binary, bitbake log dir, leak denylist) live in GitHub Environment **`master-ci`**, not in files on `master`. See [master-ci-environment.md](master-ci-environment.md).

## Hard rules

| # | Rule |
|---|------|
| 1 | Build with **`make` only**. Never add Gradle, `gradle_build.csh`, or Main’s Gradle workflows. |
| 2 | **No issue keys** of the form `CR-` + 5 or more digits in the PR title or in source/docs that land on `master`. Avoid them in commit subjects and bodies too (CI warns, does not fail). |
| 3 | Every commit should have a non-empty subject and a DCO line: `Signed-off-by: Name <email>` (CI warns, does not fail). |
| 4 | PR title is `#<BOOTGEN_BUILD_NUMBER> <summary>`. The number must match `common/include/version.h` and be a **greater integer** than `origin/master`. |
| 5 | Do not copy Main’s `Makefile`. Keep master’s Make (optional bison/flex via `BISON_HOME` / `FLEX_HOME`; default OpenSSL `-lssl -lcrypto`). |
| 6 | Do not hardcode lab/toolchain paths (`/proj`, `/wrk`, `/home`, `/build`, `/tmp`) in `Makefile` or `Makefile.win`. |
| 7 | Do not put lab paths, the internal issue-tracker host, or internal device/code names in files on `master`. Those belong in `master-ci`. |
| 8 | Default `make` (homes unset) only compiles checked-in `bisonflex/`. If `parser/` changed, regenerate with homes set and **commit** `bisonflex/`. |
| 9 | Do not add the blocked paths below. Do not replace master’s `.github/` with Main’s. |

## Paths that must never land on `master`

Skip these on every cherry-pick from `Main`. The leak-scan job fails the PR if they appear as added/renamed paths.

- `build.gradle`, `settings.gradle`, `gradle.properties`
- `gradlew`, `gradlew.bat`, `gradle/`
- `gradle_build.csh`, `gradle_build_win.bat`
- `product/`
- `.cursor/`
- `branch.json`
- Main’s `.github/workflows/` (Gradle / internal CI)

Always keep the **current `master` copy** of:

- `Makefile` / `Makefile.win`
- `.github/workflows/pr-master-ci.yml` and `.github/scripts/`
- `.github/MASTER_BRANCH_RULES.md` (this file)
- `.github/master-ci-environment.md`

Source trees that already exist on `master` (for example arch dirs, `wrapper/`, `slh/`, `lms-hash-sigs/`) **should** be ported when the change is in those files. Do not blanket-skip them.

## Commit and PR rewrite (Main messages are not reusable)

Main titles and bodies typically contain issue keys. Master CI rejects those.

For each ported commit:

1. New subject without an issue key (optional `#<n>` only on the **PR title**, not required on every commit subject).
2. Strip issue keys from the body.
3. Add `Signed-off-by: Name <email>`.
4. Do not take Main’s `version.h` blindly. Keep master’s `BOOTGEN_BUILD_NUMBER` policy: integer in quotes, increment once for the PR.

## Build and tools

| Topic | Master rule | Main (do not copy) |
|-------|-------------|--------------------|
| Build | `make` → `build/bin/bootgen` | Gradle / `gradle_build.csh` |
| bison/flex | Env `BISON_HOME` + `FLEX_HOME` (optional `M4_HOME`) | Hardcoded tool prefixes in Main’s Makefile |
| OpenSSL | Default `LIBS = -lssl -lcrypto`. Override with `INCLUDE_USER` / `LIBS` on the make command if a newer prefix is required. Do not bake a prefix into the Makefile. | Internal OpenSSL artifact / Gradle dependency |
| Tests | OpenSSL 3.5 Make binary vs branch `master_test_suit` (pipeline + external + cr_tests; internal only if `MASTER_RUN_INTERNAL_TESTS=true`). Each suite runs even if an earlier one fails | Main’s Gradle test workflows |
| Yocto | Recipe `DEPENDS` on `openssl` with `BBCLASSEXTEND = "native nativesdk"` (`openssl-native` / nativesdk openssl). Version must be whatever that release’s openssl recipe is. | Not Main Gradle |
| Bitbake gate | Scan logs under `MASTER_BITBAKE_LOG_DIR` for `FAIL` / `ERROR` / `ERR` (`WARNING:` ignored). Do not copy Main’s bitbake workflow yaml. | Lab paths inside Main workflows |

## CI jobs that will fail a bad port

All jobs use `runs-on: [self-hosted, bootgen-vm-154183]` (labels, not the runner name) and `environment: master-ci`. Missing required vars fail closed. Put those values on Environment `master-ci` and map them with `${{ vars.NAME }}` on the job. Delete the same names from repository Actions variables so repo vars cannot win.

| Job | Fails when |
|-----|------------|
| Make hygiene | `Makefile` / `Makefile.win` contain `/proj` `/wrk` `/home` `/build` `/tmp` |
| Make build | no `build/bin/bootgen`, or `bisonflex/` dirty after `make` (CI unsets bison/flex homes) |
| Make build (OpenSSL 3.5) | OpenSSL 3.5 `make` failed, or `OPENSSL_35_HOME` is unset / missing `include/` or `lib`/`lib64`. Test suite uses this binary |
| Version | build number missing, not an integer, not greater than base, or title `#n` mismatch |
| Commit format | title not `#<n> <summary>` or has an issue key. Empty subject, missing `Signed-off-by`, or issue key in commits is a warning only |
| Leak scan | blocked path added, issue key, tracker host, or `MASTER_INTERNAL_PATTERNS` hit |
| Test suite | pipeline, external, or cr tests fail |
| Bitbake scan | no `*.bitbake.log` or report, or log/report has `FAIL` / `ERROR` / `ERR` for this PR's revision. A snapshot whose bootgen SHA is not this PR is stale and does not fail the job |

## Main → master merge monitor

Run this on the **port branch** (after cherry-pick cleanup, still staged is fine). Replace `HEAD` with the port branch name if needed.

### 1. What Main still has that master must not receive

```bash
git fetch origin Main master
git diff --name-only origin/master origin/Main | grep -E \
  '^(product/|\.cursor/|gradle/|gradlew|build\.gradle|settings\.gradle|gradle\.properties|gradle_build|branch\.json)'
```

Any of those paths in the **port diff** (`origin/master...HEAD`) is a fail.

### 2. This port vs `origin/master`

```bash
git diff --name-status origin/master...HEAD
git diff origin/master...HEAD -- Makefile Makefile.win .github/
```

Checklist (copy per port):

- [ ] No blocked paths in the name-status list
- [ ] `.github/` still master’s CI (no Gradle workflow files)
- [ ] `Makefile` still has no hardcoded lab paths; bison/flex still env-gated
- [ ] `common/include/version.h`: `BOOTGEN_BUILD_NUMBER` incremented vs `origin/master`
- [ ] `parser/` change ⇒ `bisonflex/` updated and `make` with homes leaves it clean
- [ ] Every commit: `Signed-off-by`, no `CR-` + digits (warning only in CI)
- [ ] Intended PR title: `#<build> <summary>` with no issue key
- [ ] `make` produces `build/bin/bootgen`
- [ ] Local leak-style check on the diff (issue keys, tracker host, blocked paths)

```bash
# Issue keys in the port (must be empty)
git log origin/master..HEAD --format='%s%n%b' | grep -E 'CR-[0-9]{5,}' || true
git diff origin/master...HEAD | grep -E 'CR-[0-9]{5,}' || true

# Makefile hygiene
bash .github/scripts/check_makefile_hygiene.sh
```

Leak scan and version/commit scripts also need `BASE_REF=master`, `PR_TITLE='#<n> summary'`, `MASTER_INTERNAL_PATTERNS`, and `git fetch origin master`.

### 3. Candidate commits still only on Main

Do not merge this list wholesale. Use it to pick source-only commits, then apply the rewrite rules above.

```bash
git log origin/master..origin/Main --oneline -- \
  common/ parser/ versal/ versal_2ve_2vm/ versal_2vp/ zynq/ zynqmp/ \
  spartanup/ utils/ wrapper/ slh/ lms-hash-sigs/
```

Skip any commit that is Gradle-only, `product/`-only, or whose message cannot be rewritten without the issue-key content being the whole change.

### 4. After the PR is open

Confirm required `master-ci` jobs are green. A port that compiles on Main under Gradle can still fail master Make, bisonflex drift, PR title, leak scan, or bitbake log scan.
