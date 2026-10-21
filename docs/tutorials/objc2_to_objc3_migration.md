# Moving Objective-C code to the Linux SDK

Start with [Getting started](getting_started.md). The current compiler preserves
Clang's Objective-C declarations, methods, message sends, properties, protocols,
categories, Blocks and ownership semantics on the GNUstep 2.2 ABI.

Compile `.objc3` source with `objc3c`; ARC and Blocks are enabled by default.
Existing `.m` input can select the language mode with `--objc3 --arc`.
Use `-fno-objc-arc` when code deliberately uses manual ownership. Objective-C++
uses `.mm` and an explicit C++ standard where needed.

Public `defer`, `Optional`, `Result` and typed `throws` are available within the
[capability schema's qualified scope](../../schemas/objc3-language-capabilities.json).
Migrate one source/module boundary at a time and inspect compiler diagnostics;
there is no source translation command. Public async/await and actors remain
reserved. The [installed managed example](guided_walkthrough.md) demonstrates
provider/consumer interfaces, ARC/MRC boundaries and native linking.

See [the compiler API and interface guide](../../native/objc3c/src/clang_foundation/README.md)
for semantic interface emission and imports, and the [tutorial index](README.md).
