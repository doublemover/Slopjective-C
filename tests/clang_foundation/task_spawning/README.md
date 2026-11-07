# Standard spawn source witnesses

All 24 source/PCH execution configurations pass normally and under ASan/UBSan
and TSan; the installed SDK feature gate also passes. See the
[integration receipt](../../../docs/support/linux_native_feature_integration_acceptance.json)
for the exact inputs and scope.

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
ignored child handles must warn. The driver compiles equivalent
source and PCH imports, with ARC/MRC and O0/O2. Module exports must retain
the canonical task attributes; macro expansion cannot replace them with an
unclassified frame helper.

The C convenience surface accepts void async Blocks with distinct nonthrowing
and bare-throws types. C++ overloads retain generic logical results and distinguish the two async entry
representations. Separate checked-effect overloads preserve bare checked throws
and C++ typed E errors; error-task witnesses inspect the original owned native
error object after join, then check its deallocation. The C convenience surface
has a distinct bare-throws void Block overload; typed C error effects remain on
the lower-level frame API boundary.

The native compiler implements deduction of ObjC3 checked ErrorType,
exact async/frame matching and used-template-parameter tracking, with passing
specialization and PCH witnesses. This closes the typed-E header dependency.
The C++ raw-frame definitions have bounded native frame-body admission
for value parameters and trivially copyable parameters/results. Neither effect
erasure nor automatic group-scope finalization is claimed here.
