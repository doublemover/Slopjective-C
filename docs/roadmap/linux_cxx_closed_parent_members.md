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

Focused verification is recorded when implementation checks complete. Combined
compiler/frontend/sanitizer validation and SDK delivery remain batched;
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted.
Closed-parent partial/member-template boundaries, effectful projections, full
profile conformance and native Linux AArch64 remain open.
