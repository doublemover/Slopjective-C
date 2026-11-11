# Block lifetime pins and implicit-copy unwind

The System 2.6.0 source span admits borrowed bodies with explicit lifetime
bindings copied from ordinary Block slots. MRC and ARC `keepAlive`/`withLifetime`
bindings now have an explicit CFG operation for their potentially throwing heap
copy. The binding becomes active only on successful acquisition. A failed copy
unwinds completed earlier bindings without initializing or releasing the failed
binding. Native runtime/codegen acquisition and ordered release remain unchanged.

ARC initialization of an `ns_consumed` Block argument also copies before the
callee is entered. Its CFG edge is distinct from the call's exception edge,
including when the target is `noexcept`. This preserves earlier owner claims
when copying fails and applies annotated consumption when the call is entered.
The copy edge is present with and without explicit lifetime actions in scope.
Supported pins protect borrowed loans through MRC Block-owner consumption;
ending the last independent pin expires the loan. ARC source owners retain their
existing independent strong lifetime.

This span admits direct nonvolatile, nonweak Block slot loads. Effectful Block
initializers, weak/unowned loads, full-expression temporary rollback, arbitrary
Block local copies and broader capture/disposal recipes retain their existing
boundaries. It does not claim general exceptional CFG ownership or complete
Strict System conformance.

Explicit callable ownership checks also follow native array element types and
arbitrarily deep pointer/reference/member-pointer containers. An array-to-pointer
cast or a long pointer chain cannot hide a consumed callback contract. Ordinary
same-contract conversions remain accepted; representation/union escape analysis
remains separate work.

Maintained source/PCH witnesses cover C17/C++20/C++23 under ARC/MRC with native
exceptions, lexical and bounded pins, pin expiry, pre-entry copy failures and
post-entry foreign consumption. Native CFG unit tests inspect the failed-copy
path's exact completed binding prefix and verify the copy edge before a
nonthrowing call without pins. Source-free frontend/package validation is batched
with the preceding callable/foreign ownership spans. Regenerate dependent
objects, PCH, interfaces and packages for the compiler identity cutover.
Extension ABI 21, interface schema 2.2 and runtime layouts remain unchanged;
SDK 2.33 remains the accepted archive.

The [combined ownership receipt](../support/linux_callable_ownership_batch_progress.json)
records 390 compiler regressions, 188 native unit tests, all 33 frontend suites,
15 tooling tests and 24 source-free package configurations / 96 commands across
normal, eager-import and ASan/UBSan execution. Three platform compiler cases
remain unsupported. Broader combined runtime/sanitizer and SDK delivery remain
separate checkpoints.
