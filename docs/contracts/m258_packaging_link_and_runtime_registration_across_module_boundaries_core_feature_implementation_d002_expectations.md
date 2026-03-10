# M258 Packaging, Link, And Runtime Registration Across Module Boundaries Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1`
Issue: `#7165`

## Goal

Land real cross-module runtime packaging in lane D so a downstream module can
consume an upstream runtime-import-surface artifact, ingest the upstream peer
registration artifacts, emit an ordered cross-module link plan plus merged
linker response file, and prove deterministic multi-image runtime registration
and replay on the happy path.

## Published Surface

- Link-plan artifact:
  `module.cross-module-runtime-link-plan.json`
- Linker-response artifact:
  `module.cross-module-runtime-linker-options.rsp`
- Source contracts:
  - `objc3c-cross-module-build-runtime-orchestration/m258-d001-v1`
  - `objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1`
  - `objc3c-translation-unit-registration-manifest/m254-a002-v1`

## Semantic Models

- Authority model:
  `runtime-import-surface-plus-imported-registration-manifest-peer-artifacts-drive-cross-module-link-plan`
- Packaging model:
  `compiler-emits-cross-module-link-plan-and-merged-linker-response`
- Registration scope model:
  `registration-ordinal-sorted-link-plan-drives-multi-image-startup-registration`
- Link object order model:
  `ascending-registration-ordinal-then-translation-unit-identity-key`

## Required Rules

1. The downstream compile must emit both authoritative D002 artifacts.
2. The emitted link plan must deterministically preserve:
   - the lexicographic module-image set
   - the imported-runtime-surface input list
   - the imported/local registration manifest identities and ordinals
   - the registration-ordinal-sorted link object order
   - the merged driver linker flags
   - the authoritative runtime support library archive path
3. Imported peer artifacts must be validated fail closed before the D002 link
   plan is published:
   - registration manifests
   - discovery artifacts
   - runtime-metadata linker response files
   - translation-unit identity model/key
   - object format
   - runtime library path
4. The happy-path runtime proof must show:
   - two registered images at startup
   - provider and consumer class entries realized
   - imported protocol conformance survives module boundaries
   - imported/local dispatch remains nonzero and replay-stable
   - reset drops live registration state while replay restores both images in
     original registration order
5. D002 does not claim fully bound source method-body semantics across modules;
   it only proves the cross-module packaging, link, registration, and replay
   path is real and deterministic at the current runtime stage.

## Validation

- `python scripts/check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m258_d002_packaging_link_and_runtime_registration_across_module_boundaries_core_feature_implementation.py -q`
- `python scripts/run_m258_d002_lane_d_readiness.py`

## Evidence

- `tmp/reports/m258/M258-D002/cross_module_runtime_packaging_summary.json`
