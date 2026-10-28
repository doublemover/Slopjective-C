# Nested class-template alias templates

The 2.51 source span publishes alias templates inside public primary class
templates. Member defaults, constraints, packs, dependent member lookup,
rebinding, native arrays and callback packs retain native consumer substitution.
Aliases remain part of their enclosing class definition and ABI owner; they do
not become independent export or lookup rows.

Dependency checking follows the outer class before namespace-only template
admission, and checks each member alias head, default, constraint and underlying
type. Published definitions bind the complete member alias declaration. Native import
preserves primary-template member order after its field-first layout pass,
including aliases that precede storage fields.
`objc3.cxx-member-alias-templates.v1`, the primary class-template capability and
the alias-template capability are required together. Readers reject unknown or
inconsistent capability/identity combinations.

The same span rejects explicit member function specializations whose bodies
would otherwise be omitted from the primary class contract. Detached definitions,
member class/variable templates, concrete-class member templates, template-
template parameters and effectful aliases remain separate work.

Source/reprint/relocation, changed-underlying-type and excluded dependency checks
cover publication. Source-free C++20/23 consumers use new alias arguments,
constrained dependent members, callback packs and nontrivial rebound class values
with complete copy/destruction accounting. Focused checks are recorded when
complete; broader native/frontend/tooling, sanitizer and SDK delivery checks
remain batched. SDK 2.33 remains accepted. Full profile and native AArch64
qualification remain open.
