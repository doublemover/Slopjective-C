# Detached callable templates on Linux

Source **2.63** publishes authored out-of-line constructors, copy constructors,
destructors and methods of dependent primary and partial class templates, plus
member function templates of generic and concrete records. Definitions remain
under the complete class owner; no independent method rows or specialization
assembly labels are introduced.

The emitter keeps declaration and definition order, enclosing template heads,
constraints, record qualifiers, defaults and typed body dependencies. Its owner
contract binds the complete detached definition set. Consumers import those
definitions before comparing the owner identity, so source-free instantiation
receives constructor initializers, virtual lifetimes, member template bodies
and closure factories. Private unpublished helpers remain rejected.

Native declaration printing preserves bare constructor/destructor names and
suppresses inherited `explicit` on out-of-line definitions. ASTImporter retains
outer template parameter lists on ordinary dependent methods and function
templates. The semantic capability
`objc3.cxx-detached-callable-templates.v1` is required by every detached owner;
missing capability metadata is rejected with `O3IFC072`.

The [focused source receipt](../support/linux_cxx_detached_callable_templates_progress.json)
records the emitter suite, nine native importer tests, alias ABI code generation,
72 tooling tests and all 16 source-free configurations / 96 commands passing. Runtime package witnesses remove provider
sources before source-directed and eager consumer import, covering C++20/23,
ARC/MRC and O0/O2. Provider allocation and consumer copying share lifetime
counts; virtual calls, member pointers, default arguments, member templates,
closures and fresh consumer instantiations exercise imported definitions.

Detached static-data and nested-class definitions, nested/member template
specializations and remaining effectful template boundaries remain open.
Combined compiler/frontend/sanitizer validation will follow the implementation
batch. [SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains the
accepted archive and predates this source milestone. Full profile conformance
and native Linux AArch64 qualification remain open.
