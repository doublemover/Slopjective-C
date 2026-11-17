# Results owned by named inline lambda factories

Source 2.173 / System 2.99 binds local record/enum and nested closure results to
named inline namespace and concrete class-static lambda objects. The hard reader
cutover is `objc3.cxx-inline-closure-objects.v3`; public runtime layouts and
extension ABI 24 stay unchanged.

A selected native lambda call operator can own its body-defined result when its
closure has a published variable owner. Its result identity includes that stable
variable, the concrete callable bindings and the lexical path inside its result.
Nested callable factories retain their enclosing factory chain. Otherwise
identical lambdas remain distinct; copies of one lambda retain that lambda's
original result type. Generic specializations keep their concrete arguments.
Canonical identity does not depend on physical source coordinates.

Public aliases, overloaded parameters, nested member projections and native
owning results reuse these exact origins. Dependency traversal requires the
backing variable or class publication owner, preserving export and SPI checks.
An excluded factory cannot leak through a projected result alias. Dependent or
unproved callable origins retain their existing admission boundaries.

Native execution also exposed an AST importer overload collision: two functions
from the same source AST were structurally matched even though native Sema had
assigned distinct canonical declarations to their local-record parameter types.
Imported source identity now takes precedence over structural equivalence,
including before ordinary function type/effect comparison. Distinct overloads
retain both declarations and their original parameter types. Cross-AST lookup of
pre-existing target declarations keeps its existing structural matching rules.

Native bindings now use the same factory type origins rather than Clang's raw
USR, which can omit the owning variable for local lambda-result tags. Distinct
overloads retain distinct sidecar IDs and symbols, including structural template
arguments. Deduced factory signatures use the declared placeholder shape for
naming, preserving declaration/definition identity without recursively hashing
the factory's own local result. `auto`, pointer/reference shapes and
`decltype(auto)` stay distinct. Controls include declaration-before-definition
factories and otherwise identical function templates with different deduced
return shapes.

Controls cover local records, generic signed/unsigned results, repeated result
names, copied factory objects, nested closures, nested member records, local enums,
class-static factories and owning result copy/move/exception cleanup. Publication
checks canonical replay, relocation, changed capture values and changed projected
factory ownership, unsupported/missing reader capabilities and excluded backing
owners. Native controls remove provider source before compiling two consumers.
Matching SDK/vendor/profile/instrumentation and host qualification remain pending.

The focused checkpoint passes all 89 maintained LLVM AST infrastructure tests,
six expanded inline-closure publication configurations, six adjacent borrowed
publication profiles, four source-free native storage configurations, two
C++23 ARC/MRC owning standard-library async configurations, decoder controls and
25 related tooling checks. Sidecars retain two distinct IDs and symbols for
ordinary, structural-value and deduced-return template overloads. Native controls
verify distinct result types and dispatch, declaration/definition consistency,
exact owning-result copy/move/destruction after full-expression boundaries, and
original construction failures. Maintained patches reproduce the changed
compiler/control files exactly and all manifest digests match. This checkpoint
relinks changed LLVM objects against existing SDK libraries; a freshly sealed
current-source SDK still needs complete acceptance.
