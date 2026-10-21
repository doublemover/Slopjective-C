# Getting started on Linux

Use the native Clang-based SDK on Linux x86-64. The
[root README](../../README.md#build-and-use) lists pinned development prerequisites.
From a configured repository checkout, build and activate the SDK:

```sh
python -m scripts.objc3c_toolchain build-sdk --fetch
. out/toolchains/linux-sdk/activate.sh
objc3c --version
objc3c tests/clang_foundation/installed_sdk/main.objc3 -O2 -g -o application
./application
```

The application prints `native Linux application` and exits successfully. It
exercises a managed Objective-C object, ARC, Blocks-enabled language mode and
`defer` cleanup. `.objc3` selects Objective-C 3 with ARC and Blocks; use
`-fno-objc-arc` for manual ownership.

For a release archive, follow [Install a release archive](../../README.md#install-a-release-archive)
and activate that installation before the compile command. Building an SDK is a
maintainer operation; using an installed release requires no compiler rebuild.

Continue with [Build, run and verify](build_run_verify.md), then the
[guided walkthrough](guided_walkthrough.md) for installed managed modules and
package graphs. The [showcase catalog](../../showcase/README.md) links their sources.

[The capability schema](../../schemas/objc3-language-capabilities.json) is the
feature authority. Public `defer`, `Optional`, `Result` and typed `throws` have
qualified support. Public async/await and actors remain reserved. Internal
migration intrinsics in advanced SDK fixtures are experimental implementation
interfaces, not additional public syntax.
