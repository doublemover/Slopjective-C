# Borrowed provenance through aliased memory operations

System source 2.21 extends builtin fills and representation copies to finite
local aliases, conditional source/destination sets, nested fields and arrays.
Exact single-target copies with matching types and complete object sizes preserve
field, callable, pointer-target and owner-identity facts. Exact complete fills
clear only the selected storage. Zero-length operations preserve existing facts;
self `memmove` preserves its source snapshot.

Partial, unknown-sized and open-target writes invalidate pointer-target proofs
and retain possible loans and callable effects. Multiple-target copies weakly
update every possible destination with every possible source. An incomplete
local target set cannot replace the pointer value carried by its other
alternatives. Copies with possible nonlocal destinations reject loan-bearing
representations. Ordinary character copying from a borrowed region still
produces owned destination bytes.

Returned destination pointers expose the updated storage. Recovering a matching
typed view preserves its local identity, without treating user byte casts as
object-scaled pointer arithmetic. Exposing an array element's address includes
reachable siblings; reading that element's stored value remains precise.
Overwriting a required owner binding expires existing loans, and exact owner
copies carry the replacement identity. Builtin memcpy, memmove, inline/checked
copies, memset, inline/checked fills and bzero share these rules.

The new native regression executes eighteen C17/C++20/C++23 ARC/MRC source/PCH
commands, covering complete/partial/uncertain aliases, nested and conditional
copies, returned destinations, array siblings, copied pointer aliases, owner
expiry and callable effects without accidental invocation. All twenty-one
borrowed compiler regressions pass with the rebuilt development compiler.
Frontend source-free replay, relocation, changed-body identity and valid/escaping
consumer witnesses are authored and pass syntax checks; their execution remains
part of consolidation. SDK 2.62 is unchanged. General byte-offset reconstruction,
union active-member precision and interprocedural mutation remain separate work.
