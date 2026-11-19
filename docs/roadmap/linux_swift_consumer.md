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
The C++20 cases pass a trivial, padded record and a copyable heap-owning value
by value through nonthrowing functions. Swift checks the record's layout and
field types, and it observes distinct original and adjusted owners. The owning
case requires a native copy, verifies two live owners together, and confirms
that every allocation is destroyed. A move-only value crosses a `consume`
expression and a C++ by-value call. It moves without a second allocation and
destroys the original allocation once. The Swift driver uses its bundle's
signed `clang++` link.

Fourteen C and C++ controls cover independent original import, unmodified emitted import,
old spelling, wrong type, nullability, availability, changed public Swift name
and native fingerprint, a changed C++ field type, a deleted owning destructor,
and missing provider symbols. The deleted destructor changes native interface
identity and the independent importer drops the unusable owning value. A
Swift `Copyable` generic constraint rejects the imported move-only value.
Source 2.277 / System 2.203 adds eight Swift executions of generated C task and
actor completions. The native providers use ARC or MRC and O0 or O2. Swift uses
Onone or O, imports one byte-identical generated header after provider source
removal, and verifies the loaded GNUstep runtime by its installed file hash.

The Swift consumer uses a checked continuation with the caller's existing actor
isolation. Native success returns a scalar. A typed Objective-C error is
translated to a Swift error while its original object is live in the callback.
The native cancellation object maps to Swift `CancellationError`. Opaque actor
receivers dispatch through the native actor executor and keep independent state.
An additional type-checking control rejects a null receiver.

Source 2.280 / System 2.206 uses generated task handles to propagate a Swift
task's cancellation to the same native task that will deliver the callback.
The Swift handler records cancellation that arrives before it receives the
handle and keeps a retained reference while it requests cancellation. The
callback owns continuation completion. The positive cases cancel before
submission and after the handle is installed. They also release the handle of
a successful task immediately and observe its normal callback. All eight
ARC/MRC and optimization combinations pass on x86-64.

Source 2.289 / System 2.215 generates Swift source for selected checked and
async C entries. Pass `--emit-c-header Generated.h --emit-swift-overlay
Generated.swift --swift-c-module ForeignAsync` alongside the existing
`--export-c-throws` or `--export-c-async` selections. Expose the generated
C header in the named Clang module and compile the Swift file with Swift 6.4.2.
The overlay adds typed `throws` and `async throws` entries for C scalars,
explicitly represented named enums and `Void`. Async actor entries take a
borrowed opaque receiver and dispatch through its native mailbox.

Checked failures retain the original native error so Swift can inspect its
identity after the native callback or autorelease pool ends. Synchronous calls
drain their autorelease pool on the calling thread. Async wrappers translate
recognized native cancellation to `CancellationError` and keep the retained
native task alive while a Swift cancellation handler requests cancellation.
The eight x86-64 executions of the generated source cover the scalar ABI,
enum transport, checked failure and exact destruction, pre-submission
cancellation and actor isolation with the provider source removed. The gate
retains the overlay source and verifies the same digest in every native row.

Source 2.291 / System 2.217 adds pointer-free C structs to the generated
checked and async entries. Swift imports the same C definitions, including
nested structs, fixed enums and inline arrays. The native generator checks
every field before using a struct as a Swift continuation value. The C
boundary retains its independent layout checks. The same eight Swift
configurations verify by-value arguments and indirect results with a nested
enum and four-word array. A native control admits a record with nested
pointers to C and rejects it at the Swift boundary.

Reports retain compiler versions, targets, resources, loader graphs, input and
artifact hashes, commands, environments, diagnostics and exact results. The
gate rechecks native and Swift installations and the loaded dependencies.

The existing Linux native assurance workflow selects this gate for foreign
emitter/module identity, foundation, Swift toolchain/fixture and runner changes,
unknown comparisons, and its complete periodic/release selection. The terminal
gate independently requires 24 executions, 15 controls, twelve admitted
rows, relocation facts and execution oracles. It seals the compiler and runtime
inputs from current native build products for the existing qualification job.
Ordinary unrelated stdlib edits do not select Swift.

The expanded matrix passed all 24 executions and 15 controls on Debian 13 x86-64
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
Direct GNUstep Objective-C object import, async and actor synthesis in the Swift
importer, arbitrary foreign attributes, throwing C++ value contracts and
serialized AST interchange remain open.
