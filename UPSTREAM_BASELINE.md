# Upstream baseline

`feat/native-macos` was created from the following Xilinx Bootgen upstream
commit:

```text
remote: https://github.com/Xilinx/bootgen.git
branch: upstream/master
commit: 510dc3a4ffb2bd29fcfb8f8d7a1c5aa2bb1f39f1
subject: presign function broken in HSM flow
```

Verify the branch ancestry before opening or refreshing an upstream pull
request:

```sh
git fetch upstream
git merge-base --is-ancestor 510dc3a4ffb2bd29fcfb8f8d7a1c5aa2bb1f39f1 HEAD
git log --oneline 510dc3a4ffb2bd29fcfb8f8d7a1c5aa2bb1f39f1..HEAD
```

This branch deliberately retains small, reviewable commits so porting,
security, and test changes can be proposed upstream independently when useful.

## Existing macOS proposal

[PR #52](https://github.com/Xilinx/bootgen/pull/52) was open when this branch
was audited on 2026-08-22. Its single commit (`c815d8f`) adds a Makefile path
that calls Homebrew directly and omits `malloc.h` on macOS. This branch includes
the required `malloc.h` portability guards, but does not depend on that proposal:
dependency discovery is caller-supplied through CMake, and the port also covers
OpenSSL 3 compatibility, native CI, family smoke tests, packaging, and
documentation.

When proposing this work upstream, reference PR #52 to avoid duplicate review
and present these commits as a superset with independently reproducible build
and test evidence.
