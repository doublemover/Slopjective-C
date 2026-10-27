# Eager semantic imports and runtime headers

The concurrency 2.16.0 source milestone coordinates eager imports with the native
runtime header and source module registry. Eager consumers parse the exact pinned
runtime header before declaration merging when a verified dependency requires
its runtime capability. A separate parser marker waits for the complete header
and linkage scope; the normal header guard prevents a later SDK include from
redefining its types. Consumers do not need the provider SDK include paths.

The source registry receives the complete identities already checked by module
resolution, including invocation-supplied target, runtime, profile and compiler
fields. A subsequent source import therefore compares the same locked module,
including when the command line supplied only its name, package and fingerprint.
Direct dependency and visibility state is preserved before transfer to results.

Focused fixtures cover eager C runtime-bearing imports with and without a
consumer SDK include and with partial locks. Source-free C++ execution uses the
same runtime/async/class/inline fixture as source-directed imports, including
consumer-only definitions and consumer-first dynamic initialization. The
[2.15.0 inline-definition milestone](linux_cxx_definitions.md) supplies backend
registration and definition contracts.

The active SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json). Broad compiler,
source/sanitizer and installed/archive qualification remain deferred for the
combined source batch. This milestone does not claim wider language/profile
conformance or native Linux AArch64 qualification.
