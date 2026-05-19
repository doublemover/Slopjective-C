# Objective-C 3 Metaprogramming, Property Behavior, and Interop Closure

This runbook freezes the metaprogramming/property-behavior and interop closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/metaprogramming_interop_closure/boundary_inventory.json`
- `tests/tooling/fixtures/metaprogramming_interop_closure/metaprogramming_runtime_semantic_model.json`
- `tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json`
- `tests/tooling/fixtures/metaprogramming_interop_closure/interop_runtime_ownership_abi_policy.json`
- `tests/tooling/fixtures/metaprogramming_interop_closure/lowering_runtime_artifact_contract.json`
- `tests/tooling/fixtures/metaprogramming_interop_closure/executable_proof_abi_contract.json`
- `tests/tooling/fixtures/metaprogramming_interop_closure/packaged_interop_proof_contract.json`

Replayable public workflow actions:

- `npm run objc3c -- validate-metaprogramming-conformance`
- `npm run objc3c -- validate-runnable-metaprogramming`
- `npm run objc3c -- validate-interop-conformance`
- `npm run objc3c -- validate-runnable-interop`

Helper implementations are action-registry anchors and
milestone evidence builders, not a separate public command surface.

Current closure scope:

- macro package/provenance, host-cache, and runtime-boundary execution over the live compiler/runtime path
- property-behavior lowering and runtime-observable materialization on the emitted runtime surface
- import/export, bridge packaging, runtime package loading, and mixed-image interop behavior on the packaged toolchain path
- cross-module replay and runtime-import preservation for both metaprogramming and interop artifacts

Current closure constraints:

- metaprogramming/property-behavior closure and interop closure are separate internal tracks that share one milestone and one closeout gate
- property behaviors must be treated as runtime-observable behavior rather than compile-time decoration
- ownership, error, async, and broader foreign-boundary claims must stay narrower than the evidence published today

## Metaprogramming Runtime Surface

- macro/package/provenance, property-behavior, host-cache, and runtime-boundary claims are supported only through the current runtime acceptance, runnable metaprogramming conformance, and runnable metaprogramming e2e surfaces
- public claims stay narrower than the private metaprogramming runtime ABI/cache snapshots and runtime-import artifacts
- package loading through the public runtime header remains fail-closed until deliberate ABI widening lands

## Property Behavior Runtime Materialization Surface

- property behaviors are only considered closed when emitted metadata, runtime-boundary probes, and executable lowering agree on observable behavior
- generated property behavior claims must stay grounded in the metaprogramming lowering and runtime-boundary probes, not comparison-only docs
- unsupported property behavior combinations stay fail-closed and diagnostic-backed until dedicated runtime evidence lands

## Interop Runtime Surface

- import/export, runtime package loading, bridge generation, mixed-image interop, and cross-language replay claims are supported only through the current runtime acceptance, runnable interop conformance, and runnable interop e2e surfaces
- interop claims must pass through ABI, ownership, error, and async behavior on the real packaged toolchain; comparison-only narratives do not count
- packaged cross-module interop proof is anchored to the runnable interop e2e provider and consumer fixtures, packaged probe executables, and packaged execution smoke and replay steps
- public runtime ABI widening for interop/package-loading helpers remains out of scope for this closure surface

## Lowering and Runtime Artifact Surface

- the metaprogramming/interop compile-manifest and runtime-registration surface is the shared acceptance output published by `npm run objc3c -- test-runtime-acceptance-fast`
- the metaprogramming owner surfaces are the published `runtime_metaprogramming_*` and `runtime_cross_module_metaprogramming_artifact_preservation_surface` packets
- the interop owner surfaces are the published `runtime_*interop*` and package-loader bridge surfaces
- release-scope checks must consume those emitted surfaces instead of creating parallel manifest truth

## Executable Proof and ABI Surface

- the metaprogramming/interop public command surface is `npm run objc3c -- validate-metaprogramming-conformance`, `npm run objc3c -- validate-runnable-metaprogramming`, `npm run objc3c -- validate-interop-conformance`, and `npm run objc3c -- validate-runnable-interop`
- the public workflow surface remains `validate-metaprogramming-conformance`, `validate-runnable-metaprogramming`, `validate-interop-conformance`, and `validate-runnable-interop`
- private runtime-owned helper and snapshot boundaries continue to define the live executable proof surface; the public runtime header is not widened by this milestone

Explicit non-goals:

- public runtime ABI widening for macro host execution, property-behavior runtime helpers, package loader helpers, or foreign bridge helpers
- claims that full ARC/error/async interoperability is complete beyond the currently published metaprogramming and interop runtime evidence
- release-scope scaffolding parallel to the shared runtime acceptance, conformance, and packaged e2e paths
- claims that unsupported macro execution topologies, unsupported property-behavior combinations, or unsupported foreign loader topologies are live

Follow-on tracks:

- full-envelope conformance, stability, and production claimability
- developer tooling, package ecosystem, and adoption surfaces

Authoritative live surfaces:

- runtime and pipeline:
  - `native/objc3c/src/runtime/classes/`
  - `native/objc3c/src/runtime/dispatch/`
  - `native/objc3c/src/runtime/images/`
  - `native/objc3c/src/runtime/selectors/`
  - `native/objc3c/src/runtime/state/`
  - `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
  - `native/objc3c/src/artifacts/`
- acceptance and public workflow:
  - `npm run objc3c -- test-runtime-acceptance-fast`
  - `npm run objc3c -- validate-metaprogramming-conformance`
  - `npm run objc3c -- validate-runnable-metaprogramming`
  - `npm run objc3c -- validate-interop-conformance`
  - `npm run objc3c -- validate-runnable-interop`
  - package bridge: `npm run objc3c -- <action>`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `stdlib/advanced_helper_package_surface.json`
  - `stdlib/program_surface.json`
