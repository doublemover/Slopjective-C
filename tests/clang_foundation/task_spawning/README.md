# Standard spawn source witnesses

These maintained sources are authored inputs for the deferred combined feature
driver. They have not been built or executed.

`contract.m` exercises C17 ARC/MRC child and detached direct/macro equivalence,
active actor inheritance, cancellation set before child creation, global
detached independence, native suspension cleanup, and outside-owner failed
start cleanup. A real Sendable object is pinned by both child and detached
native Blocks through suspension, then deallocated after group quiescence;
failed startup also releases its captured object. `contract.mm` covers C++20/23
typed results through ordinary C++ async task Blocks and explicit native
C-frame Blocks, typed joins, preserved
parent continuation affinity, and cancelled children. Both explicitly join their
owned group before releasing captured state.

`diagnostics.m` is a strict concurrency negative input: direct/macro spawn and
detached capture errors must point at the source invocation/argument, and both
ignored child handles must warn. The driver should also compile equivalent
source and module/PCH imports, with ARC/MRC and O0/O2. Module exports must retain
the canonical task attributes; macro expansion cannot replace them with an
unclassified frame helper.

The C convenience surface accepts void async Blocks with distinct nonthrowing
and bare-throws types. C++ overloads retain generic logical results and distinguish the two async entry
representations. Separate checked-effect overloads preserve bare checked throws
and C++ typed E errors; error-task witnesses inspect the original owned native
error object after join, then check its deallocation. The C convenience surface
has a distinct bare-throws void Block overload; typed C error effects remain on
the lower-level frame API boundary.

The native compiler now has authored deduction of ObjC3 checked ErrorType,
exact async/frame matching and used-template-parameter tracking, including
specialization and PCH witnesses. This closes the typed-E header dependency in
source; it has not been built or executed. The C++ raw-frame definitions now have bounded native frame-body admission
for value parameters and trivially copyable parameters/results. Neither effect
erasure nor automatic group-scope finalization is claimed here.
