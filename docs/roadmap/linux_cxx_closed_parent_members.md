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
