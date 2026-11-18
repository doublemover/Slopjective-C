# Native field projection provenance

Source 2.216 / System 2.142 retains cleanup/resource provenance when a native
C++ method returns a field whose plain scalar type does not carry its owner's
local cleanup attribute. A getter is a projection only when a visible closed
body returns one field or another proved getter through its receiver. Nested
record fields and member-helper chains share 256 work units, a 16-level depth
limit and an active-definition cycle check. Qualified virtual calls and final
methods/classes select a closed target.

The proof carries both field type/storage contracts and the caller's object
storage contract. Scalar copies, references, aggregate initialization, direct
field assignment, accessor reference assignment and conditional reference alias
sets retain all possible managed sources. The existing bounded storage analysis
collapses native object fields to their containing local object cell; it is a
conservative aggregate union, rather than field-sensitive overwrite analysis.

Boolean conversion and computed status observations do not copy resource
identity. Ordinary scalar getters and ordinary reference writes remain usable.
Opaque/stateful/multiple-statement bodies, open virtual dispatch, mutable global
state, arbitrary pointer-pointee alias graphs, volatile/atomic fields and
unmaterialized dependent bodies do not establish this proof. Pointer receivers
retain declared field type contracts but do not establish ownership of an
unqualified pointee. This does not claim general interprocedural resource
analysis or complete profile/SDK acceptance.

All 360 source/PCH commands pass across the new field suite and four adjacent
projection suites, covering C17/C++20/C++23, ARC/MRC and all four profiles. The
previous compiler misses all 13 new resource-transfer diagnostics. The complete
native isolation contract test passes, including the new awaited-executor
positive/negative cases. Fresh maintained-patch application and 16 adjacent
tooling checks pass. These checks do not rebuild or qualify an SDK archive.
