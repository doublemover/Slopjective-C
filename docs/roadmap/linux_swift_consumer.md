# Independent Swift consumer pilot

Issue [#8292](https://github.com/doublemover/Slopjective-C/issues/8292) qualifies a
small C boundary with the real Swift importer. The admitted pair is native
Debian 13 x86-64 and Swift 6.4.1, installed separately from the product SDK.
The source-owned manifest pins the official archive, detached signature,
release signing key and selected publisher-file inventory. Installation checks
the signature and fingerprint, then extracts only the compiler, linker and
runtime resources needed by this pilot. Rewriting a local installation seal
cannot admit substituted publisher files. Missing tools or another host fail;
required rows cannot silently disappear.

```sh
python3 -B -m scripts.objc3c_toolchain.swift_toolchain \
  --root /path/to/swift-6.4.1
python3 -B -m scripts.objc3c_toolchain.swift_consumer \
  --sdk /path/to/sealed-native-prefix --swift-root /path/to/swift-6.4.1 \
  --symbolizer /path/to/pinned-llvm-23.1.3/bin/llvm-symbolizer \
  --output out/swift-consumer
```

The original self-contained header first passes independent Swift type checking.
The existing public native interface emitter produces ordinary C declarations;
their bytes and module identity survive re-emission unchanged. Byte-identical
header copies enter explicit Clang module maps. No product syntax is stripped,
and no product PCH, Clang resources or compiler libraries enter Swift's importer.
Swift uses its own bundled Clang 21 and resources. Exact importer dependency
files identify the selected header and module map in every positive case.

The provider is compiled with product Clang at O0/O2. The ordinary Swift driver
links each original/emitted header variant at Onone/O and executes it, for eight
mixed-language witnesses. They check primitive arguments/results, a named
newtype, nonnull pointer input and an explicit opaque C resource's live count
before creation, during use and after destruction. Availability is import-only;
it is deliberately unavailable for execution. Original provider/header roots
are hidden, inputs relocated, and module caches freshly created per invocation.

Eight controls cover independent original import, unmodified emitted import,
old spelling, wrong type, nullability, availability, changed public Swift name
and native fingerprint, and missing provider symbols. Reports retain compiler
versions/target/resources, loader graphs, input and artifact hashes, commands,
environments, diagnostics and exact results. Native and Swift installations and
loaded dependencies are verified again before promotion.

The existing Linux native assurance workflow selects this gate for foreign
emitter/module identity, foundation, Swift toolchain/fixture and runner changes,
unknown comparisons, and its complete periodic/release selection. The terminal
gate independently requires all eight cases, eight controls, five admitted
rows, relocation facts and execution oracles. It prepares a sealed development
tool prefix from current native build products; it builds no SDK archive and
adds no schedule. Ordinary unrelated stdlib edits do not select Swift.

The measured pilot completed on the current instrumented 616ca86288bb foundation
with eight executions/eight controls passing. Selected Swift files occupy about
1 GiB; the signed download is 1.05 GiB and is discarded after installation.
The native build is shared with the existing workflow, and uploaded evidence
excludes the development prefix. Timing is operational evidence, not a Swift
performance benchmark. Requalify the complete pilot on toolchain/foundation
upgrades; do not expand its row inventory without an independent positive and
appropriate negative/execution controls.

GNUstep Objective-C object import, Swift async/actor/error bridging, arbitrary
foreign attributes, C++ publication and product serialized AST interchange
remain unqualified. This pilot establishes the exact C declarations in
`tests/foreign/swift_consumer/rows.json`; it does not promote the broader Swift
interoperability roadmap.
