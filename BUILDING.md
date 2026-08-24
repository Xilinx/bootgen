# Building Bootgen

Bootgen's CMake build supports native Linux and macOS hosts. It uses checked-in
Bison and Flex output, so parser generators are not required for a normal
build.

## Requirements

- A C++14 compiler
- CMake 3.20 or newer
- OpenSSL 1.1.1 or newer development headers and libraries (OpenSSL 3 on macOS)
- A threading implementation supported by CMake

On macOS, install the build prerequisites with Homebrew:

```sh
brew install cmake ninja openssl@3
```

## CMake build

Configure with the normal CMake OpenSSL hint when the dependency is not in a
default search path. The path is deliberately supplied by the caller; Bootgen
does not embed Homebrew installation paths.

```sh
cmake -S . -B build/macos-arm64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
cmake --build build/macos-arm64 --parallel
ctest --test-dir build/macos-arm64 --output-on-failure
```

CMake defaults macOS builds to deployment target 15.0, matching the native
macOS CI runners. Override
`CMAKE_OSX_DEPLOYMENT_TARGET` only when all dependencies, including OpenSSL,
support the selected target.

For an Intel build, run the same configuration on an Intel Mac and set
`CMAKE_OSX_ARCHITECTURES=x86_64`. Test each architecture before any universal
binary packaging. Assemble separately tested slices with the checked
compatibility workflow:

```sh
cmake \
  -DSOURCE_DIR="$PWD" \
  -DARM64_BINARY="$PWD/build/macos-arm64/bootgen" \
  -DX86_64_BINARY="$PWD/build/macos-x86_64/bootgen" \
  -DOUTPUT_DIR="$PWD/build/universal" \
  -P cmake/package_universal_macos.cmake
file build/universal/bootgen-universal/bin/bootgen
codesign --force --sign - build/universal/bootgen-universal/bin/bootgen
codesign --verify --deep --strict build/universal/bootgen-universal/bin/bootgen
```

The script verifies both executable slices, requires their `otool -L` runtime
dependencies to match, and checks non-system absolute dependencies (including
OpenSSL) are themselves universal. The resulting staging tree includes the
binary, license, and build documentation.

Install into a staging directory with:

```sh
cmake --install build/macos-arm64 --prefix "$PWD/stage"
```

## macOS packaging and runtime dependencies

The CMake `package` target creates an architecture-specific tarball containing
`bootgen`, its license notices, and build documentation:

```sh
cmake --build build/macos-arm64 --target package
```

Bootgen intentionally links to the caller-supplied OpenSSL installation; the
tarball does not bundle or sign OpenSSL. Install the matching-architecture
Homebrew `openssl@3` package before running it, then inspect the result:

```sh
file build/macos-arm64/bootgen
otool -L build/macos-arm64/bootgen
codesign --force --sign - build/macos-arm64/bootgen
codesign --verify --deep --strict build/macos-arm64/bootgen
```

See `THIRD_PARTY_NOTICES.md` for the source and binary distribution inventory.
The package includes Bootgen's aggregate `LICENSE` and the separate Cisco
BSD-3-Clause notice for the LMS/HSS implementation. OpenSSL remains a dynamic,
caller-supplied dependency and is not redistributed in the package.

Ad-hoc signing is appropriate for local development. A released universal
package requires compatible universal OpenSSL libraries; it must not merge
executables against incompatible runtime libraries.

The compatibility Makefile provides a `macos` target that delegates to CMake:

```sh
make macos CMAKE_ARGS="-DOPENSSL_ROOT_DIR=$(brew --prefix openssl@3)"
```

## Sanitizers

Use the following configuration for the initial host smoke tests:

```sh
cmake -S . -B build/macos-asan -G Ninja \
  -DBOOTGEN_ENABLE_SANITIZERS=ON \
  -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
cmake --build build/macos-asan --parallel
ctest --test-dir build/macos-asan --output-on-failure
```

## Release validation matrix

A successful local build proves only the host and dependency architecture used
for that build. Before publishing a macOS binary or proposing these changes
upstream, run and retain the results of the following checks:

| Gate | Required evidence |
| --- | --- |
| Apple Silicon | Native `arm64` release and sanitizer builds with the complete CTest suite. |
| Intel macOS | Native `x86_64` release and sanitizer builds with the complete CTest suite and Intel Homebrew OpenSSL. |
| Universal package | Independently tested arm64 and x86_64 binaries, universal compatible OpenSSL dependencies, and a successful `package_universal_macos.cmake` run. |
| Linux compatibility | GCC, Clang, and sanitizer builds using the project workflow or an equivalent native Linux environment. |
| Image compatibility | Comparisons with a pinned upstream reference for deterministic images; structural and signature verification for randomized authenticated images. |
| Hosted CI | A clean GitHub Actions run from the contributor fork for every matrix job. |

The current GitHub workflow covers the Linux compiler/sanitizer and native macOS
architecture jobs after the branch is pushed to a fork. It intentionally does
not synthesize a universal package from untested slices.

## Parser regeneration (maintainers only)

The `bisonflex` directory is checked in. Regenerate it only when changing a
grammar or lexer, using Bison 2.7 or newer and Flex 2.5.35 or newer. This is an
explicit opt-in target because generator upgrades can create large reviewable
diffs.

```sh
cmake -S . -B build/parser-maintenance -DBOOTGEN_REGENERATE_PARSERS=ON
cmake --build build/parser-maintenance --target regenerate-parsers
```

The target updates checked-in files in `bisonflex`; inspect those changes,
rebuild, and run the complete test suite before committing them. Normal CMake
and Make builds do not configure or invoke parser generators.

Sanitizer coverage and the current smoke test do not establish boot-image
compatibility. Golden-image, cryptographic, malformed-input, and hardware
tests remain required before a release-quality macOS port can be claimed.
