# Native reference-result objects and caller lifetimes

Source 2.171 / System 2.97 extends bounded synchronous borrowed-result proofs
under `objc3.borrowed-result-proofs.v11`. Extension ABI 24 and public runtime
layouts stay unchanged; older semantic readers must rebuild their interfaces.

Pointer, callback and member-function-pointer reference results retain two
independent obligations: the pointer cell must survive the return, and the
loaded value must prove a permanent pointee or a complete known target graph.
Immutable record fields and by-value closure fields now retain both facts through
exact member/operator calls, nested forwarding, conditional returns and const
record-reference aliases. Value-capture facts belong to the actual closure
object, including nested closure copies.

A helper may return a reference into permanent immutable storage. The final
borrowed-value operation may also copy a pointer from an actual const object or
slot still alive in the active caller. The caller's return type must be a value;
this rule cannot return a reference to its own automatic storage. A nested
callee's automatic object has already died before the caller loads its result,
so dangling local fields, pointer slots and closure captures retain rejection.
Const-record local aliases preserve the underlying object's exact lifetime;
a const view cannot turn a mutable complete object into an immutable one.

Only the actual bounded result proof can forward a read-only pointer reference
as a reference result. Generic read-only-use scans retain their returned-alias
boundary: returning an alias must not hide a subsequent cast/write into a mutable
source pointer slot. Conditional and branch results retain every selected input.
The value and object-lifetime proofs share the existing work/depth/target bounds.

Unknown/mutable/volatile/TLS or replaceable receiver state, reference captures,
unproved temporary/heap objects, lazy async/coroutine result graphs and open
callable targets retain their separate contracts. This span does not implement
general mutation summaries or an unrestricted interprocedural lifetime analysis.

Controls cover direct/PCH C17 and C++20/23 ARC/MRC, original and dangling reference
graphs, import after source-AST destruction, publication/reparse and two native
consumers after provider removal. Native checks observe exact call side effects
and verify that a returned field reference names the original global pointer
cell. Matching SDK/profile/instrumentation and host acceptance remain pending.

The focused checkpoint passes all 18 expanded source/PCH commands, all 87
maintained LLVM AST infrastructure tests, six C17/C++20/23 ARC/MRC publication
profiles, four source-free native storage configurations, decoder controls and
25 related tooling checks. Native execution verifies fifteen reference/getter
observations per consumer and exact global field-reference identity. Fresh
maintained patch application reproduces the changed compiler/control files and
all manifest digests agree. Changed LLVM objects are relinked against the existing
SDK libraries; a new sealed SDK acceptance result remains pending.
