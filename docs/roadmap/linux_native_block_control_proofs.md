# Native work-item control-flow capture proofs

Source 2.204 / System 2.130 makes bounded mutable Block-environment indexing
respect side-effect-free constant control flow. Discarded `if`/`if constexpr`
arms, zero-entry `while`/`for` loops, short-circuit operands and conditional
arms do not contribute unknown environments or escapes to an enclosing binding.
C17 `_Generic` and `__builtin_choose_expr` preserve their selected work item.
Condition and loop-initializer evaluation remain part of the proof; effectful
or unknown conditions retain every possible route.

Bindings declared inside a discarded region still receive their full semantic
capture check. Source expansion ranges conservatively retain such regions for
their own bindings, including macro-produced or unavailable ranges. This keeps
valid local work items admissible while rejecting unsafe/unknown initializers
and writes inside otherwise unexecuted source. C++ void-discarded Block lvalues
retain read roles instead of being mistaken for address escapes.

The native provider/four-consumer witness adds five control-flow shapes to the
nine completed helper/global environments. All 14 shapes run for both choices
with normal completion and pre-entry cancellation, preserving exact object,
work and defer counts. Arbitrary runtime branch conditions and mutations retain
the existing closed-write-set requirements. Runtime ABI 24, interface schema
and capability admission are unchanged.

Validation passes 216 source/PCH commands covering the new controls and the
adjacent helper/mutable-binding suites across C17/C++20/C++23, ARC/MRC and all
four profiles. The preceding compiler fails the new positive control group.
All four optimized native configurations execute 896 task scenarios across
two direct and two directed consumers. The full isolation suite, production
codec, all 122 AST controls and 16 tooling checks pass. Fresh maintained-patch
application exactly reproduces the changed LLVM sources and manifest hashes.
The consolidated isolation run also covers the preceding Block-helper span.
Matching SDK/archive and full-profile acceptance remain separate.
