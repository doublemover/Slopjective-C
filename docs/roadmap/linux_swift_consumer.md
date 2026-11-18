# Independent Linux Swift consumer

Issue [#8292](https://github.com/doublemover/Slopjective-C/issues/8292) qualifies
selected C and C++ boundaries with the real Swift importer. The gate targets
native Debian 13 on x86-64 and AArch64 with Swift 6.4.2 installed separately
from the product SDK.
The source-owned manifest pins the official archive, detached signature,
release signing key and selected publisher-file inventory. Installation checks
the signature and fingerprint, then extracts only the compiler, linker and
runtime resources needed by this pilot. Rewriting a local installation seal
cannot admit substituted publisher files. Missing tools or another host fail;
required rows cannot silently disappear.

```sh
python3 -B -m scripts.objc3c_toolchain.swift_toolchain \
  --root /path/to/swift-6.4.2
python3 -B -m scripts.objc3c_toolchain.swift_consumer \
  --sdk /path/to/sealed-native-prefix --swift-root /path/to/swift-6.4.2 \
  --symbolizer /path/to/pinned-llvm-23.1.5/bin/llvm-symbolizer \
  --output out/swift-consumer
```

Each original self-contained header first passes independent Swift type checking.
The native interface emitter produces ordinary C and C++ declarations. Their
bytes and module identities survive re-emission unchanged. Byte-identical
header copies enter explicit Clang module maps. No product syntax is stripped,
and no product PCH, Clang resources or compiler libraries enter Swift's importer.
Swift uses its own bundled Clang 21 and resources. Exact importer dependency
files identify the selected header and module map in every positive case.

Each provider is compiled with product Clang at O0/O2. The Swift driver
links each original/emitted header variant at Onone/O and executes it, for 16
mixed-language witnesses. The C cases check primitive arguments/results, a named
newtype, nonnull pointer input and an explicit opaque C resource's live count
before creation, during use and after destruction. Availability is import-only;
it is deliberately unavailable for execution. Original provider/header roots
are hidden, inputs relocated, and module caches freshly created per invocation.
The C++20 cases pass a trivial, padded record by value through two nonthrowing
functions. Swift checks the layout, 32- and 64-bit field types, returned values
and original record. The Swift driver uses its bundle's signed `clang++` link.

Twelve controls cover independent original import, unmodified emitted import,
old spelling, wrong type, nullability, availability, changed public Swift name
and native fingerprint, a changed C++ field type, and missing provider symbols.
Reports retain compiler versions, targets, resources, loader graphs, input and artifact hashes, commands,
environments, diagnostics and exact results. Native and Swift installations and
loaded dependencies are verified again before promotion.

The existing Linux native assurance workflow selects this gate for foreign
emitter/module identity, foundation, Swift toolchain/fixture and runner changes,
unknown comparisons, and its complete periodic/release selection. The terminal
gate independently requires 16 executions, 12 controls, six admitted
rows, relocation facts and execution oracles. It prepares a sealed development
tool prefix from current native build products; it builds no SDK archive and
adds no schedule. Ordinary unrelated stdlib edits do not select Swift.

The expanded matrix passed all 16 executions and 12 controls on Debian 13 x86-64
with the LLVM 23.1.5 development SDK. The AArch64 archive has a separately
verified publisher inventory and its native execution remains in hosted
qualification. Selected Swift files occupy about 1 GiB. The installer discards
the temporary signed download after installation.
The native build is shared with the existing workflow, and uploaded evidence
excludes the development prefix. Timing is operational evidence, not a Swift
performance benchmark. Requalify the complete pilot on toolchain/foundation
upgrades; do not expand its row inventory without an independent positive and
appropriate negative/execution controls.

The admitted declarations are recorded in `tests/foreign/swift_consumer/rows.json`.
GNUstep Objective-C object import, Swift async/actor/error bridging, arbitrary
foreign attributes, owning C++ types and product serialized AST interchange
remain open.
