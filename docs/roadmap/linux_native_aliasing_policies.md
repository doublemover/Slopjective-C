# Native aliasing type policies

Source 2.234 / System 2.160 publishes GNU `may_alias` contracts on native
typedefs and records. The policy participates in typed declaration identity;
local alias declarations in public bodies also require the native declaration
attribute contract. Canonical printing retains written and inherited policy,
including a record definition inheriting it from a forward declaration, and
normalizes duplicate idempotent flags.

This preserves the compiler's native type-based alias analysis behavior. It does
not change the service ABI, invent a new memory model or admit other unmodeled
attributes. Pointer/function ownership and lifetime rules retain their separate
contracts. Existing native attribute v6 admission, exact source replay and
matching compiler requirements continue to apply.

The preceding publisher rejects the alias type. C17/C23/C++20/C++23 controls
check source-free declarations, relocation, exact replay, local aliases and
semantic fingerprint changes when typedef or record policy is erased. The
native execution owner removes the provider source, compiles the consumer with
`-O2 -fstrict-aliasing`, links independent provider/consumer objects and verifies
both typedef and record alias writes. Separate pointer arguments and non-inlined
consumer operations prevent a known identical pointer from hiding a lost policy.
Four real generated programs must observe the written float representation.
All four publication/replay profiles and four optimized native programs pass.
Adjacent declaration-attribute and structured-binding publication groups, 100
focused tooling checks and fresh maintained-patch reconstruction also pass.
Matching SDK, native ARM and whole-profile acceptance remain pending.
