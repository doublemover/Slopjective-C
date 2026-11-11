# Immediate reference captures and reference storage

System source 2.25 admits direct, zero-argument synchronous C++ lambda literals
that return void or an owned primitive result and preserve captured lifetime
bindings. Their bodies may read borrowed references, acquire a view through a
contracted getter, consume a declared owner, use local cursors/records and update
unrelated scalar counters. Native nested CFG checking retains loan, owner-expiry
and escape obligations. The literal's completed call removes its future-reference
hazard, allowing later source-variable updates.

The proof uses the collected expression graph because semantic checking can run
before a function is attached to its translation unit. It is bounded and excludes
argument/default-argument evaluation, lazy/async results, captured pointer
rebinding, indirect outside writes, mutable output calls, reference-field/local
reference aliases, assembly, allocation/deletion, unmodeled constructors/member
calls and escaping pointer/record results. Named, open/generic and broader mutable
reference captures retain their existing checks.

Caller-supplied C++ references designate nonlocal storage. Scalar/member stores,
record assignments and representation copies cannot export a newly acquired loan
through them. Known symbolic reference identities still support owned overwrites;
borrowed payload permission is checked separately. Preserved old may facts are
not diagnosed as newly written payloads. Template-dependent assignment resolves
at instantiation, so a qualified copying sink remains valid while a pointer
output rejects.

Known local reference variables and reference members resolve to their actual
storage, including copied reference records, nested record references and array
references. A reference's lvalue binding is never inferred from the previous
pointer value in the lvalue. Unresolved reference ancestors cannot supply local
byte-range or ownership proofs, and a reference field's physical binding offset
cannot impersonate the referenced object's address. Reference bindings and
pointer-value facts remain distinct on mutation; incomplete reference joins stay
conservative.

Two new regressions execute twenty-four C++20/C++23 ARC/MRC source/PCH commands.
They cover immediate reads/cursors/counters, owner consumption/expiry, guarded
mutation and escape cases, nonlocal outputs, copied/nested/array reference
members, generic copying sinks, conditional binding representations and virtual
base identity. All twenty-seven borrowed compiler regressions and three affected
native frontend groups pass. Source-free replay publishes immediate-reference
and local reference-cell bodies and rejects caller-reference output consumers.

Extension ABI 24 and accepted SDK 2.62 are unchanged. General reference joins,
interprocedural output summaries, named/mutable/generic/lazy callable behavior and
exceptional ownership remain in the completion ledger. Matched package/runtime
matrices and SDK delivery remain in consolidation.
