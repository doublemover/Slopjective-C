# Native typestate and enum contracts in Linux packages

Source 2.179 / System 2.105 publishes native Clang consumable-type protocols:
default states, automatic cast/read policies, allowed call states, parameter and
return requirements, state transitions and state tests. Ordinary and template
classes retain these contracts after provider source removal. Consumer
`-Wconsumed` analysis diagnoses use after consumption, invalid arguments and
incorrect return states using the imported declarations.

Native enum contracts retain open/closed extensibility and flag-enum policy,
including nested and published-local enums. Native enum representation and
separate provider/consumer calling ABI remain intact. This adds native contracts;
it does not promote Objective-C 3.0 profile conformance or infer new runtime
ownership from Clang's typestate annotations.

Canonical printing uses one GNU spelling and a sorted, duplicate-free allowed
state set. Identity binds all states and flags independently of spelling or
source location. Published-local contracts require reader admission; unpublished
bodies do not. `objc3.native-declaration-attributes.v6` is a hard reader cutover
covering these contracts and prior native declaration contracts. Older readers,
missing requirements and fabricated namespace ownership remain rejected.

The native control checks C++20/C++23 and ordinary C++/Objective-C++ ARC/MRC,
optimized separate provider/consumer calls, two consumers, exact enum layout,
all ten attribute kinds and native state diagnostics. Mutation controls vary
each state/policy, compare equivalent spellings and state-set order, and check
published-local versus unpublished-body capability requirements. All four new
native configurations, eight adjacent diagnostic/ABI-tag configurations, 91 AST
infrastructure controls, production codec and 26 tooling checks pass (one
optional skip). Maintained patch replay and generated-authority checks pass.
These focused checks relink changed LLVM objects with retained SDK libraries. Complete
matching compiler/SDK qualification remains pending. SDK 2.62 / ABI 21 remains
the last accepted archive.
