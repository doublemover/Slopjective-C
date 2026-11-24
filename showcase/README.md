# Linux SDK example catalog

Start with [Getting started](../docs/tutorials/getting_started.md), then follow
[the installed walkthrough](../docs/tutorials/guided_walkthrough.md).
[portfolio.json](portfolio.json) maps these runnable examples to their source
owners and installed qualification gates.

| Example                  | Source owner                                                            | Run path                                                               |
| ------------------------ | ----------------------------------------------------------------------- | ---------------------------------------------------------------------- |
| Log summary              | [Application and library](log-summary/)                                 | [Summarize a log file](../docs/tutorials/log_summary.md)               |
| Native application       | [main.objc3](../tests/clang_foundation/installed_sdk/main.objc3)        | [Compile and run](../docs/tutorials/build_run_verify.md)               |
| Managed module boundary  | [Managed sources](../tests/clang_foundation/native_package/)            | Installed `examples/managed/build.py`                                  |
| Local package graph      | [workspace.json](../tests/clang_foundation/source_graph/workspace.json) | Installed `objc3c-build`                                               |
| Index UTF-8 text lengths | [Collections and text source](collections-text/)                        | [Collections and text tutorial](../docs/tutorials/collections_text.md) |
| Quote a parcel batch     | [Objective-C 3 package and C++20 client](parcel-quotes/)                | [Publish and consume the quote API](parcel-quotes/README.md)           |

SDK qualification builds the native application, managed modules, package
graph, and log summary. The published SDK examples workflow also replays the
collections and text program and the source-free parcel quote consumer.
The managed example and package graph use documented migration intrinsics.
[The current capability schema](../schemas/objc3-language-capabilities.json)
defines public feature scope. Public async/await and actors remain reserved.
See the [tutorial index](../docs/tutorials/README.md).
