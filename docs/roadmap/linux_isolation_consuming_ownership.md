# Consuming ownership at awaited isolation boundaries

The concurrency 2.31.0 source milestone admits ARC `ns_consumed` object/Block
parameters and `ns_consumes_self` instance receivers at supported awaited
executor and actor entries. Synchronous and native-frame targets retain the
existing Sendable and native C++ nonthrowing requirements.

The private entry ramp adopts the caller's transferred claim through its normal
ARC parameter ABI. The lazy frame owns an independent copy before suspension;
ramp cleanup balances the transferred claim even when frame allocation fails.
Cancellation before entry destroys the frame's captured ownership without
invoking the target. Rebuilding an original consumed argument removes its first
parameter-initialization retain before applying the thunk's adopting ABI, so it
does not manufacture a second transferred claim.

Objective-C message operands bind receiver first and arguments once, in source
order. Each ARC object/Block operand obtains an owning value snapshot when it is
evaluated. A later argument may clear its original strong owner without changing
the receiver or argument passed to the entry frame. Full-expression cleanup
releases these snapshots on normal completion, cancellation, allocation failure
and exceptional argument evaluation.

The runtime witnesses cover consumed objects and Blocks, consuming receivers,
combined receiver/argument consumption, void results, retained self results and
native-frame targets. Additional cases destroy the original receiver or argument
owner during later argument evaluation and throw before frame creation. Exact
object destruction counts, target-entry counts, caller restoration and single
argument evaluation are checked independently.

MRC consumed boundaries, Core Foundation consumption, init-family entry,
class/super messages, ordinary C++ task-ABI targets and automatic actor
association remain separate implementation work. This source milestone does not
change the runtime ABI or claim full actor/profile conformance. The accepted SDK
remains [2.29.0](../support/linux_source_batch_2_29_acceptance.json) until the next
combined source batch is qualified and delivered.

The [focused progress receipt](../support/linux_isolation_consuming_ownership_progress.json)
records the compiler/tool build, two native codegen/PCH regressions, the frontend
contract suite and 48 configurations / 360 commands in each normal and ASan/UBSan
source/PCH matrix. Combined SDK delivery remains deferred to the next source batch.
