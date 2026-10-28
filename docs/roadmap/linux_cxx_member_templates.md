# Class-template member function templates

The 2.50 source span publishes in-class member function templates as part of a
public primary class template. Constrained/defaulted heads, converting and
conditionally explicit constructors, conversion/call operators, static methods,
parameter packs, dependent `noexcept`, `auto`/`decltype(auto)` results and closure
factories retain native consumer instantiation. Shared specialization-local
storage uses the enclosing primary class's module-owned ABI tag.

The complete class contract binds member heads, bodies, access and declarations.
One class row owns publication; member patterns do not become separate export or
lookup entries. Signature, default, constraint, constructor initializer and body
dependencies receive the normal visibility checks. Concrete member references
validate both class and function specialization arguments. Native import keeps
requires-expression body contexts attached to their expressions, without
inventing lexical record members or changing canonical printed definitions.

`objc3.cxx-member-templates.v1` is required alongside the class-template and C++
data capabilities. Readers reject unsupported capabilities or inconsistent
markers. Source/reprint/relocation, changed-body and hidden-dependency witnesses
cover admission. Source-free C++20/23 consumers instantiate new argument types,
copy returned closures containing nontrivial class values and share member
specialization state with the provider.

Detached member definitions, explicit/partial member specializations, nested
class/variable templates, member templates in concrete classes,
template-template parameters and effectful/C-variadic members remain gated.
The [focused source receipt](../support/linux_cxx_member_templates_progress.json)
records all 138 AST contract tests, both optimized frontend suites and 16
source-free package configurations / 64 commands across normal and eager
imports. Broader native/frontend/tooling, sanitizer and SDK qualification follow
the implementation batch. SDK 2.33 remains the accepted
archive. Full profile and native AArch64 qualification remain open.

The [2.51 source span](linux_cxx_member_alias_templates.md) additionally admits
nested alias templates under the enclosing primary class contract.
