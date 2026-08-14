# Building Bootgen

Bootgen's CMake build supports native Linux and macOS hosts. It uses checked-in
Bison and Flex output, so parser generators are not required for a normal
build.

## Requirements

- A C++14 compiler
- CMake 3.20 or newer
- OpenSSL development headers and libraries
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

For an Intel build, run the same configuration on an Intel Mac and set
`CMAKE_OSX_ARCHITECTURES=x86_64`. Test each architecture before any universal
binary packaging. A merged executable requires OpenSSL runtime libraries that
also support both architectures.

Install into a staging directory with:

```sh
cmake --install build/macos-arm64 --prefix "$PWD/stage"
```

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
