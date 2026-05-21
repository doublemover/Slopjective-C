# Objective-C 3 Macro and Metaprogramming Surface

This runbook defines the public macro/metaprogramming slice for issue #8168. It is intentionally narrower than arbitrary compile-time execution.

Authoritative checked-in contracts:

- `tests/tooling/fixtures/metaprogramming_public_surface/macro_metaprogramming_public_surface_contract.json`
- `schemas/objc3c-macro-metaprogramming-public-surface-v1.schema.json`
- `tests/tooling/fixtures/metaprogramming_public_surface/macro_expansion_artifact_ownership_contract.json`
- `schemas/objc3c-macro-expansion-artifact-ownership-v1.schema.json`
- `tests/tooling/fixtures/security_hardening/macro_supply_chain_trust_registry.json`
- `tests/tooling/fixtures/security_hardening/macro_package_provenance_trust_policy.json`

Public validation entrypoint:

- `npm run objc3c -- validate-metaprogramming-conformance`

Targeted implementation validator:

- covered by `npm run objc3c -- validate-metaprogramming-conformance`

## Supported Surface

Supported macro declaration model:

- macro name: `Trace`
- required attributes: `objc_macro`, `objc_macro_package`, `objc_macro_provenance`, `objc_macro_cache_key`, and `objc_macro_sandbox`
- admitted topology: pure, body-backed free functions
- supported sandbox policy: `deterministic`
- trusted packages: `std.metaprogramming` and `std.metaprogramming.trace`

Supported derive inventory:

- `Equality`
- `Equatable` as an alias for `Equality`
- `Hash`
- `DebugDescription`

Supported property behaviors:

- `Observed`
- `Projected`

The public claims for those forms are the narrow `objc3c.behavior.language.metaprogramming.derive-expansion-inventory` and `objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism` rows. Runtime host-cache boundary evidence remains under `objc3c.behavior.runtime.metaprogramming.host-cache-boundary`.

## Security Policy

Macro expansion is fail closed before it can publish expansion authority. A candidate must satisfy all of these checks:

- package is known and trusted by `macro_supply_chain_trust_registry.json`
- provenance is signed by a trusted signing key
- provenance is not revoked
- cache key is stable and bound to replay metadata
- replay metadata is present and deterministic
- sandbox policy is deny-by-default and exactly `deterministic`
- network access and filesystem writes are denied

The checked denial cases are unknown package, revoked provenance, unsigned provenance, denied network sandbox, untrusted signer, and missing replay metadata.

## Reserved Surface

The following are not public support claims:

- arbitrary compile-time execution
- unsafe macro plugins
- network access during macro expansion
- filesystem writes during macro expansion
- source compatibility with third-party macro ecosystems
- runtime expanded body materialization beyond the checked host-cache provenance boundary

Those paths must remain `reserved`, `rejected`, `deferred`, or `internal` in the contract fixture. Generated files under `tmp/` are replay outputs only and cannot become claim inputs.

## Deterministic Fixtures

Positive fixtures:

- `tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3`
- `tests/tooling/fixtures/native/macro_package_provenance_positive.objc3`
- `tests/tooling/fixtures/native/macro_host_process_provider.objc3`
- `tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3`
- `tests/tooling/fixtures/native/property_behavior_legality_positive.objc3`

Negative macro-safety fixtures cover missing metadata, orphan metadata, invalid package, invalid provenance, non-pure callable, missing cache key, missing sandbox policy, and method topology. The validator requires diagnostic codes `O3S320`, `O3S321`, `O3S322`, `O3S323`, `O3S324`, `O3S325`, `O3S331`, and `O3S332`.
