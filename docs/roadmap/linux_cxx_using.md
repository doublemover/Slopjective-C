# C++ using declarations and directives

The concurrency 2.13.0 source milestone publishes namespace and member using
declarations, inherited constructors, overload-set introductions and namespace
using-directives and using-enum declarations under required
`objc3.cxx-using.v1` semantics.

Using declarations bind typename/access state and their exact native target set.
Target-qualified stable identities distinguish separate introductions with the
same spelling. Containing records also bind member using declarations. Directives
bind the namespace as written and its resolved target; their stable identities
include scope and nominated namespace, without source locations.

Namespace exports include their using declarations/directives normally. A global
directive can be selected explicitly as `@using:QualifiedNamespace`; it still
A global enum introduction can be selected as
`@using-enum:QualifiedEnum`. Each requires the nominated declaration to satisfy export/dependency policy. Imported
lookup follows using targets, inline namespaces and transitive directives while
preserving native overload and declaration identity. Granted visibility and SPI
filters apply to nominated declarations before lookup expansion.

Source fixtures exercise namespace aliases through using declarations, overloads,
transitive/global namespace lookup, inherited constructors and borrowed async
members, including enum introductions through a type alias. The native importer
preserves member using access, and canonical printing preserves the enum type
spelling and avoids repeating inherited default arguments. The active SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json). Broad source,
sanitizer and installed/archive qualification remain deferred to the combined
implementation batch. Dependent/template declarations, expression
contracts and broader profile/ownership work retain their explicit boundaries.
