# Native control-flow contracts in source-free packages

Source 2.182 / System 2.108 gives published native bodies an explicit control-flow
contract. Statement attributes previously fell outside the default typed
`AttributedStmt` walk. Dependency validation now traverses their argument
expressions, and body identity binds attribute kinds and complete policy values.
An assumption cannot silently reference a function excluded from the package.
Unknown statement policies require their own modeled contract before admission.

Supported native policies cover likelihood, fallthrough, required tail calls,
inlining, merge/outline control, assumptions, loop alignment, native loop hints,
OpenCL-style unroll counts and diagnostic suppression. Loop identity binds its
option, state and typed value, including dependent counts; suppression identity
binds its spelling family and diagnostic identifiers. Existing declaration
contracts retain their separate treatment.

Native AST import deep-copies assumption, alignment and loop-value expressions
into the target context; the generic attribute clone retained provider-owned
pointers for these policies. This preserves parameter and template bindings
after the provider AST is destroyed. Generic attribute clones also import their
complete common name/scope/location data, fixing lazy spelling resolution that
previously dereferenced a provider-owned scope identifier. The common-info copy
assignment adds no layout or extension-ABI fields.

Label definitions, direct/computed jumps and label addresses retain native
lexical target identity. GNU assembly binds basic/extended form, volatility,
input/output/clobber/label counts and symbolic operand roles alongside its
existing typed strings and operands. Empty extended assembly retains its first
colon, preserving the distinction from basic assembly. Printing consistently
uses `__asm__`, which reparses in strict C17 as well as C++.

Published bodies using these forms require
`objc3.native-control-flow-contracts.v1`. Unpublished implementation bodies do
not add the requirement. Codec and contract admission reject a missing
requirement, unsupported readers or namespace-forged owner claims.

The maintained native fixture removes the provider source before importing two
consumers. Its mutation controls cover loop policy/counts, assumptions,
likelihood, call-site policies, suppression identifiers, goto targets and native
assembly volatility, constraints and clobbers. C17, Objective-C ARC/MRC and
C++20/23 replay controls defend strict spelling and exact source/identity
roundtrips. LLVM AST controls destroy the provider AST before replaying the
imported statements. All four optimized native configurations, the three C17/Objective-C replay
configurations, all 19 selected publication groups, all 95 LLVM AST controls,
complete naming/SPI/codec tests and 26 tooling checks pass (one optional skip).
Maintained patch replay, feature authority and undefined-name checks pass.

A replacement current-source SDK is still required. Focused compiler-object
relinking does not qualify an archive or complete native ARM acceptance.
