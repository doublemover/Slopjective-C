# Objective-C 3.0

Slopjective-C develops a native Objective-C 3 compiler on a patched LLVM/Clang
SDK. `objc3c` is the public compiler. It uses Clang's parser, semantic analysis,
AST, ownership model, native CodeGen and linker driver. Linux x86-64 with the
GNUstep 2.0 ABI and pinned libobjc2 runtime is the active distribution target.

## Build and use

The authoritative versions are in
[`toolchains/llvm/manifest.json`](toolchains/llvm/manifest.json). The current
foundation is LLVM 23.1.3. Development requires Python 3.15.0, Node 26.12.0,
npm 12.2.1, CMake 4.4.4 and Ninja 1.13.2. A compiler cache is recommended.
PowerShell is not part of the Linux compiler build or qualification pipeline.

In the managed Linux development environment:

```sh
bash scripts/setup_cloud_environment.sh
source /workspace/.cloud-setup/Slopjective-C/activate.sh
npm run objc3c -- build-sdk --fetch
source out/toolchains/linux-sdk/activate.sh
objc3c tests/clang_foundation/installed_sdk/main.objc3 -O2 -g -o application
./application
```

`build-sdk` builds and tests the sealed LLVM fork, pinned native runtime and
frontend; installs an isolated candidate; qualifies the installed compiler,
embedding API, semantic interfaces, runtime and editor tools; creates a sealed
archive; and publishes a content-addressed installation. Logs and phase timings
live under `out/toolchains/linux-sdk-builds/`. Incremental Ninja builds and a
persistent compiler cache are reused. The product SDK and the bootstrap compiler
have separate roots; `OBJC3C_BOOTSTRAP_LLVM_ROOT` pins the latter.

The compiler accepts normal Clang compilation, linking, diagnostics and
Makefile dependency options. `.objc3` selects Objective-C 3 with ARC and Blocks.
Use `-fno-objc-arc` for explicit manual ownership, `.mm` for Objective-C++, and
`-c`, `-S`, `-E` or `-fsyntax-only` for individual compilation stages.

```sh
objc3c main.objc3 helpers.c -O2 -MMD -MF application.d -o application
objc3c library.m --objc3 --arc --emit-object library.o \
  --emit-interface Library.objc3i --module Library --package example.library
```

Semantic interface imports use locked module identities and can publish a
machine-readable dependency record. The same in-process frontend is available
through the installed C embedding API and `find_package(objc3c CONFIG)`:

```cmake
find_package(objc3c REQUIRED CONFIG)
target_link_libraries(my_embedding PRIVATE objc3c::Frontend)
```

Pass the installation through `CMAKE_PREFIX_PATH`. See the
[frontend API and interfaces](native/objc3c/src/clang_foundation/README.md) and
[LLVM foundation workflow](toolchains/llvm/README.md) for details.

## Install a release archive

An installation needs no checkout, Node, PowerShell or development Python.
On an apt-based Linux host, the release installer can provision its distribution
Python, decompressor, system headers and loader dependencies:

```sh
sh install-linux.sh --provision --archive objc3-sdk.tar.zst \
  --sha256 SHA256_FROM_RELEASE_RECEIPT --root /opt/objc3c
. /opt/objc3c/activate.sh
objc3c --version
```

Run provisioning as root; omit `--provision` when prerequisites are installed.
Release installers are shipped under `share/objc3c/` in the archive and alongside
CI release artifacts. Installations live under `sdks/<full-content-seal>`;
activation atomically switches `current`. Existing prefixes are verified before
reuse. The standalone verifier checks all file contents, modes and relative
symbolic links after extraction or relocation.

The current binary qualification baseline is Debian 13, x86-64, glibc 2.41 and
libstdc++ with GLIBCXX_3.4.33. The shipped archive is also qualified on Ubuntu 26.04. Other distributions
require their own installed archive execution evidence before being declared supported. Windows, Darwin,
other architectures and older Linux runtimes are outside this milestone.

## Language status

[`schemas/objc3-language-capabilities.json`](schemas/objc3-language-capabilities.json)
is the authority for public extensions; its generated
[Clang capability report](docs/support/clang_foundation_capabilities.json)
and compiler feature queries must agree. Public `defer { ... }` is qualified for
Linux x86-64, GNUstep 2.0, compatibility profile, with ARC and MRC. Cleanup runs
in reverse registration order on returns, structured exits and native exception
unwinding.

```objc
#include <stdio.h>
int main(void) {
  FILE *file = tmpfile();
  if (!file) return 1;
  defer { fclose(file); }
  fputs("Objective-C 3 on Linux\n", file);
  return 0;
}
```

Optional/Result, expression matches and typed error transport have bounded
native implementation and migration fixtures. Public Optional/Result and
synchronous throws syntax, managed payload generalization, async/await and
actors still require further implementation and qualification. Private native
fixtures do not make these public features available. Generic callable metadata
and interfaces have explicit supported subsets; they do not establish complete
language-evolution readiness.

The former custom parser, `fn` module dialect and its historical capability
inventory no longer describe the public compiler. Historical roadmap and
support reports remain in the repository as records of earlier work.

## Validation and development

```sh
python -m scripts.objc3c_toolchain check-host-tools --runtime
python -m scripts.objc3c_native_validation
python -m pytest -q tests/tooling/test_toolchain_package.py
python -m scripts.objc3c_toolchain.generate_features --check
```

Native validation checks O0/O2 behavior, ABI, ownership, cleanup, expected
failures, installed interface consumers and editor tools. Evidence records bind
source, compiler, runtime, system headers and executable contents and reject
changes during validation. The
[Linux SDK acceptance receipt](docs/support/linux_sdk_release_acceptance.json)
records the qualified archive and fresh-host checks; each new SDK has its own
qualification and release receipt. Runtime timings must be distinguished from compile and wrapper
timings when comparing performance.
