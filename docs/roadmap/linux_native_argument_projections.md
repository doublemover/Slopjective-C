# Native argument projections and concurrency provenance

Source 2.195 / System 2.121 preserves cleanup-managed scalar provenance through
closed native argument projections. An inline or internal free function with a
single return can forward one argument by value or reference, including nested
helper chains and concretely bound template patterns. Resource attributes on the
original storage remain relevant when the returned scalar is copied into a plain
local, captured by asynchronous work or passed to an isolated executor.

Reference projections retain the selected caller storage cell and propagate its
surrounding read, write or escape use. Unused arguments are reads rather than
escapes; a helper returning its second reference does not spuriously invalidate
the first. Ordinary integer transfers through reference/value helper chains are
accepted, while writes of resource values through projected references remain
visible to the complete-body concurrency proof. Standard library reference views
continue through the same storage and statement indexes.

The proof admits only scalar or reference results and non-owning scalar/reference
parameters. It excludes methods, variadic calls, executor/actor/async/checked-error
entries, ownership effects and resource/consumed parameters. A definition must
contain one return and no preceding runtime statements; type declarations,
static assertions and empty statements are harmless. Boolean conversions retain
their ordinary telemetry semantics. Recursive definitions, unknown references,
mutating helpers, aggregate-return conversions, dangling value-to-reference
paths and unproved result expressions cannot establish a projection. Inference
shares 256 steps, 16 nested calls and a 32-argument limit, independently of the
outer body and value-proof budgets. This is a resource-provenance proof, not a
current-actor pointer identity proof.

Focused controls exercise source and PCH publication in C17, C++20/C++23,
ARC/MRC and all four profiles. Negative cases cover direct/nested scalar copies,
Block captures, projected reference writes, mutating/opaque helpers and recursive
calls. The source-free strict native witness removes its provider before two
independent consumers, imports ordinary and template helpers, performs real
executor transfers and verifies the original Linux descriptor closes exactly
once. Imported resource copies and projected reference writes remain rejected.

All 72 source/PCH commands, four source-free native configurations, the complete
isolation suite, all 108 AST controls, all 17 import groups, 18 publication groups,
production codec and 16 tooling checks pass. The changed LLVM translation unit
compiles as C++17 with warnings treated as errors; fresh maintained patch replay
reproduces every changed production/control file and all digests match. Runtime layouts, extension ABI 24 and interface schema 2.2 are
unchanged. Conditional/multiple-return helpers, general interprocedural/global
mutation and complete profile/SDK acceptance remain separate obligations.
