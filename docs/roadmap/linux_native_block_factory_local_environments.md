# Local producer results and nested factory environments

Source 2.210 / System 2.136 composes closed local producer results with returned
Block values and nested literal/produced environments. A returned async Block
can capture a local ordinary callback; that callback retains its own effects
rather than inheriting the outer task's async requirement. Scalar, object,
callback and concrete template captures map through every layer to the original
actual caller values.

Construction-only validation charges the shared work budget, but rolls back
published capture/literal facts and expression memoization for an unused local.
Its captures are transported only when the returned environment uses them.
Returned local producer calls replay their complete symbolic recipe. All
symbolic bindings are established before checking actual argument transfers and
nested environments; repeated producer definitions cannot reuse another
caller's value contract. Nested environment records never replace the actual
argument checks. Unknown callbacks, unsafe objects and cleanup-managed values
retain their boundary diagnostics through every layer.

MRC outer Block copies use native nested-Block copy/dispose helpers. ARC retains
its native local/return behavior. The analysis adds no runtime copy, claim or
release. Mutable/weak/shared-byref local environments, opaque/stateful
construction arguments, unresolved dependent targets and direct/mutual factory
recursion remain unproved. Work, scope scans, depth and cycles remain bounded.
Runtime ABI 24, interface schema 2.2 and reader capabilities are unchanged.

The capture-policy audit also repairs an existing factory admission bug: a
Sendable actual argument cannot override an explicit unowned capture. Factory
recipes and ordinary Block checks share one unsafe-capture predicate. Concrete,
generic, unused explicit and nested callback capture controls reject, including
source-free imported factories. The preceding compiler admits the concrete and
generic unowned reproducer; the repaired compiler rejects both. Strong capture
controls remain admitted.

The source/PCH controls cover nested ordinary callbacks, local producer results,
produced callback wrappers, concrete dependent captures, unused unsafe producer
environments, repeated producers with different inputs and each negative caller
contract. Native helpers add nested-callback, producer-local and wrapped-local
tasks to the preceding 28 shapes, preserving provider removal, two direct/two
directed consumers, exact retirement/work/defer accounting and pre-entry
cancellation.

Nine suites pass 648 source/PCH commands across C17/C++20/C++23, ARC/MRC
and all four profiles. Four optimized native configurations pass 3,968 task
scenarios with exact retirement, work, defer and pause counts. Full isolation,
production codec, all 122 AST controls, fresh maintained patch application and
16 tooling checks pass. Matching SDK/archive and full-profile acceptance remain
separate.
