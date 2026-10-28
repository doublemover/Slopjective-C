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
concrete-class member templates, template-template parameters and effectful
aliases remain separate work.

Source/reprint/relocation, changed-underlying-type and excluded dependency checks
cover publication. Source-free C++20/23 consumers use new alias arguments,
constrained dependent members, callback packs and nontrivial rebound class values
with complete copy/destruction accounting. The
[focused source receipt](../support/linux_cxx_member_alias_templates_progress.json)
records 138 AST contract tests, both optimized frontend suites and four
source-free package configurations / 16 commands: C++20 ARC O0 and C++23 MRC O2
through normal and eager imports. Remaining configurations and broader
native/frontend/tooling, sanitizer and SDK delivery checks remain batched. SDK 2.33 remains accepted. Full profile and native
AArch64 qualification remain open.

The [2.52 source span](linux_cxx_nested_template_types.md) additionally admits
named nested records, primary class templates and inline member variable
templates under the outermost class contract.
