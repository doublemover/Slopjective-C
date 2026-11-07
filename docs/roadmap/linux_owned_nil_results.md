# Nil results with construction and resource obligations

Concurrency source 2.116 implements Part 7 §7.2.4 for native frame/task messages
and implicit async entries for synchronous actor/executor methods. A nil receiver
preserves the declared result descriptor. Results that cannot represent an
ordinary zero value complete with native `INVALID_ARGUMENT`; no C++ constructor,
copy, destructor, reference binding or resource owner is fabricated.

The classifier retains typedef resource contracts and checks structural fields,
arrays and C++ bases, default member initializers and user construction. Shared
record graphs are memoized with bounded work. The shared managed-storage and
throwing-copy walkers also count qualified types and carrier context once; a
large plain graph cannot falsely consume the managed-storage proof budget. Plain scalar, object and void
results, and structural records without these obligations, keep ordinary nil
success. Non-owning pointers do not inherit their pointee's resource ownership.
Optional sends keep their existing short-circuit policy.

Explicit nil frame/task construction uses valid allocation-independent typed
failure carriers. Lazy abandonment does no logical-result work; cancellation
before entry remains cancellation. Receiver/arguments evaluate once and consumed
arguments retain nil cleanup. Implicit isolated entries check their receiver
before actor/executor entry and balance independent captured argument pins.
A free executor function's nil object argument is not treated as a nil receiver.

Sema prepares SDK descriptors for private/internal result types for both frame
and task representations. CodeGen recovers the validated native frame typedef
after PCH loading even when a task-only result has not seeded the transient
ASTContext frame cache. No guessed task layout or logical default construction
is used.

Source-free publication requires `objc3.async-entry-abi.v4` for explicit async
and actor/executor contracts, including synchronous isolated-only interfaces.
Readers advertising v3 and artifacts omitting v4 are rejected. Regenerate
interfaces, PCH, objects and packages; extension ABI 24, frame ABI 1 and service
ABI 4 layouts are unchanged. Accepted SDK 2.62 remains historical delivery.

The maintained `nonzero_nil_results` runtime fixture covers C17/C++20/C++23,
ARC/MRC and source/PCH. It checks allocation rejection, exact CF argument counts,
private/task-only results, resource fields/arrays/bases, reference and initialized
records, lazy disposal, cancellation, actor/executor nil entries and a free-call
control. All twelve selected ASan/UBSan development configurations pass 43
compile/PCH/link/execution commands. The optional `--feature nil-results` matrix
maintains the complete 24-case O0/O2 cross-product for final consolidation.

All eighteen new compiler commands pass, including source/PCH lowering and
shared structural graphs. All sixteen AST infrastructure tests pass, including
plain/shared graphs and a real managed payload after those shared fields.
Frontend controls cover six language/ownership profiles, source replay,
relocation, consumer parsing, old-reader rejection and stripped capabilities for
both explicit and implicit entries. Those six profiles, the codec and isolation
contract suites and three installed-helper import controls pass. Final matched SDK and wider feature/profile
verification follow the remaining implementation work.
