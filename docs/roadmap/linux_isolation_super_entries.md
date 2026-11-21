# Super messages at awaited isolation entries

The concurrency 2.33.0 source milestone admits supported instance and class
`super` messages at awaited executor and actor entries, including category
implementations. The private function retains the source Objective-C method as
its lexical declaration context, while the first frame parameter independently
owns the original `self`. Native super dispatch uses that source class/category
lookup context and the captured receiver; it cannot fall back to dynamic
instance dispatch or reload an enclosing method's vanished stack slot.

Standard AST declaration contexts preserve lexical lookup through PCH. Ordinary
message initialization supplies the converted arguments and ownership/result
contracts before the generated message is marked as a super send. Receiver-first
snapshots, typed value storage, consumed ARC adoption, retained results and
checked errors reuse the preceding isolation implementation. Native frames and
ordinary C++ task callers retain their original continuation destination.

Actor-super entry from a nonisolated method resolves the receiver's retained
mailbox and enters it before invoking the actor member. Named class-super calls
retain the actual derived class receiver and the lexical superclass lookup.
Independent frame ownership survives cancellation and allocation failure before
target execution. Automatic object/mailbox association remains separate work.

Maintained runtime witnesses distinguish lexical superclass dispatch from a
different subclass override. They cover class and category sends, records,
retained object results, actor state, ARC consuming receivers, caller
restoration, exact source evaluation and exact object destruction. Each native
frame case checks success, pre-entry cancellation and failed entry allocation;
C++ clients also exercise an ordinary task method. Native PCH replay covers both
C17 MRC and C++23 ARC implementations.

The [2.33.0 SDK acceptance](../support/linux_source_batch_2_33_acceptance.json)
records combined validation, installed delivery and fresh native-feature checks.
MRC/CF consumption, init-family entry, unqualified dynamic Class receivers
and full profile conformance remain open. The [2.35.0 source span](linux_task_frame_interop.md)
adds ordinary C++ task-ABI isolated targets, including class/super dispatch.

The [combined source receipt](../support/linux_source_batch_2_33_progress.json)
records 375 compiler regressions, 179 native unit tests, 33 frontend suites,
287 tooling tests and all 168 configurations / 1,110 commands in each normal,
ASan/UBSan and TSan matrix. SDK/package/archive delivery is recorded separately.
