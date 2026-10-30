# C++ immediate and constexpr defaulted members

The concurrency 2.19.0 source milestone publishes consteval defaulted special
members and immediate comparisons under required
`objc3.cxx-immediate-defaulting.v1` semantics. In-class and out-of-line forms
preserve native immediate invocation and constant-evaluation rules. Containing
records independently bind their immediate member set.

Out-of-line defaulted members also bind their constexpr state explicitly.
Canonical printing retains an explicit constexpr qualifier on their definitions;
implicitly derived in-class constexpr state remains derived from native member
contracts. This distinguishes constexpr and consteval callability during import,
including when their ordinary calling signatures coincide.

Source fixtures cover immediate construction and equality, out-of-line immediate
and constexpr construction, inline constant objects and consumer constant
expressions. Changing consteval to constexpr changes interface identity, and
capability erasure is rejected. Consumers use the imported definitions after the
provider source has been removed.

Dependent/templates, captured/local-type declarations, concrete non-defaulted
friends and broader ownership/profile contracts remain separate work. The active
SDK remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad
source/sanitizer matrices and installed/archive delivery remain deferred for the
combined source batch.

Focused source evidence is recorded in the
[immediate defaulting receipt](../support/linux_cxx_immediate_defaulting_progress.json).
