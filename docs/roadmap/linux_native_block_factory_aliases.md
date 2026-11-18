# Native work-item factory aliases and concrete templates

Source 2.206 / System 2.132 extends closed Block factory recipes with immutable
local scalar, Objective-C pointer and Block aliases. Each alias retains the
complete set of caller arguments that can supply its value. Conditional aliases
union both value arms; condition-only arguments remain reads. Repeated aliases
of one actual argument are checked once without losing their separate captured
bindings. Mutable locals, computed aliases and ownership-changing conversions
remain unproved.

Fixed-arity concrete free function templates and static class-template factories
can use a definition pattern before Clang instantiates its body. Actual parameter
types must satisfy the existing scalar/by-value ownership gate. Pattern types
never substitute for the actual caller's transfer contract. References, parameter
packs, consumed/weak/unsafe/volatile/atomic storage and record copies remain
outside this recipe.

Clang defers dependent Block captures, so an empty pattern capture list cannot
prove an empty environment. Bounded evaluated-reference discovery recovers
references to enclosing parameters and immutable locals; unevaluated operands
do not create captures. Unknown dependent selection, nested deferred closures and
shared `this` remain unproved. Actual Block-valued inputs recursively check their
real environments. Both ordinary and deferred MRC captures require the reserved
runtime Block-copy contract; the analysis synthesizes no ownership operations.

Recipe work is bounded at 1,024 steps independently of the 4,096-node scope scan,
depth and cycle limits. Oversized pure recipes reject; a smaller counterpart is
admitted. This accommodates explicit MRC copy wrappers and deferred capture
analysis without removing termination bounds. Runtime ABI 24, interface schema
2.2 and reader capabilities remain unchanged.

The maintained native package witness covers 23 work-item shapes through two
direct and two source-directed consumers, with provider sources removed. Added
shapes exercise object/scalar aliases, free/static templates and callback aliases
with exact object retirement, work/defer counts and pre-entry cancellation.
Resource-erasing aliases and unknown callback environments must reject.

Focused verification and matching SDK delivery are tracked separately. Full
profile and replacement-archive acceptance are not implied by this source span.
