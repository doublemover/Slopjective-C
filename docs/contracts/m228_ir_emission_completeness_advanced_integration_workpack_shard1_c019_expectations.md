# M228 IR Emission Completeness Advanced Integration Workpack (Shard 1) Expectations (C019)

Contract ID: `objc3c-ir-emission-completeness-advanced-integration-workpack-shard1/m228-c019-v1`
Status: Accepted
Scope: lane-C IR-emission advanced integration workpack (shard 1) closure on top of C018 advanced-conformance-shard1 governance.

## Objective

Execute issue `#5245` by locking deterministic lane-C advanced-integration-shard1
continuity over IR-emission readiness surfaces, typed/parse integration
handoff alignment, and emitted IR metadata anchors so readiness remains
fail-closed when dependency, key-transport, or sequencing drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M228-C018`
- `M228-C018` remains a mandatory prerequisite:
  - `docs/contracts/m228_ir_emission_completeness_advanced_conformance_workpack_shard1_c018_expectations.md`
  - `scripts/check_m228_c018_ir_emission_completeness_advanced_conformance_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_c018_ir_emission_completeness_advanced_conformance_workpack_shard1_contract.py`
  - `spec/planning/compiler/m228/m228_c018_ir_emission_completeness_advanced_conformance_workpack_shard1_packet.md`
- Packet/checker/test assets for C019 remain mandatory:
  - `spec/planning/compiler/m228/m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_packet.md`
  - `scripts/check_m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C019
   advanced-integration shard1 guardrails:
   - `pass_graph_advanced_integration_shard1_ready`
   - `parse_artifact_advanced_integration_shard1_consistent`
   - `typed_handoff_advanced_integration_shard1_consistent`
   - `advanced_integration_shard1_consistent`
   - `advanced_integration_shard1_key_transport_ready`
   - `core_feature_advanced_integration_shard1_ready`
   - `pass_graph_advanced_integration_shard1_key`
   - `parse_artifact_advanced_integration_shard1_key`
   - `typed_handoff_advanced_integration_shard1_key`
   - `advanced_integration_shard1_key`
2. `BuildObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Key(...)`
   remains deterministic and keyed by C018 closure continuity plus parse/typed
   advanced-integration shard1 alignment.
3. `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
   advanced-integration shard1 fail-closed from C018 continuity and
   parse/typed advanced-integration consistency plus key-transport continuity.
4. `IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(...)`
   fails closed when advanced-integration shard1 consistency/readiness
   or key-transport continuity drifts.
5. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C019
   through `IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(...)`
   with deterministic diagnostic code `O3L337`.
6. IR metadata transport includes C019 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_integration_shard1_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_integration_shard1_key`
   - IR text lines:
     - `; ir_emission_core_feature_advanced_integration_shard1 = ...`
     - `; ir_emission_core_feature_advanced_integration_shard1_ready = ...`

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c019-ir-emission-completeness-advanced-integration-workpack-shard1-contract`
  - add/check `test:tooling:m228-c019-ir-emission-completeness-advanced-integration-workpack-shard1-contract`
  - add/check `check:objc3c:m228-c019-lane-c-readiness` chained from C018 -> C019
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add M228 lane-C C019 contract and validation command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C019 advanced-integration shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C019 fail-closed advanced-integration shard1 wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C019 advanced-integration shard1 metadata anchors

## Validation

- `python scripts/check_m228_c018_ir_emission_completeness_advanced_conformance_workpack_shard1_contract.py`
- `python scripts/check_m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_contract.py --summary-out tmp/reports/m228/M228-C019/ir_emission_completeness_advanced_integration_workpack_shard1_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m228-c019-lane-c-readiness`

## Evidence Path

- `tmp/reports/m228/M228-C019/ir_emission_completeness_advanced_integration_workpack_shard1_contract_summary.json`
- `tmp/reports/m228/M228-C019/closeout_validation_report.md`





