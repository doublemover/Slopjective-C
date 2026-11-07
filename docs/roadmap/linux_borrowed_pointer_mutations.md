# Borrowed pointer mutations and representation writes

System source 2.23 models built-in pointer increments/decrements, compound
assignments and their expression results in the native borrowed-memory CFG.
Prefix results use the advanced target; postfix results preserve the previous
address. C++ prefix/assignment lvalues still designate the mutated binding.
Constant, commuted and conditional array offsets retain complete target sets,
including bounded one-past addresses and subsequent reverse traversal. Unknown
steps and mismatched reinterpret/character strides retain conservative may facts.

Partial character writes, scalar compound updates and increments can change a
pointer representation. They invalidate target and nonnull proofs for every
possible root, preserve loan/callable payloads and expire affected owner claims.
A mismatched typed write cannot erase the whole stored pointer's loan. An
unproven indirect loan store diagnoses rather than reusing an earlier address.
Exact typed assignments retain their existing field-sensitive strong updates.

Nonnull address facts are independent of complete non-null target sets: a closed
set can still contain a null alternative. CFG joins, copies, captures, expression
snapshots, fills and invalidations preserve that distinction. Pointer comparisons
refine only elements of the same nonzero-sized constant array against a complete
nonnull endpoint. This bounds forward/reverse cursor loops without confusing
union arms, adjacent arrays or overlapping C++ subobjects. Nullable endpoints do
not remove a possible element from a comparison's false branch.

The new regression executes eighteen source/PCH commands across C17 and
C++20/23, ARC/MRC. It covers partial writes, original false-negative cases, owner
expiry, callable consumption, pre/post increments/decrements, compound and nested
lvalue results, conditional aliases, dynamic/wrong strides, one-past/reverse
traversal, bounded loops, nullable endpoints and union comparisons. All existing
twenty-two borrowed compiler regressions pass. The source-free interface control
passes six source-free modes with replayed cursor/walk/clear bodies and invalid
escaping/partial-write consumers.

This is development-source verification. The accepted SDK 2.62 archive has not
been rebuilt; package/runtime matrices and matched SDK delivery remain in the
consolidation stage. Stored byte-cursor precision, broader union/interprocedural
and callable/exceptional ownership contracts remain in the completion ledger.
