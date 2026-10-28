# Concrete member variable-template specializations on Linux

Source **2.68** adds full and constrained partial member variable-template
specializations under concrete record owners. Complete parent contracts bind
exact arguments/patterns, types, storage, native ownership and consumer-visible
initializers. Specializations introduce no independent interface rows or
replacement concrete assembly labels.

Inline and partial patterns replay with checked public dependencies. Non-inline
full specializations keep storage and initialization in the provider. Their
natural C++ static-member declaration supports records without default
constructors; private initialization is excluded from the public fingerprint.
Native import preserves lexical scope, record qualifiers and template heads.
All member variable declarations/specializations retain their primary ABI tag.

The capability `objc3.cxx-concrete-member-variable-specializations.v1` binds this
projection to concrete member-template owners. Missing or inconsistent metadata
is rejected with `O3IFC077`. Namespace variable-specialization capabilities retain
their separate ownership rules.

The [focused source receipt](../support/linux_cxx_concrete_member_variable_specializations_progress.json)
records the emitter suite, fourteen native importer tests, alias ABI code generation,
72 tooling tests and 16 source-free configurations / 96 commands passing. Source-free
witnesses cover shared provider/consumer addresses, mutable full/partial storage,
independent worker TLS, constrained non-type selection, primary fallback and a
record without a default constructor initialized once across C++20/23, ARC/MRC,
O0/O2 and both import modes. Combined compiler/frontend/sanitizer checks follow
implementation. [SDK 2.62](../support/linux_source_batch_2_62_acceptance.json)
remains accepted and predates this source milestone.

Dependent-parent and constructor/operator/conversion specializations, remaining
effectful boundaries, full profile conformance and native Linux AArch64
qualification remain open.

The [combined 2.63–2.73 source receipt](../support/linux_member_effects_batch_progress.json)
supersedes deferred source checks above: 411 compiler regressions, 220 native unit
tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending; SDK 2.62 stays accepted.
