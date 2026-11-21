# Captured native callable storage graphs

Source 2.233 / System 2.159 extends bounded helper summaries to closed C++
lambdas with scalar by-copy captures, including named/init captures, mutable
operators, closure copies and reference/address views. Captured loan/resource
values retain their sources through global stores and caller reference writes.
Capture-state assignments participate in the existing may-source union; this
retains earlier possibilities rather than establishing temporal strong updates
or per-instance field precision.

Receiver discovery admits a matching native lambda class through direct
initialization, implicit copies/moves, local aliases, standard reference views,
pure same-class alternatives and closed immutable pointer targets. All receiver
bindings are checked for writes and escapes after the completed scope index.
Unknown capture state taints reads from the capture, including caller-local
reference writes; it does not rely solely on marking a global unknown.
Captured records, native retained/NSObject payloads, by-reference captures,
opaque factories and unresolved receiver/state forms remain outside this proof.

At most thirty-two scalar captures and receiver bindings, a 64-step/depth-32
receiver search and the existing helper/index budgets are admitted. Truncation
stays conservative. Structured-binding captures use their actual binding
expressions; implicit array-copy loops preserve source values and read roles.

Publication now prints decomposition names, reference qualifiers and the
original array initializer instead of an unnamed declaration containing an
internal copy-loop/index expression. This repairs source-free array and const
reference bindings and preserves range-for declaration spelling. Binding values
now receive lexical identities, so ordinary and range-for references retain
their fingerprints across provider relocation and canonical replay. A focused
C++20/23 test also rejects reference-mode and body mutations as equivalent.
The native package owner exercises captured global/reference/resource stores, mutable
updates, closure copies, array bindings and qualifiers after provider removal.
Its six language/ownership configurations have separate CTest identities and
timeouts, so coverage matrices can distribute them and failures identify their
configuration; they retain one executable owner/build.

Controls also defend escaped receiver state, shared bindings, receiver/capture
limits and structured-binding types. All 1224 new/adjacent source/PCH commands,
six source-free configurations, the full isolation suite, seven selected
publisher groups and 100 tooling checks pass. Selected publisher groups do not
qualify the full catalogue. Broader capture/field/lifetime precision,
matching SDK, native ARM and full-profile acceptance remain pending.
