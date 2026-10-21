# Build, run and verify

First [build or install and activate the SDK](getting_started.md). Run from the
repository root:

```sh
objc3c tests/clang_foundation/installed_sdk/main.objc3 -O2 -g -MMD -MF application.d -o application
./application
```

Expect `native Linux application` and exit status zero. `application.d` records
source/header dependencies. `objc3c --help` lists Clang driver options; `-c`,
`-S`, `-E` and `-fsyntax-only` select individual compilation stages.

The installed SDK qualification compiles this same source at O0 and O2 and runs
both executables outside the checkout. The installed managed module and package
graph gates exercise the next [walkthrough](guided_walkthrough.md) examples.
Receipts under `out/toolchains/linux-sdk-builds/` identify the inputs and checks
for a specific build. Keep application output separate from sources and the SDK.

For repository maintainers, the current documentation guard is:

```sh
python -m scripts.objc3c_toolchain.check_onboarding
```

See [tutorial navigation](README.md) and the [showcase catalog](../../showcase/README.md).
