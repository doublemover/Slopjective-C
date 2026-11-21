# Async property accessor composition

Source 2.123 executes async property getters and setters as individual awaited
steps inside a private native frame. Reads use the getter's logical result rather
than its physical frame/task carrier. Assignment, compound and pre/post operations
preserve their native values; a compound operation awaits the getter, computes the
update, awaits the setter and then publishes its result. Matching checked effects
propagate at each accessor. Mixed sync/async and throwing/nonthrowing pairs work.
Two conflicting error or executor contracts still reject.

The parser can type a compound getter's logical value before the complete written
await is adapted. Its unlowered protocol node cannot survive into execution:
unawaited mutations reject, including ordinary synchronous functions and getters
paired with synchronous setters. The adapted frame rebuilds actual awaited calls.
Source/PCH replay and dependent template instantiation preserve this distinction.

The composed frame also handles checked or async operations that stay on their
caller's executor. These need no executor lookup or Sendable crossing proof.
Actor, executor, class and lexical super calls keep their destination contracts;
C++ task accessors transfer through the existing task/frame bridge. Async retained
results retain their existing await ownership instead of being treated as a new
synchronous +1 result. Nontrivial async setter results are logical void, and their
receiver/construction ordering does not mistake a frame carrier for an early RHS
capture.

Controlled native tests verify exact getter/setter counts and order, logical
assignment/pre/post values, error identity, mixed accessors, actor facts before
and after suspension, same-actor/executor composition, class/super dispatch, nil,
unstarted disposal and cancellation before entry, after the getter and during the
setter's suspension. C++ frame/task accessors and owned nontrivial results verify
construction, argument destruction, nil failure without construction and disposal
of captured owners on cancellation. The phase-counter fixture now resets between
independent sequences; native development runs stop on UBSan findings.

All 47 selected compiler cases pass, including 78 new async source/PCH commands
across Core, Strict, Strict Concurrency and Strict System. All 48 AST controls,
fourteen ASan/UBSan native configurations / 49 commands, twelve new source-free
profiles, eight adjacent frontend groups and codec/isolation controls pass.
Production decoder coverage now has eight real artifact families and 160 exact
replays, including async property and untyped dependent-constructor controls,
changed-byte rejection and unsupported capabilities. All 51 open GitHub issue
bodies and update timestamps remain unchanged at this review.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are unchanged.
These are development-source receipts. Synchronous checked getters returning
nontrivial C++ records need the remaining checked-result ABI work. Raw-resource
transfer beyond explicit owners, implicit resource-union lifetimes, complete
Sendable/profile conformance, full matched analysis and one replacement SDK remain
tracked in the main roadmap.
