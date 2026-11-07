# Nested public C++ factory types on Linux

Source **2.86** adds nested record, enum and union projections from complete
public factory results, including local member factories, wrapped result graphs,
callbacks, pointers/references and static result storage. Exact lexical tag paths distinguish same-layout nested
types under different parents. Namespace, closed generic and closure factory
origins preserve their existing argument, dependency and lifetime contracts.

`objc3.cxx-nested-factory-types.v1` binds nested projections to complete published
owners. Capability or namespace-owner mismatches fail with `O3IFC095`. A projected
tag must be reachable through the returned type graph or its checked member
factory chain; unrelated callable-local types remain
outside this contract. Aliases, fields, wrappers, storage and provider entries
retain native type and ABI identity.

Compilation, replay and source-free checks pass in normal, eager, ASan/UBSan
and TSan modes; see the [combined receipt](../support/linux_implementation_sprint_progress.json).
SDK 2.62 remains accepted. Remaining
ownership, concurrency, isolation, key-path, language/profile implementation and
native Linux AArch64 qualification remain open.
