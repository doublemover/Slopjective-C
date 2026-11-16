# Bounded local retargeting in permanent-result proofs

Source 2.165 / System 2.91 extends permanent borrowed-result proofs to ordinary
local and by-value pointer/callback slots with known assignments. Each slot binds
its initializer/caller argument and every assigned value in the complete bounded
body. Every alternative must prove a permanent base or known callback target;
this conservative union does not assume a particular assignment reaches a return.
Slot lookups retain unknown initial values even if a later assignment overwrites
them; an explicit assignment expression binds its result directly to its RHS.

Direct assignments, conditional/switch retargeting, callback-producing helpers,
deferred writes and object-pointer base-preserving arithmetic retain their native
semantics. Prefix/postfix and compound pointer updates preserve the same lifetime
base. Function-pointer arithmetic cannot establish a known callable target.
The active callable uses its checker-owned body before Sema attaches that body
to the declaration, covering functions, Objective-C methods and Blocks.
Address/reference escapes, reference captures, volatile/global/TLS mutation,
`__block` and implicit resource storage remain conservative. Weak/alias callback
globals and externally replaceable C globals cannot attest an initializer target.

All initial/assigned values share the existing 4096-unit AST/CFG budget, depth 32
and 64-target limit. Exact physical ABI, replaceable-function gates and complete
normal returns remain mandatory. No global mutation summary or arbitrary
interprocedural alias effect is inferred. `objc3.borrowed-result-proofs.v7` is a
hard reader cutover; physical ABI and runtime layouts are unchanged.

Eighteen compiler/PCH commands cover C17/C++20/23 ARC/MRC, including known and
unknown retargeting, callbacks/factories, pointer updates, deferred writes,
escaped slots, reference results, active function/method/Block bodies and
replaceable weak globals. All 81 AST tests pass. Six borrowed and four adjacent storage publication profiles
and four source-free native configurations exercise the new value sets. Complete rebuilt
compiler/instrumentation/SDK acceptance remains separate.
