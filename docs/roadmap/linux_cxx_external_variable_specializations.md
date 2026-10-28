# Explicit variable-specialization storage

Source 2.62 publishes non-inline explicit namespace variable-template
specializations with provider-owned initialization and storage. Scalar, record
and thread-local definitions travel as declarations when their initializers are
private implementation. Replay therefore requires no default constructor and
cannot run another initializer in a consumer. Inline zero-initialized
specializations retain shared native storage.

Generated replay uses Clang's existing declaration-only specialization extension.
Its diagnostic suppression is scoped to each generated declaration. Source
storage properties are restored after printing, and ordinary source diagnostics
retain their own settings. Constant initializers that belong to the published
contract continue to travel with their definitions.

The required `objc3.cxx-variable-specialization-storage.v1` capability depends on
the explicit variable-specialization capability. Exact arguments, independent
owned symbols, type/TLS properties and delivery mode remain checked. Private
provider initializers do not enter public fingerprints; changing their bodies
requires a new provider object without changing the declaration contract.

Maintained witnesses cover source/reprint/relocation, capability rejection,
records without default constructors, private initializers, exactly one provider
initialization, shared record/scalar values, independent worker TLS and inline
zero storage. Nested/member specializations, detached definitions and effectful
forms remain separate work. Combined validation and SDK delivery remain batched;
SDK 2.52 remains accepted. Full profile and native Linux AArch64 qualification
remain open. The [focused receipt](../support/linux_cxx_external_variable_specializations_progress.json)
records the emitter suite, 19 tooling tests and all 16 source-free configurations
/ 96 commands passing through source-directed and eager imports.

The [combined 2.53–2.62 source receipt](../support/linux_template_publication_batch_progress.json)
supersedes the deferred source checks above: 411 compiler regressions, 208 native
unit tests, 33 optimized frontend suites and the maintained 335 tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending.
