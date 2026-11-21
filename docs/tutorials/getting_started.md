# Getting started on Linux

For an SDK preview, start with the [Linux release and install guide](../releases/linux.md).
Use the archive and distribution listed for your architecture in its release notes.
The [root README](../../README.md#build-and-use) lists the pinned development tools.
If you are building the compiler from a configured Linux checkout:

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

Continue with [Build, run and verify](build_run_verify.md), then the
[guided walkthrough](guided_walkthrough.md) for installed managed modules and
package graphs. The [showcase catalog](../../showcase/README.md) links their sources.

[The capability schema](../../schemas/objc3-language-capabilities.json) is the
feature authority. Public `defer`, `Optional`, `Result` and typed `throws` have
qualified support. Public async/await and actors remain reserved. Internal
migration intrinsics in advanced SDK fixtures are experimental implementation
interfaces, not additional public syntax.
