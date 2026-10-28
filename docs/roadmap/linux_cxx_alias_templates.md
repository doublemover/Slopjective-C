# Published native C++ alias templates

The 2.44 source span publishes namespace-scope C++20/23 alias templates with
type defaults, packs, concept constraints, requires-clauses and dependent member
types. Consumers substitute new types through pointer and callback aliases and
resolve dependent members using their own instantiated types.

Alias templates remain transparent C++ type aliases and introduce no binary
symbol. Their stable lookup identity uses the qualified alias name; parameter
heads, constraints and underlying native types bind the semantic contract.
Private or excluded alias/type dependencies are rejected. The importer exposes
the alias template without leaking its underlying alias declaration into lookup.

Interfaces require `objc3.cxx-alias-templates.v1` and the C++ data contract.
Encoding and module validation reject mismatched capabilities. Source relocation,
canonical printing and source-free import preserve the declaration contract.

Member alias templates, template-template parameter heads, effectful aliases and
public class/variable templates remain separate work. This span does not claim
full profile conformance or rebuild the accepted SDK 2.33 archive.

The [source receipt](../support/linux_cxx_alias_templates_progress.json) records
all 33 frontend suites, 335 tooling tests and 24 source-free package
configurations / 96 commands across normal, eager-import and ASan/UBSan execution.
The unchanged native foundation retains the 2.43 results: 410 compiler
regressions and 196 native unit tests.
