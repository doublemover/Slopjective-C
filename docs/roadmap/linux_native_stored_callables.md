# Native callables stored in aggregates

Source 2.237 / System 2.163 resolves captured callable receivers through direct
record fields, nested aggregate fields, fixed arrays and their local copies,
reference aliases, structured bindings and closed pointers. Aggregate brace and
parenthesized initialization retain field/element selection. Explicit active
union initialization participates without providing implicit arm lifetime rules.

A pure runtime fixed-array index proves every in-bounds element of the exact
closure class. It never substitutes one sampled entry for the complete set.
Storage paths share the existing 64-node/32-depth receiver budget, 32 bindings
and 16 path components; runtime array discovery also caps the logical array at
32 elements. Exhaustion stays conservative. Inherited fields, reference
subobjects, user-provided copies/constructors, opaque factories, open pointer
sets and arbitrary container mutation remain separate proofs.

The receiver's entire containing variable joins capture ownership tracking.
Escaping or changing an enclosing aggregate therefore invalidates known captured
state, including reference captures and copies nested in other closures. Native
scalar reference/global writes, resource numbers and conservative value unions
retain their original contracts. This changes no runtime ABI or native copy,
destruction, cancellation or ownership representation.

Maintained compiler controls cover field/array/reference/resource writes,
brace/parenthesized/union storage, nested fields, copies, aliases, closed pointer
views, runtime indices and escaped containers. Source-free publication checks
instantiate exported generic storage around real native closures. Matching SDK,
full profiles and native ARM qualification remain pending. All 360 new/selected
adjacent source/PCH commands, six expanded source-free ARC/MRC configurations and
100 focused tooling checks pass. The preceding compiler misses seven new transfer
diagnostics and rejects an ordinary stored-reference callable. The preceding
Source 2.236 isolation regression remains its own receipt.
