# Native reference-result storage

Source 2.235 / System 2.161 connects closed scalar reference-returning helpers
to the storage selected by each native call. Writes, increments and unknown
escapes through those references retain the caller's borrowed/resource sources.
Global/static/TLS cells, record fields, fixed array cells, reference parameters,
reference-local aliases, helper chains and alternative returns participate.
Receiver-independent methods, qualified/final dispatch, stateless operators and
immutable function-pointer targets retain their existing admission rules.

Storage discovery binds formal references to the current call's arguments
before helper-body indexing. Shared callees do not reuse another caller's cell
selection. Final write/escape replay still uses the complete argument graph.
Reference results with side-effecting bodies also retain the body's argument,
global and returned-value sources; ordinary writes and observations stay usable.
Unknown consumers escape the actual returned cells, including zero-argument
helpers whose global target is absent from the call's argument list.

The existing closed-definition policy, 256-node cell work budget, depth limit,
32-cell union and active function/alias cycle checks bound discovery. Unproved
reference targets retain conservative global and reference-argument treatment.
Callee-owned automatic values cannot establish surviving storage. Captured
reference-result operators, arbitrary receiver state, pointer-based parameter
retargeting, open dispatch and opaque/foreign producers remain separate proofs.
This adds no whole-program, cross-thread or temporal global precision.

Maintained source/PCH controls cover writes, chained/alternative references,
shared callees, globals/fields/arrays/static/TLS, native dispatch, resource values,
unknown escapes, recursive targets and discovery exhaustion. The source-free
publication owner covers imported reference bodies, ARC/MRC, detached captures
and awaited executor crossings. Matching SDK and native ARM acceptance remain
pending. All 72 new source/PCH commands and six expanded source-free
configurations pass. The preceding compiler misses twelve new transfer/escape
diagnostics and rejects an ordinary returned-reference use. All 360 selected adjacent source/PCH commands and 100 focused tooling checks
also pass.
