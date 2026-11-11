# Resource-owning C++ key-path values

Source 2.126 / keypath 2.11 admits native by-value C++ resource owners through
key-path reads, writes, record traversal and nested writeback. A complete C++
record with a user-provided, nondeleted, nonthrowing destructor supplies its own
resource copy/move/destruction. Native overload resolution still checks each
required constructor and setter operation. Composed records and arrays retain
their members' individual lifetimes. Whole nontrivial C++ unions with an explicit
nonthrowing native lifetime are also admitted; their own copy/destructor pair
preserves the active scalar/resource arm.

This admission cannot waive borrowed storage. Raw resource values, raw-resource
aggregates without owning members, resource owners with throwing destructors and
resource contracts reached through arbitrary pointer types retain their gates.
Implicit/managed union ownership, traversal into a nontrivial union arm and
effectful/isolated accessors remain separate.
A raw resource field selected out of an otherwise admitted owner is still rejected.

The maintained witness checks exact open/close counts for whole values, composed
pairs, scalar fields and nested writeback. It injects each relevant copy failure
and setter failure, checks preservation of the old stored value, and verifies
that union copies retain their selected arm/value, failed union setters preserve
the old value, and that a nullable intermediate skips accessors while still evaluating and cleaning
up its independent owning argument. C++20/23, ARC/MRC and source/PCH execution
passes eight ASan/UBSan configurations / 60 commands. All 29 key-path compiler
cases pass, including 52 new commands across all four profiles, plus 48 AST controls.

Source-free signatures and published literal bodies require
`objc3.keypath-resource-values.v1`. Scalar final values also require it when their
accessor traverses a resource owner. Unpublished bodies do not require it.
Production catalog, emitter, contract validation and decoder share the capability;
older readers and missing/invented capability claims reject. Dependency traversal
also recognizes compiler-owned key-path records from type locations, preserving
logical Root/Value dependencies instead of requiring private generated record
names from another package. Four C++20/23 ARC/MRC source-free profiles pass, including signature-only,
scalar-body-only and unpublished-body controls. Nine adjacent frontend groups and
codec/isolation controls pass. Eleven real artifact families pass 220 exact
production decoder replays, including independent resource-record and nontrivial
union inputs, changed-byte rejection and unsupported-capability controls. All 51
open issue bodies/timestamps remain unchanged.

Native metadata remains descriptor ABI v2; extension ABI 24, native frame 1,
runtime services 4 and schema 2.2 are unchanged. These are development-source
receipts. Final package/profile consolidation and the replacement SDK remain open.
