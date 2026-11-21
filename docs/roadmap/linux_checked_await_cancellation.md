# Checked await cancellation

Source 2.243 / System 2.169 admits the public `try`, `try?` and `try!` prefix
spellings. The parser binds the operand at cast-expression precedence and uses
the existing typed propagation AST and canonical interface spellings. A C
declaration named `try` keeps ordinary lookup and parsing. Objective-C++ keeps
native try-blocks and function-try-blocks, including native rethrow.

`try? await` now handles native child cancellation and a parent cancellation
request observed at the await. The frame lowering leaves the await through its
normal cleanup edge before creating `None`. It does not attempt result copy-out
on cancellation. C++ task coroutines carry the optionalization policy in the
compiler-owned checked dependency adapter and use the distinguished cancellation
value for the failure branch. A known request skips starting another child.

Source 2.244 / System 2.170 extends the checked failure edge to throwing
awaits with the root `Error` domain, including checked joins of cancelled task
handles. The failure preserves the distinguished runtime error without copying
a result. The same route triggers `try!` failure. A narrowed error domain
continues to carry cancellation as task status, preserving its type contract.
C exports report the distinguished checked error as cancelled and pass the
original cancellation object to the callback.

`objc3_task_current_cancel_requested()` observes the active task's sticky flag.
`objc3_task_check_cancellation()` is an explicit throwing checkpoint in
`objc3.concurrency`. It throws the same `Error` object at that call site. The
query reports false outside task execution, and the checkpoint returns normally.

The 96 source/PCH commands cover C17, C23, C++20 and C++23, both ownership modes
and all four profiles. Eight source-free C client programs exercise 72 native
frame and 36 private C++ task cancellation cases. They cover optional and checked
results, narrowed domains, exact defer and child destruction counts, eight
throwing joins, 16 checkpoint invocations across both outcomes, source-free
checkpoint calls, and canonical C callback delivery after checked propagation.
Fresh patch application reproduces the compiler changes. Full profile and
matching SDK release acceptance remain open.

Source 2.276 / System 2.202 wakes suspended group iteration after explicit
cancellation if there is no ready child. The shared group leaves later child
completions available and its cleanup join still waits for quiescence. C
polling reports `CANCELLED`, unchecked native frame iteration completes as
cancelled, and the checked root `Error` path observes the distinguished
cancellation value. C++ `group.next<T>()` resumes with `task_cancelled`.

Source 2.284 / System 2.210 releases ordinary group join and iteration
registrations as soon as the waiting task is cancelled, externally completed,
or loses its last handle. Registration and cancellation coordinate through the
published suspension state, including when cancellation arrives during
registration. Shielded cleanup joins still wait for group quiescence. Six
native controls keep a member suspended and verify that each abandoned waiter
releases its registration before a later group event, then consume the member's
result and join the group. The schedule evidence gate requires those controls
in the complete sanitizer and optimization matrix.

Source 2.285 / System 2.211 extends prompt retirement to task-to-task awaits.
An interrupted parent drops its registration while the dependency remains
suspended. Completion and cancellation each claim the same published node,
and the existing completion queue order is retained. Native controls cover
cancellation, external completion, last-handle release, dependency completion
after registration, and dependencies already complete at registration. The
task-service suite passes at O0 and O2 with both ASan/UBSan and TSan, using
the pinned LLVM 23.1.5 compiler and a matching instrumented GNUstep runtime.

Source 2.288 / System 2.214 rejects a direct task await that would close a
cycle of active task dependencies. Admission publishes the edge before another
task can inspect it. Completion, cancellation and abandonment detach the edge
through the same registration ticket. A task that has already requested
cancellation follows its cancellation continuation without adding an edge.
Three native controls cover sequential and simultaneous cycle formation and the
pre-cancelled closing wait. The tasks admitted before the rejected edge retain
their normal dependency completion and executor behavior.

Source 2.292 / System 2.218 checks the same dependency graph when a task
joins a closed group. The join snapshots its nested membership before
suspending, so admission can reject a cycle through a direct wait or
another closed group. Rejection leaves the frame's status slot untouched.
Completion and cancellation retire the active edges independently of the
observer ticket. Forced scope cleanup also unlinks a previous body wait
before installing its reserved cleanup continuation.

Native controls exercise both closing-edge orders, simultaneous admission,
two groups, pre-cancellation and cancellation after registration. A forced
cleanup witness verifies that the old waiting task is reclaimed while its
external dependency is still suspended.

Source 2.296 / System 2.222 adds compiler-owned group membership to dependency
admission. A scope already owes completion to each admitted member, so adding
that member can close a cycle before finalization. Direct awaits and explicit
closed-group joins see the same obligation. Cancellation ancestry alone still
permits a child outside the scope's groups to wait for its parent.

Membership tickets keep the reserved scope finalizer allocation-free. The
native tests exercise nested group membership, shielded joins, simultaneous
admission, rejection of late membership, and an independent ancestry control.
All 96 instrumented schedule replays and 12 scenario configurations pass.
