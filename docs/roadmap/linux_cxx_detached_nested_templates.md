# Detached nested templates on Linux

Source **2.65** publishes authored out-of-line definitions of named nested records
and nested primary class templates under generic and concrete class owners.
Constrained partial parents and deeper dependent nesting retain the same complete
owner contract. Nested definitions introduce no independent export rows.

Native declaration printing and import preserve outer template heads, record
qualifiers and exact forward/definition redeclaration chains, including deeper
dependent nesting. The emitter keeps authored declaration/definition order and binds the
complete nested definition set into the parent identity. Consumers import nested
definitions before comparing their parent contract, then instantiate the same
native record layouts, constructors, methods and virtual lifetimes.

The capability `objc3.cxx-detached-nested-templates.v1` is required for every
parent publishing a detached nested definition. Capability discovery follows
forward declarations into complete bodies, including TLS and recursive
template-template heads. Missing or inconsistent
capability metadata is rejected with `O3IFC074`. Definitions require module-owned
source and public dependencies; unpublished helper references remain rejected.

The [focused source receipt](../support/linux_cxx_detached_nested_templates_progress.json)
records the emitter suite, eleven native importer tests, alias ABI code generation,
72 tooling tests and 16 source-free configurations / 96 commands passing. Source-free
package witnesses cover provider allocation, consumer copying, shared lifetime
counts, virtual calls, member templates, concrete and partial parents, deeper
nesting and new consumer instantiations across C++20/23, ARC/MRC, O0/O2 and both
import modes. Combined compiler/frontend/sanitizer validation follows further
implementation. [SDK 2.62](../support/linux_source_batch_2_62_acceptance.json)
remains accepted and predates this source milestone.

Nested/member template specializations, remaining effectful boundaries, generic
argument/closure projections, full profile conformance and native Linux AArch64
qualification remain open.
