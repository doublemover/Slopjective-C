# Native C++ carrier calls and owned storage

Source 2.129 / System 2.55 implements native by-value Optional/Result arguments
and results whose payloads have qualified pure C++ lifetimes and public,
nonthrowing destruction. Linux x86-64 GNU/Itanium calls pass an initialized
caller-owned argument slot and return through fresh indirect result storage.
Register exhaustion preserves that pointer contract instead of introducing a
bytewise `byval` copy. Only the active payload is constructed or destroyed;
inactive union bytes never supply an object. Copy/move constructors retain their
native exceptions, access and deleted-operation rules. Move-only payloads work.

Direct, indirect, member/virtual and template calls share this lowering. Ordinary
returns, discarded values, conditional initialization, automatic and static
references, arrays and composed native fields establish selected-payload ownership.
Arguments remain owned across later-argument failures and the callee. A completed
return remains owned by the callee through return-time cleanup. Normal success
transfers the obligation exactly once. Both ARC and MRC expressions get a real
full-expression cleanup boundary, including calls whose result is scalar but whose
arguments own C++ carriers. Discard-only consumers instantiate and reference
contained template destructors even without a source local variable.

Checked C++ carrier results use the same no-value-on-failure rule as checked native
records. Typed failure never fabricates an empty C++ object. Caller cleanup becomes
active only after the hidden error output confirms success. `try`, successful
`tryForce` and nested `tryOptional` preserve selected arms; discarded checked values
also have success-only ownership. Return-time typed/native failures destroy a
completed result before control reaches the caller. Indirect and virtual checked
calls preserve that contract.

Explicit fresh scalar and completely initialized nonempty constant-bound array
allocations
use selected-payload initialization, partial-prefix destruction and matching
allocation cleanup. Placement construction and explicit carrier destruction preserve
the active C++ lifetime, including reuse after destruction. Borrowed direct and
aliased storage cannot grant destruction ownership. Missing initialization, implicit
array fillers, throwing-destructor payloads and volatile explicit destruction keep
precise gates. Additional computed/default/list allocation policies, mixed ARC/C++
call payloads and general runtime method/Block encodings remain separate work.

Source-free signatures and published bodies require `objc3.cxx-carrier-call.v1`.
Function, pointer/member callable, heap and explicit-destruction uses contribute
position-stable identities. Reference-only views do not invent a value-call
requirement. Readers reject missing/invented capabilities and an older catalog;
replay and source relocation preserve semantic fingerprints. Template payload
cleanup, declaration-only signatures and body-only storage operations have controls.
The production decoder adds a twelfth independent family with twenty exact paired
source/sidecar replays, including checked and nested optional results.

Initial native ASan/UBSan execution covers Some/None, both Result arms, nested and
move-only payloads, copy/move/construction faults, discarded results, all nine
argument slots, temporary/return faults, typed/native errors, heap/placement prefix
cleanup and exact program-exit destruction. Dedicated source/PCH compiler checks
cover all profiles, C++20/23 and ARC/MRC. A new AST control verifies full-expression
boundaries, and emitted-IR controls verify template destructor instantiation. Consolidation passes all 522 executable compiler cases (525 discovered, three
unsupported), including 62 dedicated commands; all 50 AST controls, twenty frontend
groups, codec/isolation, 240 exact decoder replays and sixteen ASan/UBSan native
configurations / 56 commands. ARC/MRC, source/PCH, C17/C++20/23, C++-only exceptions
and disabled elision preserve exact owner counts through program exit.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are unchanged.
These remain development-source receipts; final matched analysis and one replacement
SDK follow the remaining implementation batch.
