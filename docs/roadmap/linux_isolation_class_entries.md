# Named class messages at awaited executor entries

The concurrency 2.32.0 source milestone extends qualified executor entry to
Objective-C messages with a named class receiver. The private frame captures
only the written arguments; its body dispatches to the original named class
on the declared main/global/named executor. Inherited class methods preserve
the written receiver class rather than substituting the method's declaring
class. Class methods on actors retain class-method semantics without inventing
an instance mailbox.

Arguments retain receiver-free, exactly-once semantic bindings before lazy frame
creation. Synchronous and native-frame methods use the existing typed result,
checked-error and cancellation paths. ARC consumed object/Block parameters use
the adopting parameter ABI from the preceding ownership milestone. Retained
results preserve native return ownership under ARC and MRC. [Super messages](linux_isolation_super_entries.md) are added by the subsequent
2.33.0 source milestone. Init-family entry, unqualified dynamic Class receivers and ordinary C++
task-ABI targets remain separate work.

Maintained runtime cases cover scalar, record, void, retained object and
native-frame results; main/global/named destinations; inherited dispatch;
consumed object/Block arguments; caller restoration; and cancellation before
target entry. Source-free frontend consumers reconstruct class-method record
and consumed ownership signatures before forming their own entry thunks.

The [focused receipt](../support/linux_isolation_class_entries_progress.json)
records the compiler/tool build, frontend contract suite and two native codegen/PCH
regressions pass. Focused runtime validation passes C17 ARC/O0/source and C++23
MRC/O2/PCH configurations, 15 commands total. The
[2.33.0 SDK acceptance](../support/linux_source_batch_2_33_acceptance.json) records
combined validation and delivery. Full actor/profile conformance remains open.
