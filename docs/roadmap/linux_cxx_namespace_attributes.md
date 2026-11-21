# C++ namespace attributes and policy

The concurrency 2.20.0 source milestone publishes modeled namespace availability,
visibility, type visibility, ABI tags and SPI policy under required
`objc3.cxx-namespace-attributes.v1` semantics. Namespace contracts bind their
attribute set; scoped declarations bind the resulting native visibility.
Canonical source preserves the attributes of each declaration context.

Namespace SPI groups apply to their descendant API, including nested types and
friends. Conflicting nested SPI roots are rejected. Existing direct-import and
explicit grant checks continue to control lookup. Attributes introduced by an
extension must agree with the original namespace contract; unsupported attributes
remain an explicit boundary.

Native import preserves attributed namespace extensions as redeclarations and
imports their attributes before their children. This retains contextual
visibility and group policy. Native namespace printing also preserves modeled
attributes. Attribute and availability facts are hashed in identity so message
text cannot be mistaken for a semantic capability marker; canonical source
retains the actual native diagnostics and text.

Focused fixtures cover visibility, versioned inline namespace ABI tags,
deprecation, namespace SPI inheritance and grants, conflicting groups, semantic
identity changes and capability erasure. Source-free execution resolves hidden
and versioned namespace APIs through both import paths.

Unmodeled namespace attributes, dependent/templates, captured/local-type
expressions and broader ownership/profile contracts remain separate work. The
accepted SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad
source/sanitizer matrices and installed/archive delivery remain deferred for the
combined source batch.

Focused source evidence is recorded in the
[namespace attribute receipt](../support/linux_cxx_namespace_attributes_progress.json).
