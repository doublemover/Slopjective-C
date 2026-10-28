# Template-template type parameters and nominal aliases

Source 2.56 publishes recursive template-template type parameter heads across
namespace function/class/alias/variable templates and supported class members.
Defaults, inherited defaults, nested heads, constraints and packs retain complete
contracts. Closed class, alias and compiler-builtin template arguments receive
exact keys, including nominal ownership and enclosing class arguments.

A named alias stays transparent as a type and retains its distinct identity as
a template argument. Module-owned namespace alias templates carry a preserved
`objc3.alias-owner` annotation. Native specialization mangling includes that
module/package owner, and structural equivalence refuses to merge different
owners. Member aliases inherit their record owner and exact enclosing arguments.
Aliases do not require object storage merely because their ABI owner is recorded.
Compiler and package export policies admit up to 4,096 names, retaining the
256-byte limit per name.

`objc3.cxx-template-template-parameters.v1` and, when applicable,
`objc3.cxx-nominal-alias-templates.v1` are required. Old consumers and inconsistent
owner/capability contracts reject. Heads admit type-producing template parameters;
function, variable and concept template parameters from later C++ modes remain
outside the C++20/23 contract. Recursive head depth is bounded at 64.

Maintained witnesses cover source/reprint/relocation, alpha-renamed constraints,
changed bodies, rejected dependencies, native source/PCH alias symbols, import,
classes and aliases as distinct arguments, nested and higher-order defaults,
packs, inherited defaults, compiler builtins, closed generic lambdas and member
aliases in distinct enclosing specializations. Source-free consumers check shared
specialization state and independent worker TLS. The [focused receipt](../support/linux_cxx_template_template_parameters_progress.json)
records the optimized build, four native import/equivalence tests in C++20/23,
six native source/PCH codegen commands, the emitter suite, 147 tooling tests and
all eight source-free configurations / 32 commands passing. Broader compiler/
frontend/tooling/sanitizer checks and coordinated SDK delivery remain batched. SDK 2.52 remains accepted; full profile and native
AArch64 qualification remain open.
