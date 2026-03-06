# M228 IR Emission Completeness Advanced Performance Workpack (Shard 3) Expectations (C032)

Contract ID: `objc3c-ir-emission-completeness-advanced-performance-workpack-shard3/m228-c032-v1`
Status: Accepted
Scope: lane-C IR-emission advanced performance workpack (shard 3) closure on top of C031 advanced-conformance-shard1 governance.

## Objective

Execute issue `#5258` by locking deterministic lane-C advanced-integration-shard1
continuity over IR-emission readiness surfaces, typed/parse integration
handoff alignment, and emitted IR metadata anchors so readiness remains
fail-closed when dependency, key-transport, or sequencing drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M228-C031`
- `M228-C031` remains a mandatory prerequisite:
  - `docs/contracts/m228_ir_emission_completeness_advanced_integration_workpack_shard3_c031_expectations.md`
  - `scripts/check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py`
  - `tests/tooling/test_check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py`
  - `spec/planning/compiler/m228/m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_packet.md`
- Packet/checker/test assets for C032 remain mandatory:
  - `spec/planning/compiler/m228/m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_packet.md`
  - `scripts/check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py`
  - `tests/tooling/test_check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C032
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
   remains deterministic and keyed by C031 closure continuity plus parse/typed
   advanced-integration shard1 alignment.
3. `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
   advanced-integration shard1 fail-closed from C031 continuity and
   parse/typed advanced-integration consistency plus key-transport continuity.
4. `IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(...)`
   fails closed when advanced-integration shard1 consistency/readiness
   or key-transport continuity drifts.
5. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C032
   through `IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(...)`
   with deterministic diagnostic code `O3L337`.
6. IR metadata transport includes C032 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_integration_shard1_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_integration_shard1_key`
   - IR text lines:
     - `; ir_emission_core_feature_advanced_integration_shard1 = ...`
     - `; ir_emission_core_feature_advanced_integration_shard1_ready = ...`

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c032-ir-emission-completeness-advanced-performance-workpack-shard3-contract`
  - add/check `test:tooling:m228-c032-ir-emission-completeness-advanced-performance-workpack-shard3-contract`
  - add/check `check:objc3c:m228-c032-lane-c-readiness` chained from C031 -> C032
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add M228 lane-C C032 contract and validation command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C032 advanced-integration shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C032 fail-closed advanced-integration shard1 wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C032 advanced-integration shard1 metadata anchors

## Validation

- `python scripts/check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py`
- `python scripts/check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py --summary-out tmp/reports/m228/M228-C032/ir_emission_completeness_advanced_performance_workpack_shard3_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py -q`
- `npm run check:objc3c:m228-c032-lane-c-readiness`

## Evidence Path

- `tmp/reports/m228/M228-C032/ir_emission_completeness_advanced_performance_workpack_shard3_contract_summary.json`
- `tmp/reports/m228/M228-C032/closeout_validation_report.md`


















