# Typed property values and native frame exception transport

Concurrency source 2.118 admits nontrivial and move-only C++ property writes
through the native frame storage hooks. Copy/move initialization uses ordinary
Sema construction, then independently owned frame storage and native setter
arguments. Deleted/inaccessible operations and throwing destruction retain their
existing diagnostics. Nontrivial setter assignments preserve their ordinary void
result; they do not fabricate a copied return value.

A native ordinary-property oracle exposed the distinction between captured
assignment results and nontrivial setter arguments. The latter construct after
receiver evaluation. Isolated lowering now preserves that order, including
receiver mutation and constructor failure. Qualified record-graph checks memoize
repeated subgraphs while charging traversed edges and retaining alias/resource,
union-arm and depth limits. A nine-level shared graph defends admission.

Native C++ frame bodies and synchronous isolation entries now retain escaping
exceptions in the SDK's owned error carrier. C++ consumers rethrow the same
exception object, with dynamic type and identity. C++-only EH works independently
of Objective-C EH. Source handlers and lexical cleanup run first; explicit task
cancellation stays cancellation, existing task errors preserve their native
handle, and other exceptions cannot masquerade as cancellation. GNUstep
Objective-C++ exceptions retain their original object. Unrepresentable foreign
exceptions produce INVALID_ARGUMENT. Error-payload allocation failure produces
an allocation-independent OUT_OF_MEMORY owner. The callback fallback follows the
same outcome policy. The compiler never assumes an exception_ptr layout.

Potentially throwing isolated entries require enabled C++ EH and the qualified
SDK bridge. Publication retains that SDK dependency and the producer's exception
settings during validation. Reconstructed source, relocation and dependent
consumers preserve behavior and fingerprints. The hard source-free cutover is
`objc3.async-entry-abi.v5` across writers, readers, importers and the codec fuzz
target; older readers reject it. Extension ABI 24, frame ABI 1 and service ABI 4
retain their physical layouts.

Verification passes thirty new compiler commands and all ninety-two selected
async/isolation/frame/storage/defer compiler cases. Four new source-free C++
profiles and four adjacent frontend groups pass, as do codec and complete
isolation contract controls. Fourteen ASan/UBSan native configurations pass
forty-nine compile/PCH/link/execution commands, including C++-only EH. Exact
object/exception destruction, ordinary ordering, throwing copies/moves/setters,
nil values, pre-entry cancellation, abandonment and controlled allocation failure
are maintained in `native_frame_exceptions`; optional `--feature exceptions`
keeps the full 24-case normal O0/O2 matrix installed with the SDK.

The existing async fixture also now requires ERROR and exact defer cleanup for a
foreign throw, and its runtime allocation/disposal declarations have C linkage in
Objective-C++. Four existing C17/C++23 source/PCH ownership configurations pass
nineteen instrumented commands. These are current development-source checks.

Resource property writes, async/checked-effect accessors, broader ownership and
profile obligations remain in the implementation backlog. The accepted SDK 2.62
is historical; one matched replacement SDK follows consolidated implementation
and verification.
