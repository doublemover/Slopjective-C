# M251 Frontend Handoff Normalization and Manifest Projection for Runtime Records Expectations (A003)

Contract ID: `objc3c-runtime-record-manifest-handoff/m251-a003-v1`
Status: Accepted
Scope: M251 lane-A core feature expansion for deterministic runtime-record manifest handoff.

## Objective

Make runtime metadata source records consumable as a deterministic manifest-stage
handoff without requiring later LLVM IR/object emission readiness.

## Required Invariants

1. `Objc3FrontendOptions` carries explicit emit intent for:
   - `emit_manifest`
   - `emit_ir`
   - `emit_object`
2. `BuildObjc3FrontendArtifacts(...)` records downstream post-pipeline emit
   failures without surfacing them until after manifest projection is built.
3. `pipeline/objc3_frontend_artifacts.cpp` returns a manifest-only success path
   when `emit_ir == false` and `emit_object == false`.
4. The manifest continues to project runtime metadata source records through:
   - `interfaces`
   - `implementations`
   - `protocols`
   - `categories`
   - `runtime_metadata_source_records`
5. `objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object` succeeds
   for the A002 reference fixtures, writes a manifest path, and marks the emit
   stage skipped rather than failed.
6. `objc3c-native` full compile still fails closed when later lowering/runtime
   readiness is incomplete, but writes the manifest handoff before returning a
   failing exit code.
7. `frontend_anchor.cpp` writes the manifest whenever requested and available,
   not only when the overall compile status is fully `OK`.
8. `driver/objc3_objc3_path.cpp` preserves the manifest handoff before
   returning fail-closed diagnostics for later compile stages.

## Fail-Closed Rules

- `M251-A003` does not mark runtime metadata source records lowering-ready.
- `M251-A003` does not bypass the existing later LLVM IR/object fail-closed
  checks for real emit workflows.
- `M251-A003` only widens the availability of manifest-stage handoff artifacts.
- The contract fails closed when:
  - manifest-only C API runs still report downstream emit failures,
  - full CLI runs stop writing the manifest before fail-closed exit,
  - emit stage summaries stop reflecting skipped emit behavior for manifest-only
    runs,
  - runtime record projection disappears from emitted manifests.

## Reference Fixtures

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract`.
- `package.json` includes
  `test:tooling:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract`.
- `package.json` includes `check:objc3c:m251-a003-lane-a-readiness`.

## Validation

- `python scripts/check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py`
- `python -m pytest tests/tooling/test_check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py -q`
- `npm run check:objc3c:m251-a003-lane-a-readiness`

## Evidence Path

- `tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json`
