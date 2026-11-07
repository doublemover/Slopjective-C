# Delegating construction and function-try lifetime regions

System source 2.45 extends native subobject cleanup to delegating constructors
and constructor/destructor function-try blocks. A failed delegation target cleans
its own initialized prefix. Once that target succeeds, a later delegating-body
failure destroys the complete object exactly once; successful return keeps it
alive. A private typed whole-object cleanup adds no public AST layout.

Constructor function-try handlers receive initializer/body exceptions only after
completed subobject and local cleanup. Destructor function-try handlers also
cover implicit member/base destruction after body return or fallthrough. The
remaining prefix completes before the handler; a second exception during unwind
terminates before handler entry. Handler regions carry an empty subobject prefix,
so their calls, rethrows and exits cannot destroy those parts again.

Falling off a constructor/destructor function-try handler implicitly rethrows.
A destructor handler may return to suppress its exception without repeating
implicit destruction. Noexcept boundaries apply after a function-try handler
finishes: an actual handler permits the protected cleanup and may suppress a
destructor exception, while an escaping rethrow terminates at that boundary.
Ordinary nested body handlers retain their narrower protected region.

Publication makes a hard cutover to `objc3.native-subobject-cleanup.v2`.
Constructor printing places `try` before the initializer list, then prints the
protected compound and complete handlers. Independent AST print/reparse controls
and source-free identity/replay controls cover this repaired source syntax.
Extension ABI 24, native frame/service ABIs and semantic schema 2.2 are unchanged.

Source/PCH controls cover C++20/23 ARC/MRC. Native witnesses distinguish target
failure from delegating-body failure, verify complete-object destruction counts,
constructor/destructor handler entry after exact reverse prefixes, implicit
rethrow, suppression, protected return and noexcept behavior. Child processes
check secondary whole-object/subobject exceptions and uncaught function-try
rethrows with exact termination traces. All 62 affected compiler cases, four new source-free profiles, nine adjacent
frontend groups, codec/isolation controls, 33 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.

Virtual bases, indirect anonymous-member initialization, other resource/Block
subobject recipes, managed initializer temporaries and broader ownership/profile
contracts remain in the implementation ledger. Matched SDK delivery follows
consolidated implementation and verification.
