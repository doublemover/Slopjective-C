# Public unnamed template member projections on Linux

Source **2.92** implements public type graphs rooted in unnamed C++ member records
of complete named class-template owners. SDK **2.62** remains the accepted archive;
the [source receipt](../support/linux_cxx_unnamed_template_projections_progress.json)
records this completed implementation batch.

Canonical types bind ordered nominal origins: the outer published template,
closed or partial-specialization arguments, nested record path and lexical member
ordinal. Identical layouts and reversed callback/pack arguments retain distinct
identities. Native USRs use the same lexical member order, preserving module-owned
function names after moving provider sources or reparsing interfaces. Local
factory results retain their existing callable/disambiguator contracts.

Aliases, function signatures, callbacks, arrays, member pointers, shared storage,
fields/bases and closed generic arguments preserve their native C++ value types.
Nested class/alias template arguments, type/template/member-pointer defaults,
structural values and member-enum integral arguments bind their originating
records as well. Sibling enum types and equal-valued enum arguments stay distinct.
Published owners carry `objc3.cxx-unnamed-template-projections.v1`; missing
capabilities and namespace-only capability owners fail admission with `O3IFC181`.
Dependencies and export visibility follow the originating template. The compiler
retains ordinary C++ construction, copying, destruction and provider calling ABI.

Authored witnesses cover sibling records with identical layouts, distinct closed
arguments, overloads, indirect type graphs, full/partial selection, owned values,
source relocation, AST import and a second provider projecting imported types.
Dependency imports reuse only exports admitted against their exact locked
sidecar identities, including template declarations and parameter owners.
All 33 frontend suites, 237 native unit tests, 412 compiler regressions and 27
focused tooling tests pass. Three platform compiler cases remain unsupported.
Source-free packages pass all 32 configurations / 224 commands across normal,
eager, ASan/UBSan and TSan; the complete async fixture passes three smoke
configurations / 21 commands. The maintained factory batch checks development
inputs without rebuilding SDK archives for each feature.

Runtime service and GNUstep binaries are reused unchanged. The complete five-family
source/PCH runtime matrix remains recorded at source 2.91; it was not repeated
against this native indexing build. Earlier failures remain in distinct reports.

Broader unnamed contexts and dependent lookup, general standard-library
publication, remaining ownership/isolation/language/profile contracts and native
Linux AArch64 remain open. This scope does not qualify full profiles or other
standard-library implementations.
