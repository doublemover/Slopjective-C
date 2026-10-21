# Application and library packages

Use the [installed package walkthrough](guided_walkthrough.md) to build a native
application with explicit local library dependencies. The
[workspace source](../../tests/clang_foundation/source_graph/workspace.json)
is installed as `share/objc3c/examples/packages/workspace.json`.

`objc3c-build` consumes that workspace and produces compiler interfaces, objects,
dependency records and a linked application. Keep workspace sources and build
output separate. This is a local source graph, not a general package registry or
a platform application framework distribution.

The [root README](../../README.md#offline-source-package-graphs) describes graph
locking and reuse. Public async/await and actors remain reserved; consult the
[capability schema](../../schemas/objc3-language-capabilities.json) for support
scope. Return to the [tutorial index](README.md) or [showcase](../../showcase/README.md).
