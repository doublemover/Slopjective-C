# C++ reference bindings and native-frame receivers

The concurrency 2.7.0 source span admits borrowed C++ reference parameters and
implicit receivers in explicit `__objc3_async_frame` definitions. This matches
the existing ordinary C++ task-coroutine lifetime contract. The accepted SDK is
still the [2.6.0 dynamic-storage delivery](linux_dynamic_frame_storage.md) until
coordinated delivery of this span completes. Service ABI 4, frame ABI 1,
extension ABI 21 and semantic interface schema 2.1 are unchanged.

Each reference parameter gets an independent pointer slot in the lazy frame.
The factory copies the binding before initial suspension. It does not construct,
move, retain or destroy the referred object. This includes lvalue/rvalue,
const/volatile, array, function and pointer-reference parameters. A nested native
await forwards the same referent. Unstarted disposal and pre-entry cancellation
leave the referred object untouched.

Implicit C++ `this` remains the ABI-adjusted borrowed receiver. Coroutine
splitting preserves its value across suspension, including virtual calls through
secondary-base adjustment thunks and member pointers. Const/ref qualifiers and
member operators retain their ordinary native identity. A C++23 explicit-object
parameter passed by value instead uses the existing independent value capture,
including move-only objects; a reference explicit-object parameter is borrowed.

Callers must keep borrowed referents alive until the task and frame have fully
quiesced, including registered child-group cleanup. A reference binding does not
extend a temporary's lifetime. Cancellation does not transfer ownership to the
frame. These rules do not provide actor isolation or Sendable guarantees, and
full profile conformance remains reserved.

Captured parameter debug locations follow the independent frame storage while
preserving their source argument identities. Resumed reference bindings and
owned value captures remain inspectable after the ramp has returned. A focused
IR check requires the frame-slot declaration; DWARF verification and LLDB inspect
both `this` and a reference argument after suspension and evaluate their sum.

Reference results remain rejected by the shared value/void task-result contract.
Nontrivial class semantic-package projections retain their separate bounds.
Source-free package witnesses extend the existing public aggregate/member
projection with native-frame reference parameters and const/ref-qualified
receivers; they preserve physical `c-frame-v1` and module-owned symbol identities.

Maintained witnesses cover mutation before first resume and between suspensions,
deleted-copy referents, rvalue forwarding, exact destruction counts, cancellation,
unstarted disposal, arrays, function references, pointer rebinding, overloaded
calls, secondary-base virtual dispatch, borrowed group joins and owned C++23
explicit-object receivers. Compiler fixtures cover native IR, PCH replay and
reference-result rejection. Coordinated normal and sanitizer validation is in
progress; package execution follows with the rebuilt SDK.
