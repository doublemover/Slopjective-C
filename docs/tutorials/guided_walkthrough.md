# Installed example walkthrough

Complete [Getting started](getting_started.md) and activate the SDK. These
examples build from installed source, so they also work outside the checkout.
Choose a fresh working directory and run:

```sh
python3 "$OBJC3C_SDK_ROOT/share/objc3c/examples/managed/build.py" \
  --output-dir ./managed-sample --optimization O2
cp -R "$OBJC3C_SDK_ROOT/share/objc3c/examples/packages" ./packages
objc3c-build --workspace ./packages/workspace.json \
  --output-dir ./package-build --optimization O2
```

The managed example compiles provider and consumer modules, emits semantic
interfaces and a locked dependency record, then links and runs the application.
It writes `managed-sample/build-report.json`. The package example builds a local
source graph with explicit module dependencies and runs its application.

The [managed sources](../../tests/clang_foundation/native_package/) and
[package workspace](../../tests/clang_foundation/source_graph/workspace.json)
are the repository owners of these installed examples. They demonstrate managed
ownership and interface/package boundaries; some features use bounded migration
intrinsics. Consult [the capability schema](../../schemas/objc3-language-capabilities.json)
for public feature spellings and qualified scope. Public async/await and actors
remain reserved.

For dependency invalidation and incremental reuse details, see the
[root README](../../README.md#offline-source-package-graphs). Return to the
[showcase catalog](../../showcase/README.md) or [tutorial index](README.md).
