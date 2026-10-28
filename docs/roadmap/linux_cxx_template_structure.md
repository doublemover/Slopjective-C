# Class and variable template structure

The 2.48 source span admits class and variable template forward declarations and
redeclarations when a complete definition is available. Merged semantic heads
bind inherited defaults through lexical parameter positions across declarations.
ABI ownership remains independent of written parameter names and defaults.

Primary class templates additionally admit native single/multiple/virtual
inheritance, in-class virtual and pure methods, override/final contracts and
modeled using declarations. Definitions include their base types in dependency checks.
Hierarchy publication requires native RTTI; native consumers instantiate new
specializations, preserve virtual-base sharing, dynamic casts and virtual
construction/destruction.

All class and variable template rows now require
`objc3.cxx-template-structure.v1`, together with their primary template and C++
data capabilities. This is a hard cutover: regenerate interfaces and packages.
Semantic identity binds the complete definition and merged head, without
requiring the importer to preserve a particular declaration-chain shape. Native
class import may fold a forward declaration into its definition; native lookup
still exposes one canonical template. Variable chains retain their prior links.
Template declarations join their previous declarations before lookup publication.

Source/reprint/relocation and changed-body witnesses cover the contract.
Source-free consumers exercise inherited defaults and new hierarchy
specializations. Runtime package witnesses allocate a virtual diamond in the
provider, cast/delete it in the consumer and verify one shared virtual base and
exact final destruction, alongside consumer-only hierarchy instantiation.

Subsequent source spans add [deduced returns](linux_cxx_template_deduction.md) and
[member function templates](linux_cxx_member_templates.md). Explicit/partial
specializations, nested class/variable templates, effectful methods and
remaining dependent projections stay separate work. SDK 2.33 remains accepted; optimized/broader/instrumented qualification and
coordinated archive delivery follow the combined implementation batch.

The [focused source receipt](../support/linux_cxx_template_structure_progress.json)
records both frontend suites, all 137 AST contract tests and one source-free
package configuration / four commands with the development facade. Remaining
configurations and optimized/broader/instrumented checks are deferred to the
combined implementation batch.

The [combined 2.47–2.49 source receipt](../support/linux_cxx_template_generics_progress.json)
records 410 compiler regressions, 200 native unit tests, all 33 optimized frontend
suites, 335 tooling tests and 32 source-free package configurations / 128 commands
across normal, eager-import, ASan/UBSan and TSan execution. It supersedes the
focused checks and deferred combined-source checks above. SDK delivery remains
pending.
