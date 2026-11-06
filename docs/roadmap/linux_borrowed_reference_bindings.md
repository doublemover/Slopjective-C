# Reference bindings, current values and owner dependencies

System source 2.26 separates C++ reference bindings from the values stored in
those objects. A reference keeps its possible lvalue objects; loads, writes,
pointer mutations, byte cursors and owner acquisitions read their current facts.
Conditional references can therefore see later loans, write to proven local
alternatives and retain every possible loan after a conditional clear.

Bindings propagate through CFG joins, inherited closure state, copied reference
records, nested fields and exact representation copies. A saved reference keeps
its original binding when the containing record is replaced. Reference-containing
records expose the current loans of their referenced objects. Partial or unknown
representation writes discard completeness while preserving possible targets.
Complete local writes require at most 64 targets. Exceeding that proof bound does
not truncate the targets used for escape checks.

Pointer increments and compound offsets update each possible referenced object's
own pointer and byte-position facts before joining outcomes. Zero offsets retain
consumed-owner state. Known pointer and integer aliases, reference owner fields,
copies and explicit mutable output calls propagate owner invalidation. Derived
borrowed acquisitions retain the source loan's lifetime obligations. Nonescape
parameter forwarding retains its declared owner relationship.

Capturing a reference by value copies the referenced value and its aggregate
shape; it does not create another reference binding. Reference init-captures keep
the actual lvalue binding. Value captures of reference records retain contained
pointer/reference dependencies without treating the original aggregate's address
as a reference capture.

The maintained witnesses cover C++20/C++23, ARC/MRC, source and PCH: conditional
references, future writes, pointer and byte values, reference-containing records,
representation rebinding, consumed/replaced owners, mutable outputs, integer
aliases, derived loans and reference capture modes. Separate 64/65-target controls
exercise both the proof boundary and the last possible future-loan target.
Source-free frontend replay and consumer rejection extend the published reference
body contract. All twenty-nine borrowed compiler regressions and three affected native frontend
groups pass, including eighteen new source/PCH commands.

Extension ABI 24, physical runtime ABI 4 and accepted SDK 2.62 remain unchanged.
General interprocedural/global aliases, exceptional ownership, open/mutable/lazy
callables and complete ownership counts remain in the completion ledger. This
source milestone does not claim installed SDK qualification.
