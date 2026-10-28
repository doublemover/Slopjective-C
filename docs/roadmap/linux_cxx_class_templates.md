# Public primary C++ class templates

The 2.47 source span publishes namespace-scope primary class template definitions
with explicit member types, type/value defaults, packs, constraints and native
consumer instantiation. Private fields, in-class constructors/destructors,
defaulted/deleted members, ordinary methods, aliases/enums and inline static data
remain part of the checked definition. A module-owned ABI tag on the primary
class preserves native specialization and member mangling.

The required `objc3.cxx-class-templates.v1` capability binds the semantic head and
canonical complete definition, together with the C++ data contract. One primary
row owns publication; underlying records and pattern members do not become
independent lookup/export entries. Head, type, initializer and body dependencies
must resolve to public declarations or members supplied by the same definition.
Native import installs record ownership before recursive member/template lookup
and compares ABI tags before merging records or class templates.

Source/reprint/relocation and changed-body checks cover canonical identity.
Source-free C++20/23 consumers instantiate new type/value/pack arguments. Package
witnesses cover shared specialization counters, private storage, copy/destruction
and class-template values crossing suspended C++ task/native-frame entries.
Imported inline variable definitions, including class counters and SDK result
operation descriptors, receive backend delivery alongside imported functions.

Forward/opaque declarations, bases, virtual members, nested/member templates,
explicit/partial specializations, deduced-return/effectful methods and remaining
dependent projections stay gated. This source span does not rebuild the accepted
SDK 2.33 archive or claim full profile/AArch64 conformance.

The [focused source receipt](../support/linux_cxx_class_templates_progress.json)
records both frontend suites, all 136 AST contract tests and eight normal
source-free package configurations / 32 commands. These use the development
facade build; optimized, broader and instrumented qualification is deferred to
the next combined implementation batch.
