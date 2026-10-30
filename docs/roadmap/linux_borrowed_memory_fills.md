# Borrowed provenance through memory-fill builtins

System 2.13.0 recognizes native `memset`, `memset_inline`, checked `memset` and
`bzero` builtins as nonescaping write-only destination operations. A complete
constant-sized overwrite of proven direct local storage discards its previous
borrowed values, callable effects and local address targets. Whole records and
arrays, nested fields, selected constant elements and returned `memset`
destination aliases preserve their storage identities.

Only exact storage prefixes justify strong overwrites. Clearing one element
cannot erase a wildcard array summary describing untouched siblings. Partial,
zero-byte, unknown-sized and uncertain indirect writes retain earlier facts.
Writing an expired borrowed region still diagnoses; overwriting an ordinary
local slot containing an expired pointer does not read that old payload.
Overwriting an owner binding expires loans tied to that binding. Fill-byte and
size operands retain their usual escape checks, and fills never invoke callbacks
stored in the destination.

The scope is compiler-known memory fills, not arbitrary user callees, byte-range
alias reconstruction, union active-member tracking or general heap mutation.
Native witnesses cover C17/C++20/C++23, ARC/MRC and PCH, complete and partial
writes, nested fields, exact and unknown arrays, checked/bzero forms, returned
aliases, owner expiry and callback non-invocation. Frontend cases add canonical
source-free inline publication of a complete overwrite. Broader validation is
batched with the isolation operand changes; SDK 2.33 remains accepted.

Public inline definitions may reference reserved compiler-provided builtin
functions without listing them as package exports. Native builtin identity and
the pinned compiler supply that dependency, including implicit C linkage
contexts. Ordinary libc declarations, unknown lookalike names and user-provided
definitions retain the existing export/dependency checks. Object-emitting import
witnesses cover both admitted and rejected dependencies; source-free packages
execute complete fills and counted record-captured consumption.

The [combined operand/fill receipt](../support/linux_operand_fill_ownership_progress.json)
records completed native/frontend/tooling checks, all three complete runtime
matrices and focused source-free package execution. Installed SDK delivery is
a separate checkpoint.
