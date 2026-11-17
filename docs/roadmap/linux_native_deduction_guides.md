# Native C++ deduction-guide packages

Source 2.198 / System 2.124 gives authored namespace-scope C++ deduction guides a complete
source-free contract. Guides inherit their class template's export root and SPI
policy. Selecting the class retains its guides; an excluded class cannot acquire
an independently exported guide. Guides have no native entry point, so module
ownership binds the resulting class without fabricating a guide assembly label
or ABI tag. Explicit native symbol bindings on a guide are rejected.

`objc3.cxx-deduction-guides.v1` records the nominal template owner and bare,
conditional or implicit explicitness. Function/template identity retains native
parameter/result types, defaults and associated constraints. Conditional
predicates retain their existing lexical expression contract. Native structural
equivalence compares the guide's nominal owner and actual explicit-specifier
expression, including bare `explicit`, before reusing declarations. Same-signature guide policy conflicts are rejected
before C++ overload fallback; distinct associated constraints retain separate
candidates. Template-root SPI reads the authored declaration attributes.

Class-template identity cuts over to `objc3.cxx-class-templates.v4`. A temporary
printing view normalizes nested template parameter names as well as partial
heads, restoring every authored name before source emission. Renaming type,
non-type and pack parameters must preserve the complete semantic fingerprint. Concept and
alias-template heads use the existing structured template-parameter contract
instead of hashing parameter spellings, with hard cutovers to
`objc3.cxx-concepts.v2` and `objc3.cxx-alias-templates.v2`. A concept has no underlying
templated declaration; SPI lookup retains that node instead of dereferencing a
null template body.

The provider/two-consumer witness covers C++20/C++23, pure C++ and Objective-C++
ARC/MRC, direct and source-directed imports, class-only export selection,
conditional and bare explicit guides, constrained pointer guides, default
arguments, non-type/variadic results, real by-value provider calls and exact
object destruction. It removes provider source before consumers compile.
Parameter renaming must preserve the semantic fingerprint. Valid mutations of
explicitness, predicate, constraint and result type must change identity and
fail against old metadata. Reader erasure, owner and native-symbol forgeries
have independent controls. Native AST tests destroy the source AST before
printing/reparsing imported guides and reject conflicting import policies.

All six optimized native guide configurations, six adjacent conditional-explicit
configurations, all 116 native AST controls, 17 import groups, 18 publication
groups, production codec and 16 tooling checks pass. The C++23 ARC standard-owned
payload witness and all four class-friend configurations also pass. Maintained
LLVM patches reproduce authored files byte-for-byte and all manifest digests
match. These focused object checks do not qualify an SDK archive.

Nested/member deduction-guide roots, full generic/profile conformance,
arbitrary dependent/unnamed graphs and
matching SDK/archive acceptance remain tracked in the Linux roadmap. Runtime
layouts, extension ABI 24 and interface schema 2.2 are unchanged.
