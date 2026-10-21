# Objective-C, Swift and C++ boundaries

The current Linux compiler uses Clang's Objective-C and Objective-C++ model:
Objective-C message sends and ownership coexist with native C and C++ calls.
It does not provide Swift source compatibility or a Swift runtime distribution.

| Area                      | Current Linux SDK boundary                                            |
| ------------------------- | --------------------------------------------------------------------- |
| Objective-C objects       | GNUstep 2.2 ABI, pinned libobjc2, ARC or manual ownership             |
| C and C++                 | Native Clang compilation and linking; Objective-C++ uses `.mm`        |
| Managed cross-module APIs | Semantic interfaces and explicit package graphs                       |
| Errors and value carriers | Public typed `throws`, `Optional` and `Result` within qualified scope |
| Concurrency syntax        | Public async/await and actors remain reserved                         |

The [capability schema](../../schemas/objc3-language-capabilities.json) records
feature scope. Experimental C/C++ runtime task adapters and reserved migration
intrinsics do not imply public concurrency syntax or compiler-enforced actor
isolation. Try the [installed walkthrough](guided_walkthrough.md), or return to
[Getting started](getting_started.md).
