# Closed-parent member specializations on Linux

Source **2.71** publishes full member class, function and variable specializations
under closed template parents. Complete owners bind exact outer arguments,
authored template heads, checked definitions and published initialization/bodies.
Native replay retains every written outer/inner specialization head exactly once.
Consumers import authored members before comparing complete owner identities.

The capability `objc3.cxx-closed-parent-members.v1` binds the projection to complete
class-template or concrete member-template owners. Missing or inconsistent metadata
is rejected with `O3IFC080`. Source-free witnesses exercise specialized class
lifetimes, member-pointer dispatch, exact parent selection, primary fallback,
shared storage/mutation and independent worker TLS.

The [focused source receipt](../support/linux_cxx_closed_parent_members_progress.json)
records the emitter suite, seventeen focused native importer tests, all thirty-one
tests in the native import family, alias ABI code generation, 72 tooling tests and
16 source-free configurations / 96 commands passing. Combined
compiler/frontend/sanitizer validation and SDK delivery remain batched;
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted.
Closed-parent partial/member-template boundaries, effectful projections, full
profile conformance and native Linux AArch64 remain open.

The [combined 2.63–2.73 source receipt](../support/linux_member_effects_batch_progress.json)
supersedes deferred source checks above: 411 compiler regressions, 220 native unit
tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending; SDK 2.62 stays accepted.
