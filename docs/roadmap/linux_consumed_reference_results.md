# Consumed read-only reference result graphs

Source 2.174 / System 2.100 extends bounded permanent-result proofs under
`objc3.borrowed-result-proofs.v12`. Extension ABI 24 and public runtime layouts
stay unchanged; older proof readers must rebuild their interfaces.

Known synchronous read-only reference producers can now forward through nested
helpers when the result is loaded immediately, decayed as a function reference,
or returned into the actual bounded result proof. Conditional results and finite
producer target sets retain every possible input and selected body. This covers
pointer, function-pointer, member-function-pointer and function-reference graphs.

A const reference result may name a bounded mutable pointer/callback slot still
alive in the active caller. The cell's lifetime is checked independently of its
loaded value: every initializer and write must prove a permanent base or exact
callable target, and the complete slot-use scan must still reject escapes and
unmodeled mutation. Mutable global slots and callee-owned expired slots do not
receive this rule. Const record receiver requirements remain unchanged.

Returning a reference alone does not classify a call as read-only. Only the
specific result's immediate value load or checked reference forwarding grants
that use. Storing the reference, taking its address, capturing it by reference,
changing its source through a cast, unknown inputs/targets and dangling helper
results retain rejection. Analysis shares the existing AST/CFG work, depth and
target bounds. Broader mutable-object state and open call effects remain separate
contracts.

Expanded source/PCH controls include nested producers, conditional references,
known producer/callback/member target sets, mutable caller slots and rejected
alias/address/capture/dangling graphs. Additional controls guard direct mutable
record fields, constexpr method reads and mutable closures against initializer
folding that could erase later writes. Import destroys the source AST before
print/reparse. Publication and native controls preserve source-free callable
behavior and exact observer counts in two consumers. Matching SDK/profile,
instrumentation and host acceptance remain pending.

The focused checkpoint passes all 18 expanded source/PCH commands, all 90
maintained LLVM AST infrastructure tests, six C17/C++20/23 ARC/MRC borrowed
publication profiles, six adjacent inline-closure publication configurations,
four source-free native storage configurations, decoder controls and 25 tooling
checks. Every native consumer observes exactly thirty nested producer/callback
entries with the expected pointer bases and selected targets. Fresh maintained
patch application reproduces the changed compiler/control files and all manifest
digests agree. These are focused changed-object checks against existing SDK
libraries; complete current-source SDK acceptance remains pending.
