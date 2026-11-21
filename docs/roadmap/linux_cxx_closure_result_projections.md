# Public C++ closure factory results on Linux

Source **2.84** extends public factory-origin types to returned ordinary and
generic closures. Aliases, fields, shared storage, function entries and template
type arguments retain the complete checked factory root. Closed generic factories
retain exact arguments; distinct same-layout closures retain distinct origins.
Captureless constexpr closure results can also form structural template values.

`objc3.cxx-closure-result-projections.v1` binds these projections to complete
published owners. Capability or namespace-owner mismatches fail with `O3IFC093`.
Closures created outside a qualified public factory, lambda-owned local results,
private roots and unsupported dependent contexts retain separate boundaries.

The [focused source receipt](../support/linux_cxx_closure_result_projections_progress.json)
records the emitter suite, 84 tooling tests, 16 source-free configurations /
96 commands and a full-fixture smoke case passing. Broader source
and sanitizer matrices stay batched; SDK 2.62 remains accepted. Remaining
ownership, concurrency, isolation, key-path, language/profile implementation and
native Linux AArch64 qualification remain open.
