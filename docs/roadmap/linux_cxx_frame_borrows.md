# C++ reference bindings and native-frame receivers

Concurrency 2.7.0 admits borrowed C++ reference parameters and
implicit receivers in explicit `__objc3_async_frame` definitions. This matches
the existing ordinary C++ task-coroutine lifetime contract. The
[acceptance receipt](../support/linux_cxx_frame_borrow_acceptance.json) binds the
source checkpoint, rebuilt compiler, source instrumentation, installed SDK and
exact-archive Debian/Ubuntu checks. The preceding
[2.6.0 dynamic-storage delivery](linux_dynamic_frame_storage.md) retains its scope. Service ABI 4, frame ABI 1,
extension ABI 21 and semantic interface schema 2.1 are unchanged.

Each reference parameter gets an independent pointer slot in the lazy frame.
The factory copies the binding before initial suspension. It does not construct,
move, retain or destroy the referred object. This includes lvalue/rvalue,
const/volatile, array, function and pointer-reference parameters. A nested native
await forwards the same referent. Unstarted disposal and pre-entry cancellation
leave the referred object untouched.

Implicit C++ `this` remains the ABI-adjusted borrowed receiver. Coroutine
splitting preserves its value across suspension, including virtual calls through
secondary-base adjustment thunks and member pointers. Const/ref qualifiers and
member operators retain their ordinary native identity. A C++23 explicit-object
parameter passed by value instead uses the existing independent value capture,
including move-only objects; a reference explicit-object parameter is borrowed.

Callers must keep borrowed referents alive until the task and frame have fully
quiesced, including registered child-group cleanup. A reference binding does not
extend a temporary's lifetime. Cancellation does not transfer ownership to the
frame. These rules do not provide actor isolation or Sendable guarantees, and
full profile conformance remains reserved.

Captured parameter debug locations follow the independent frame storage while
preserving their source argument identities. Resumed reference bindings and
owned value captures remain inspectable after the ramp has returned. A focused
IR check requires the frame-slot declaration; DWARF verification and LLDB inspect
both `this` and a reference argument after suspension and evaluate their sum.

Reference results remain rejected by the shared value/void task-result contract.
Nontrivial class semantic-package projections retain their separate bounds.
Source-free package witnesses extend the existing public aggregate/member
projection with native-frame reference parameters and const/ref-qualified
receivers; they preserve physical `c-frame-v1` and module-owned symbol identities.

Maintained witnesses cover mutation before first resume and between suspensions,
deleted-copy referents, rvalue forwarding, exact destruction counts, cancellation,
unstarted disposal, arrays, function references, pointer rebinding, overloaded
calls, secondary-base virtual dispatch, borrowed group joins and owned C++23
explicit-object receivers. Compiler fixtures cover native IR, PCH replay and
reference-result rejection.

The final C17/C++20/23 source/PCH matrix passes 24 configurations / 132 commands
in each normal, ASan/UBSan and TSan mode (72 / 396 total). C17 is included because
parameter debug rebinding also applies to C frames. The unchanged service and
sanitizer GNU libraries are reused; normal GNU comes from the accepted 2.6.0 SDK.
A cloud restart interrupted ASan after 20 completed cases; the remaining four
were resumed against the same compiler and their reports combined explicitly.
The earlier C++-only preflight scopes are not added to these final totals.

The rebuilt foundation passes 369 Clang/index/query regressions (366 ObjC3),
three unsupported cases and 166 AST/matcher/clangd contracts. All 32 facade
suites, nine GNU clients, 37 installed consumer checks, 13 integration nodes
(76 runtime checks) and 33 installed SDK gates pass. The installed feature
matrix covers 168 configurations / 966 commands. Native async packages pass
12 modes / 36 executions, including eight C++ reference/member modes and four
C-frame scope modes. Two focused compiler/PCH tests and 22 tooling checks pass;
the final 12 authority/onboarding checks overlap that tooling scope.

The exact archive passes fresh Debian 13 and Ubuntu 26.04 as ordinary users,
including relocation, source-free packages, compiler witnesses, the expanded
feature matrix and debugger clients. Both owned containers are removed. One
SDK installation and archive remain after superseded-artifact cleanup.
An earlier candidate was stopped before packaging to repair the debug location
gap; the cloud restart also interrupted the replacement before publication.
Diagnostic reports remain, and no interrupted candidate published an archive.
