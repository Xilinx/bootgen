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
