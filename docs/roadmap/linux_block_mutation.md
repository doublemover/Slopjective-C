# Native Block slot mutation and exceptional ownership

The [System 2.50 span](linux_block_producer_transfers.md) extends direct producer
transfers and supersedes Block expression v2 with v3. This is the System 2.49 receipt.

System source 2.49 completes stable native strong Block slot assignment across
locals, members and indexed storage. The RHS evaluates first, its Block copy
completes before destination-address evaluation, and only a successful store
replaces the old owner. Copy failure preserves the old slot. Null stores perform
no heap copy, including explicit null casts. MRC stable slot stores keep their
native nonowning behavior.

The compiler now guards the retained RHS while evaluating the destination. If
an address/index expression throws, that guard releases the new owner before
lexical cleanup and leaves the old slot untouched. The private CFG registers the
same temporary after successful copy and transfers it after the store. This
repairs a native leak as well as the lifetime proof; no public layout or runtime
ABI changes are required.

Borrowed loans survive failed assignment and expire after replacement. Nullable
flow includes copy failure before destination-side assignments; a null store does
not invent that failure. Managed producer operands, capture-disposal regions,
weak/unsafe/atomic/volatile mutation and wider global/interprocedural precision
retain separate contracts.

Publication cuts over to `objc3.automatic-owner-unwind.v2`. Published Block
assignments and dependent assignments require it, including functions with no
handler or Block literal. Computed-value v2 and subobject v5 admission remain
current. The shared catalog, production decoder and fuzz dictionary agree.
A real `NativeBlockValues` package recipe covers nested construction, computed
local/member writes, conditional reads, nil stores and independent execution;
installed package acceptance checks its exact identities.

All 66 affected compiler cases, 45 AST controls, ten new source-free profiles,
sixteen adjacent frontend groups, codec/isolation controls and eighty exact
production decoder replays pass. The decoder controls now include real C17 and
C++23 Block-mutation artifacts alongside C-inline and incomplete-template
artifacts, with changed-byte and unsupported-capability rejection. Twelve
ASan/UBSan native configurations / 42 commands pass, plus four instrumented
package-recipe executions. Controlled RHS, copy and address faults verify the
1/12/123 operation order, old-slot preservation, exact owner destruction and
one-time deferred cleanup. The original literal stack scopes finish before
counting heap owners. Final null-store source/PCH controls also pass.

All 51 open issue bodies and discussion/update timestamps were rechecked and are
unchanged. Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are
unchanged. These are development-source receipts; one matched SDK, full analysis
and installed package qualification follow the remaining implementation sprint.
