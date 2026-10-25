# Linux explicit Block capture lists

Part 8.8 now has a native implementation in the shared development compiler.
The native compiler regressions and all 24 runtime configurations pass across
C17/C++20/C++23, ARC/MRC, O0/O2 and source/PCH transport, including exceptional
creation and return cleanup. Installed SDK qualification remains in progress;
these development checks do not establish profile conformance.

## Implemented ownership contract

`^[weak self, move handle] { ... }` is parsed before the ordinary Block
signature. `strong`, `weak`, `unowned` and `move` are contextual modifiers;
an item without a modifier selects strong/value capture. Identically named
ordinary variables remain usable as capture items. Duplicate bindings,
malformed items, nonlocal bindings and invalid ownership/type combinations are
diagnosed. Other ordinary captures in the body keep native implicit-capture
rules; owning by-value resource captures require an explicit move. An ordinary
`__block` resource capture shares the forwarded slot without copying its cleanup
obligation. The source's lexical cleanup closes and invalidates that slot, even
while a heap Block still references it. General shared-resource alias proof in
Strict System remains gated; concurrent shared `__block` capture diagnostics
remain enforced.

Explicit items register captures before parsing the body, including unused
items. Creation initializes fields in written order even when the physical
Block layout sorts fields. Captured values are snapshots, including explicit
captures of `__block` sources and C++ references. Body references use the
selected field ownership, rather than the source variable's ownership.

Strong object fields acquire ownership at creation. Weak fields register
zeroing weak storage under ARC; unowned fields remain unsafe, nonowning
references. Copy/dispose helpers use actual capture field types. Their identity
distinguishes explicit exception-cleanup guarantees from ordinary implicit
captures. Completed fields have native exception cleanup when a later capture
constructor or Block heap copy fails. Retain-cycle checking recognizes explicit
weak/unowned captures and unused strong captures of the receiving object.

Completed explicit fields have one shared live flag across construction
rollback, temporary-failure rollback and committed lexical disposal. A guard
below the full-expression temporary stack retains the rollback obligation until
temporary destruction finishes and permanent cleanups are installed. This also
covers ARC scalar initialization, ordinary return-expression field cleanup and
precise `keepAlive`/`withLifetime` receiver evaluation. Source-scope queues delay
committed implicit managed field releases until after that creation scope's
defers; the source variable's declaration scope does not determine the field's
owner. Resources and user C++ destructors retain their source-action LIFO order.

Ordinary Objective-C3 ARC physical retainable return slots now record the
originating return only after native `CK_ARCProduceObject` initialization proves
their +1 ownership. Native unwind and escaping checked errors release an
abandoned result. C++/Objective-C catches and checked handlers release it only
when their protected source body contains that return, so a handler inside a
defer can catch its own error and still finish the original return. The pending
site is cleared before release and before successful physical transfer or
epilogue autorelease. Retained-result annotations and `id`-erased Blocks follow
the same rule. Native async frames and task/coroutine logical results retain
their separate staging ownership and are excluded explicitly.

Moving an ARC strong object slot transfers its existing owned reference and
writes nil to the source. Moving a weak source acquires a strong snapshot and
clears its weak slot. Other scalar moves write the previous value into the
capture and leave a zero/null source. C++ object moves use a separately stored
creation-time move-constructor recipe; later heap copies use the ordinary const
copy constructor. This preserves the C++ type's own moved-from state. Move-only
C++ objects are rejected because an ordinary Block must remain copyable. A const, volatile, atomic or immutable
enclosing capture cannot be consumed. MRC raw pointer slots do not prove a
separately owned +1: their destination acquires a reference before source
nil-ing, and any independently held manual ownership remains the caller's
responsibility. The runtime witness covers that +0-alias case.

Qualified Linux ARC Optional/Result moves reuse the existing active-arm storage
operations in C17 and C++20/C++23. They move strong/weak leaves, clear remaining
selected scalar/unsafe leaves, and retain valid source discriminants. Nested
const, volatile or resource-contracted payloads and unqualified/mixed carrier
shapes remain rejected. Later Block copies use the existing owned-carrier copy
contract.

## Resource ownership across Block copies

An explicit move from a validated `objc_resource` local creates a private,
refcounted owner containing the native cleanup callback and typed handle.
Allocation and payload initialization precede the source's declared invalid
sentinel store. Allocation failure traps before consuming the source. Each
Block capture owns one reference to this owner. Heap copies retain the owner;
stack cleanup, failed-copy cleanup and final heap disposal release it. Only the
last owner closes the still-valid handle and frees storage. Copying the Block
therefore preserves one cleanup obligation rather than duplicating a handle.
Address-based cleanup callbacks receive the acquired handle through a typed
snapshot. Their validated argument conversions and calling convention remain
native. The final allocation has cleanup on native exception edges too.

`take`, `reset` and `discard` in the Block body resolve the original captured
operand through that owner payload. Independent copies therefore observe the
same sentinel changes. Reset/discard cleanup and exceptional invalidation use
this storage rather than synthesizing a reference to the enclosing source
local. A failed reset replacement leaves the captured owner invalid.

A nested explicit resource move transfers the current shared payload into a
new owner and invalidates the original owner. Copies of the outer Block all
observe that sentinel, while the nested Block owns the surviving cleanup
obligation independently of their lifetime. This mutation applies to the
resource owner's payload; ordinary const capture fields remain immutable.
Nested construction failure destroys the new completed owner once and leaves
the outer owner invalid. Import/PCH witnesses preserve the common original
declaration and both resource capture flags.

Admission currently requires a statically nonthrowing callback (`nothrow` or
`noexcept`). This avoids unwinding through ARC's nonthrowing release intrinsic.
Potentially throwing final-disposal callbacks, arbitrary `objc_cleanup`/GNU
cleanup variables without a resource sentinel, and over-aligned handles are
rejected explicitly. Object/record/carrier lifetime work already present in the
compiler remains in place.

Strict resource flow consumes capture moves at their individual CFG stages,
permits reinitialization and sentinel observation, and diagnoses use after move,
repeat moves and double cleanup, including the captured owner inside the Block
body. Capture flags are finalized before the completed-Block flow check. The
current strict exceptional-flow admission remains limited to simple explicit
lists; general C++ copy-constructor capture paths are a separate boundary.

## Persistence and tooling

`BlockDecl::Capture` retains the original declaration, ownership kind, resolved
logical field type, item source range and resource-owner flag. The declaration
also stores the list range. This preserves borrowed provenance and resource,
Sendable and actor contracts for the corresponding mandatory checkers.

PCH records, template rebuilding, native AST import, AST text/JSON dumps and
pretty printing preserve capture metadata. AST import now supports Block
expressions/declarations and their `ExprWithCleanups` ownership records. ODR
profiling and structural comparison bind the ordered capture contract and body.
Index references point at explicit item identifiers; moves carry read/write
roles. Debug information uses selected capture ownership and follows the owner
pointer to a resource payload.

## Authored witnesses and remaining work

[`explicit_capture_lists_test.cpp`](../../tests/clang_foundation/explicit_capture_lists_test.cpp)
contains native semantic diagnostics, strict consumed-state cases, AST import,
PCH and ODR-identity checks for Objective-C C17 and Objective-C++ C++20/C++23
under ARC/MRC. The
[`explicit_capture_lists`](../../tests/clang_foundation/explicit_capture_lists)
fixtures cover source/PCH runtime snapshots, contextual names, zeroing weak and
unsafe captures, moved-from values, heap copies sharing one resource, address
callbacks, sentinels, left-to-right C++ capture effects, construction failure,
heap-copy rollback and IR ownership operations. They are authored evidence,
not passing results.

Further fixtures cover captured resource operations, transferred owners,
deferred discard during unwind, ordinary/return/goto exits, nested creation
scopes, precise Block lifetime receivers and throwing full-expression
temporaries after object/resource/C++ moves. These remain unexecuted.
`return_failures.h`/`.m` and the C++ exception fixture additionally cover prepared
retained/autoreleased and `id`-erased Block results, failed return temporaries,
nested catches, and both escaping and locally handled deferred checked errors.

Remaining boundaries include unqualified/mixed/C++ carrier move captures,
potentially
throwing resource final disposal, unqualified foreign/raw return staging,
broad exceptional CFG capture admission and
end-to-end validation of the combined native compiler packets. Borrowed escape
and concurrent-transfer admission are implemented by their owning slices and
must be verified with these capture paths. Native source is exported and sealed
centrally by the root agent; no nested LLVM Git commit is part of this work.
