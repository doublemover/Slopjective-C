# Native conditional explicit specifiers

Source 2.197 / System 2.123 publishes written C++20/C++23 conditional `explicit`
predicates in constructors and conversion operators. Ordinary records no longer
reject these native declarations. Class and member templates, partial/full
specializations, nested classes and published local factory/closure graphs bind
the predicates through `objc3.cxx-explicit-specifiers.v1`.

Typed identity records the owning native class and function types and the full
predicate expression with its lexical template/parameter bindings. Enclosing class
and local-factory template heads bind references by position across redeclarations;
renaming their parameters does not change the semantic fingerprint. The written
predicate supplies the contract; an instantiation's cached truth does not replace
it. Declaration traversal excludes unpublished implementation bodies, while
published local declarations and initializer/closure graphs remain covered.
Predicate dependencies undergo the same export/SPI checks as other public
expressions, including references that constant evaluation could otherwise hide.
The production reader requires the new capability and rejects erased promises
or claims on invalid owner kinds.

Partial-specialization identities now normalize bound template parameter names
while hashing printed heads and definitions. Class-template contracts cut over to
`objc3.cxx-class-templates.v3`; variable-template contracts cut over to
`objc3.cxx-variable-templates.v2`. Renaming non-type parameters in primary/partial
class and variable templates preserves the complete semantic fingerprint. Native
non-type partial arguments undergo pattern-expression dependency checks instead
of the concrete-specialization admission rule.

Ordinary inline out-of-line constructors, conversions, instance/static methods
and virtual destructors now deliver their definitions after the owning class.
Their source bodies no longer disappear while identity still promises them.
`objc3.cxx-out-of-line-definitions.v1` separately binds those definitions and
requires a current reader. Existing detached template and defaulted-definition
lanes keep their own contracts. Namespace wrappers and native class/method names
remain attached to the corresponding body.

Native import already owns explicit-specifier expressions. Structural equivalence
now compares their actual expression graphs rather than treating two unresolved
predicates as equivalent. Bare explicit specifiers retain their native
comparison. AST controls distinguish dependent `E` and `!E`, retain equivalent
renamed template parameters and reject conflicting imports in both directions.
They destroy the provider AST before inspecting, printing and reparsing the
imported predicates.

The native provider/two-consumer witness passes all six pure C++ and
Objective-C++ ARC/MRC configurations in both standards. It removes its provider
before direct consumers compile; Objective-C++ modes also exercise two
source-directed consumers. It checks ordinary/template constructors and conversions, nested/partial/full class
graphs, member templates, local factories, inline closures, native by-value calls
and exact copy/destruction counts, out-of-line static factories and virtual calls
through a separately compiled provider with exact final destruction. Invalid
implicit conversions stay rejected.
A canonical, validly rehashed predicate mutation must fail against its previous
semantic identity. Hidden predicate dependencies and reader/owner forgeries have
separate negative controls.

All 112 focused native AST infrastructure controls pass, including three new
conditional-explicit ownership, equivalence and conflict checks. Sixteen tooling
checks and fresh maintained-patch replay/digest validation pass. All 17 import
groups, 18 adjacent publication groups, production codec and four standard-owned
payload configurations also pass.

This span does not claim publication of user-defined deduction-guide roots,
arbitrary dependent/unnamed graphs or complete generic/profile conformance.
Extension ABI 24, public runtime layouts and interface schema 2.2 are unchanged.
Matching SDK/archive and native-host acceptance remain pending.
