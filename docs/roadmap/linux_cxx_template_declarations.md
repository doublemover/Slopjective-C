# Function template declarations and defaults

The 2.45 source span admits forward declarations and later redeclarations of
public C++20/23 function templates when the complete definition is available.
Template/type/value defaults, function argument defaults and constraints survive
canonical source and source-free import, including defaults inherited from
another declaration. Uninstantiated defaults participate in dependency checks
and semantic identity.

ABI ownership binds the native declaration USR and canonical constraint spelling.
Written defaults and parameter names do not change the owning symbol. Constraint
printing normalizes function and requirement parameter names and canonical type
and non-type parameter references while restoring the original AST names before
returning. Semantic heads independently bind merged defaults, parameter kinds,
packs, types and constraints. A changed default changes the interface contract
without changing the specialization's ABI owner.

Published calls bind complete scalar/structural template argument keys when
available. Exact floating-point bits distinguish signed zero and NaN payloads,
even when the written call names a constant instead of spelling its value.

Function templates make a hard cutover to
`objc3.cxx-function-templates.v2`; v1 is not admitted. Opaque declaration-only,
deduced-return, member, variadic and effectful template boundaries remain
separate work. This source span does not claim full profile conformance or
rebuild the accepted SDK 2.33 archive.

The [combined source receipt](../support/linux_cxx_template_contracts_progress.json)
records 410 compiler regressions, 198 native unit tests, all 33 frontend suites,
335 tooling tests and 24 source-free package configurations / 96 commands across
normal, eager-import and ASan/UBSan execution.
