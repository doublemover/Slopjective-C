# Native member deduction-guide packages

Source 2.200 / System 2.126 extends authored deduction-guide publication to
nested class templates and dependent enclosing records. A guide retains a
separate declaration identity even when its enclosing class prints the source.
Export/SPI ownership still follows that class; no standalone runtime entry or
extra source declaration is fabricated.

`objc3.cxx-deduction-guides.v2` replaces v1. It binds guide access, nominal
template ownership, native signatures, explicitness and associated requirements.
Identity normalizes both guide-local and enclosing template parameter heads,
including enclosing packs, while preserving authored source spelling. Nested
guide predicates also retain their lexical binding when summarized by an
owning record. The compiler declaration stream includes semantic guide names.

Ordinary and template record admission now validates member guides using the
same signature/attribute rules as namespace guides. Native structural
equivalence compares complete records' authored guide sets, including their
template heads and explicit policies. Importing a complete class cannot silently
reuse a conflicting guide definition merely because the class layout matches.

The provider/two-consumer witness adds ordinary nested classes, dependent
families and enclosing parameter packs. It checks parameter renaming, exact
source replay, explicit-conversion rejection and source-free direct and
source-directed consumers. AST controls destroy the provider before printing
and reparsing imported member guides, and reject whole-record policy conflicts
in both import directions.

This span retains the wider dependent/unnamed graph, vendor/profile and matching
SDK/archive acceptance work in the Linux roadmap. Runtime layouts, extension
ABI 24 and interface schema 2.2 are unchanged.

Focused validation covers six C++20/23 pure-C++/MRC/ARC optimized provider and
two-consumer configurations, all 118 native AST controls, 17 import groups,
18 publication groups, production codec and 16 tooling checks. The guide
witness includes validly rehashed nested result and enclosing-policy mutations.
Fresh maintained-patch application reproduces the authored LLVM sources and
all manifest digests match. These receipts do not qualify an SDK archive.
