# M258 Cross-Module Build And Runtime Orchestration Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-cross-module-build-runtime-orchestration/m258-d001-v1`
Issue: `#7164`

## Goal

Freeze the truthful cross-module build/runtime orchestration boundary above the
transitive serialized import payload and the emitted local runtime registration
manifest before lane-D lands real packaging and aggregated runtime-registration
execution.

## Published Surface

- Semantic surface:
  `frontend.pipeline.semantic_surface.objc_cross_module_build_runtime_orchestration_contract`
- Source contracts:
  - `objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1`
  - `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- Authoritative artifacts:
  - `module.runtime-import-surface.json`
  - `module.runtime-registration-manifest.json`

## Semantic Models

- Authority model:
  `serialized-runtime-import-surface-reuse-payload-plus-local-registration-manifest`
- Input model:
  `filesystem-runtime-import-surface-artifact-path-list-plus-local-registration-manifest`
- Registration scope model:
  `translation-unit-registration-manifests-remain-image-local-until-cross-module-registration-aggregation-lands`
- Packaging model:
  `no-cross-module-link-plan-artifact-or-imported-registration-manifest-ingest-during-freeze`

## Required Freeze Rules

1. The compiler publishes one deterministic orchestration surface derived from
   the live `M258-C002` serialized reuse payload plus the emitted local runtime
   registration manifest.
2. The surface must account for:
   - the lexicographic module-image set derived from the transitive reuse
     payload
   - the direct imported-runtime-surface input count
   - the local class/protocol/category/property/ivar descriptor counts from the
     emitted registration manifest
   - the transitive runtime-owned declaration and metadata-reference counts
3. The surface must remain fail closed:
   - cross-module link-plan artifacts are not landed
   - imported registration-manifest loading is not landed
   - runtime-archive aggregation is not landed
   - cross-module runtime registration is not landed
   - cross-module launch orchestration is not landed
   - public cross-module orchestration ABI is not landed
   - `ready_for_packaging_and_runtime_registration_impl` remains `false`
4. `ir/objc3_ir_emitter.cpp` remains explicit that cross-module link/runtime
   orchestration still sits above the IR emitter in this lane.
5. `libobjc3c_frontend/api.h` remains explicit that the public embedding ABI
   exposes no cross-module orchestration handles yet.

## Non-Goals

- `M258-D001` does not emit a cross-module link-plan artifact.
- `M258-D001` does not ingest imported registration manifests.
- `M258-D001` does not aggregate runtime archives across module boundaries.
- `M258-D001` does not launch aggregated cross-module runtime registration.
- `M258-D001` does not widen the public frontend ABI with orchestration
  handles.

## Validation

- `python scripts/check_m258_d001_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m258_d001_cross_module_build_and_runtime_orchestration_contract_and_architecture_freeze.py -q`
- `python scripts/run_m258_d001_lane_d_readiness.py`

## Evidence

- `tmp/reports/m258/M258-D001/cross_module_build_runtime_orchestration_contract_summary.json`
