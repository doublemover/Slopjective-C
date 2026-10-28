# Concrete constructor/operator/conversion specializations on Linux

Source **2.69** extends concrete member function-template publication to explicit
constructor, operator and conversion specializations. Complete parents bind
exact arguments, callable kinds, explicitness, native symbols and published
constructor initializers/bodies. The primary function-template ABI owner remains
unchanged and no independent specialization rows are introduced.

Native replay keeps template argument lists on operator function names and omits
them from constructor/conversion names, where C++ requires deduced syntax.
Consumers import these definitions before parent comparison and retain overload
selection, explicit conversions, primary fallback and member-pointer dispatch.
Static-member references retain one canonical identity when native replay adds
record qualification; typed traversal still binds the exact owner and arguments.

The capability `objc3.cxx-concrete-special-member-specializations.v1` requires the
concrete callable-specialization contract. Missing or inconsistent metadata is
rejected with `O3IFC078`. Named/static specializations retain the same base
contract; effectful and dependent-parent boundaries remain separate work.

The [focused source receipt](../support/linux_cxx_concrete_special_member_specializations_progress.json)
records the emitter suite, fifteen native importer tests, alias ABI code generation,
72 tooling tests and 16 source-free configurations / 96 commands passing. Source-free
witnesses cover provider construction, specialized constructor initialization,
call/assignment operators, explicit conversions, primary fallback, member pointers
and virtual lifetime counts across C++20/23, ARC/MRC, O0/O2 and both import modes.
Combined compiler/frontend/sanitizer checks follow further implementation.
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted
and predates this source milestone.

Dependent-parent specializations, remaining generic/effectful projections, full
profile conformance and native Linux AArch64 qualification remain open.
