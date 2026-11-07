# Native scalar disposal during unwind

System source 2.40 admits scalar C++ `delete` with a throwing destructor into
the borrowed lifetime and exceptional CFG model. Destructor failure still runs
the matching deallocator before full-expression and enclosing lexical cleanup.
That path preserves the original handler and owner/pin boundaries. A second
escaping exception from mandatory deallocation reaches termination.

The operand evaluates once. A private null branch skips both destruction and
deallocation when it is null, including a type whose destructor is noreturn.
Subsequent source code remains reachable along that branch, so an expired loan
cannot evade checking through a fabricated unconditional noreturn edge. Existing
CFG delete-destructor and statement nodes describe the actual destruction and
deallocation phases; no public AST layout or physical extension ABI change is
needed. Extension ABI 24 and service/frame layouts remain unchanged.

Published native delete bodies require `objc3.native-disposal.v1` and matching
canonical identity. Codec, artifact, module and import readers require exact
agreement. Source-free replay reconstructs the exceptional deallocation and null
paths from the published body.

Twelve C++20/23 ARC/MRC source/PCH compiler commands cover live and expired
views, deferred cleanup, finalizers, the null/noreturn case and the remaining
nontrivial-array boundary. A new AST control follows every null, normal and caught
exception path, requiring deallocation immediately after destruction and before
outer cleanup. All twenty AST controls pass. Native witnesses check operand
evaluation, destructor/deallocator/defer/catch order and exact allocation/free
counts. A controlled fault runs in a child with a primary exception handler and
verifies termination after deallocation if that deallocator violates its
nonthrowing contract during unwind. All twelve instrumented native scope
configurations / 42 commands pass. All 78 affected compiler cases, four source-free
profiles, five adjacent frontend groups and codec/isolation controls also pass.

Nontrivial array prefix destruction, failed-construction deallocation, wider
constructor/destructor and base/union ownership recipes remain separate work.
Complete profile conformance and matched SDK delivery remain on the roadmap.
