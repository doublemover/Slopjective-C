# Partial namespace class-template specializations

Source 2.60 publishes namespace partial class-template specializations under the
primary template's interface and ABI owner. Complete parameter heads, constraints,
dependent argument patterns and definitions participate in the primary contract.
Type/value parameters, packs and template-template heads retain native selection
and consumer instantiation. Incidental concrete instantiations add no contract.

The required `objc3.cxx-class-partial-specializations.v1` capability depends on
the class-specialization and primary class-template capabilities. Opaque partials,
private dependencies, unsupported heads and inconsistent metadata reject.
Declaration replay preserves namespace wrappers and source order. Native printing
uses bare constructor/destructor names inside specialization records, including
dependent partial classes. Import preserves complete checked partial definitions
before comparing the primary contract or instantiating consumer code.

Maintained witnesses cover source/reprint/relocation, constraints, changed bodies,
capability rejection, exact pattern heads, value expressions, parameter packs,
template-template deduction and consumer-created partial instantiations.
Source-free consumers exercise shared state, virtual copy/delete lifetimes and
both source-directed and eager import. Nested/member specializations, detached
bodies and effectful definitions remain separate work. Combined validation and
SDK delivery remain batched; SDK 2.52 remains the accepted archive. Full profile
and native Linux AArch64 qualification remain open. The [focused receipt](../support/linux_cxx_partial_class_specializations_progress.json)
records seven native import tests in C++20/23, the emitter suite, 19 tooling
tests and all 16 source-free configurations / 80 commands passing.
