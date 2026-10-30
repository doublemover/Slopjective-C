# Concrete member function-template specializations on Linux

Source **2.67** publishes explicit specializations of named instance and static
member function templates owned by concrete records. Complete parents bind exact
template arguments, callable types, native symbols, inline/constexpr/deleted
contracts and published inline bodies. Specializations introduce no independent
export rows and retain their primary member template's module ABI tag.

Inline specialization bodies replay with checked public dependencies. Non-inline
specializations publish declarations and keep implementation and private helpers
in the provider. Changes to those private bodies do not alter the public
fingerprint. Consumers import the explicit specialization set before comparing
its parent, preserving lookup, primary fallback and member/function pointers.

The capability `objc3.cxx-concrete-member-function-specializations.v1` binds the
projection to concrete member-template owners. Missing or inconsistent metadata
is rejected with `O3IFC076`. Generic arguments retain their exact native keys,
including distinct positive and negative floating zero values.

The [focused source receipt](../support/linux_cxx_concrete_member_function_specializations_progress.json)
records the emitter suite, thirteen native importer tests, alias ABI code generation,
72 tooling tests and 16 source-free configurations / 96 commands passing. Source-free
package witnesses cover instance/static dispatch, specialization and primary
selection, member/function pointers and floating arguments across C++20/23,
ARC/MRC, O0/O2 and both import modes. Combined compiler/frontend/sanitizer checks
follow further implementation. [SDK 2.62](../support/linux_source_batch_2_62_acceptance.json)
remains accepted and predates this source milestone.

Dependent-parent, constructor/operator/conversion and member variable-template
specializations, effectful boundaries, full profile conformance and native Linux
AArch64 qualification remain open.

The [combined 2.63–2.73 source receipt](../support/linux_member_effects_batch_progress.json)
supersedes deferred source checks above: 411 compiler regressions, 220 native unit
tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending; SDK 2.62 stays accepted.
