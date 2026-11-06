# Native C++ standard-library customizations

Concurrency source 2.113 extends native header publication with package-authored
full and partial class template specializations. The current implementation span
covers standard-library class primaries, including hashing, tuple traits, variadic
common-type traits and numeric limits, plus full and partial `inline constexpr`
variable customizations such as range traits. The required capability is
`objc3.cxx-standard-library-specializations.v1`; header contents and configuration
remain bound by `objc3.cxx-standard-library-headers.v1`.

A customization must name a public user-owned type argument. Its export selection
follows that type's public root; all arguments, bases, members, constraints,
initializers and method bodies retain ordinary dependency and visibility checks.
Partial specializations retain their template parameters and requirements. Owner
discovery traverses type arguments and packs with depth and work budgets.
Built-in-only customizations cannot acquire a package owner through namespace
spelling. A specialization's native ABI follows its standard primary and the
module-owned argument type. It does not retag the native standard-library primary
or publish a replacement `std` namespace. Authored namespace attributes diagnose.

Identity binds the native primary, user owner, template arguments and complete
printed definition. Different specializations of the same primary retain distinct
lookup identities. Metadata requires the specialization capability, the checked
header capability and a class/variable specialization declaration kind. Canonical interfaces
include the native headers, then publish user types and their specializations in
source order. Reparse and native AST import verify those contracts again.

The maintained frontend group covers C++20/23 and plain C++, Objective-C++ MRC and
ARC, location independence, changed definitions, excluded dependencies, missing
capabilities and malformed metadata. Native provider tests also exercise an
unordered map using a published hash and equality operator, an instantiated
partial hash, tuple/common-type traits, range traits and numeric limits after
provider source removal. All six frontend profiles and both instrumented native
provider/two-consumer execution modes pass. Compiler configuration inputs and
feature-query fingerprints receive matching replay checks. Broader header-control
consolidation and the libc++ importer repair are in progress. Accepted SDK 2.62 remains unchanged; consolidated delivery follows.

The substantial header and customization groups are separate CTest entries:
`objc3_standard_library_headers` and `objc3_standard_library_specializations`.
The native executable is `objc3_standard_library_provider`. Wider library/profile
acceptance and the matched SDK remain tracked.
