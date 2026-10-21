# Linux SDK tutorials

Start with [Getting started](getting_started.md), then follow
[Build, run and verify](build_run_verify.md) and the
[guided walkthrough](guided_walkthrough.md).

| Goal                                | Example                                                                                  |
| ----------------------------------- | ---------------------------------------------------------------------------------------- |
| Compile a native application        | [Installed SDK smoke application](../../tests/clang_foundation/installed_sdk/main.objc3) |
| Share managed APIs across modules   | [Managed source example](../../tests/clang_foundation/native_package/)                   |
| Build a local package graph         | [Package workspace](../../tests/clang_foundation/source_graph/workspace.json)            |
| Choose an example                   | [Showcase catalog](../../showcase/README.md)                                             |
| Move existing Objective-C code      | [Migration guide](objc2_to_objc3_migration.md)                                           |
| Compare language boundaries         | [Objective-C, Swift and C++](objc2_swift_cpp_comparison.md)                              |
| Organize applications and libraries | [Application packages](application-framework-samples.md)                                 |

The current target is Linux x86-64 with the GNUstep 2.2 ABI. Feature status comes
from [the language capability schema](../../schemas/objc3-language-capabilities.json).
Public async/await and actors remain reserved; experimental runtime examples do
not widen that public language boundary.
