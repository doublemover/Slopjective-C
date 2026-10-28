# Closed-parent member template patterns on Linux

Source **2.72** publishes explicit member class/function/variable template patterns
and class/variable partial specializations under closed template parents.
Complete owners bind exact outer arguments, template heads, constraints and full
published definitions. Native replay preserves the authored heads and imports
these patterns before comparing complete parent identities. Injected class names
remain bare inside specialized member-template definitions so replay preserves
user copy-constructor identity and lifetime side effects.

The capability `objc3.cxx-closed-parent-patterns.v1` binds the projection to complete
class-template or concrete member-template owners. Missing or inconsistent metadata
is rejected with `O3IFC081`. Source-free witnesses cover provider factories,
consumer instantiation and copies, virtual lifetimes, member-pointer dispatch,
shared storage/mutation, primary fallback and independent worker TLS.

Focused verification is recorded when implementation checks complete. Combined
compiler/frontend/sanitizer validation and SDK delivery remain batched;
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted.
Remaining effectful/generic projections, full profile conformance and native
Linux AArch64 qualification remain open.
