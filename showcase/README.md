# Linux SDK example catalog

Start with [Getting started](../docs/tutorials/getting_started.md), then follow
[the installed walkthrough](../docs/tutorials/guided_walkthrough.md).
[portfolio.json](portfolio.json) maps these runnable examples to their source
owners and installed qualification gates.

| Example                 | Source owner                                                            | Run path                                                 |
| ----------------------- | ----------------------------------------------------------------------- | -------------------------------------------------------- |
| Log summary             | [Application and library](log-summary/)                                 | [Summarize a log file](../docs/tutorials/log_summary.md) |
| Native application      | [main.objc3](../tests/clang_foundation/installed_sdk/main.objc3)        | [Compile and run](../docs/tutorials/build_run_verify.md) |
| Managed module boundary | [Managed sources](../tests/clang_foundation/native_package/)            | Installed `examples/managed/build.py`                    |
| Local package graph     | [workspace.json](../tests/clang_foundation/source_graph/workspace.json) | Installed `objc3c-build`                                 |

Each example is built from source by installed SDK qualification. The managed
example and package graph use bounded migration intrinsics where documented;
[the current capability schema](../schemas/objc3-language-capabilities.json)
defines public feature scope. Public async/await and actors remain reserved.
The SDK builds and tests the same log summary sources in both application forms.
The native application, managed modules, and package graph are SDK fixtures.
See the [tutorial index](../docs/tutorials/README.md).
