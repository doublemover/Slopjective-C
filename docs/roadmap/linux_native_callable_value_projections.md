# Native callable value projections

Source 2.227 / System 2.153 preserves known borrowed and cleanup-managed
scalar results through local C++ lambda calls. Copy captures, init captures,
closure copies and reference aliases retain their original value sources;
selected generic bodies and pure mutable call operators participate. Bounded
pure graphs support local scalar aliases, conditionals and multiple returns.
Only returned sources contribute: an unused borrowed capture does not taint an
independently returned value. Boolean observations and resource integer telemetry
remain ordinary values; borrowed pointer-bit transformations retain their loan.

Parameter projections distinguish an operator call's implicit receiver from its
explicit arguments, including explicit `operator()` calls and C++23 static
operators. Static operator syntax still carries a syntactic object argument in
the AST; parameter mapping accounts for it without treating it as a formal. Identity, generic
parameters, pointer cursors and borrowed bit calculations keep the original
argument contract. Constant results and comparisons do not acquire that contract.
The same rules apply at detached captures and awaited executor crossings;
`objc_unsafe_sendable` cannot erase a known borrowed source.

Receiver bindings are indexed for writes and escapes. Unknown state changes,
reference captures and discovery beyond the precise graph bounds conservatively
retain known input/capture provenance. Pure receiver discovery is bounded at
32 captures and 32 receiver steps; body proofs use a shared 256-work budget,
depth 16 and at most 32 returns. Exhausted precise discovery does not establish
an independent result. This is local conservative analysis, not complete global,
interprocedural, lazy-async or mutable-state precision.

The previous frontend accepted the standalone borrowed init-capture return;
the maintained source/PCH regression now rejects it. Controls cover C17 and
C++20/23, ARC/MRC, all four profiles, direct source and PCH inputs. All 720 capture/adjacent source/PCH controls and 90 focused tooling checks pass.
Source-free
C++ consumers also reject captured, generic, mutable and argument-returned loans,
while admitting independent values. Exact maintained patch reconstruction and
manifest digests are checked. These focused receipts do not qualify a matching
SDK, native ARM host or full profile conformance.
