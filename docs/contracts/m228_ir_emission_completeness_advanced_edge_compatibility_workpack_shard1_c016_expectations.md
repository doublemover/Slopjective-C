# M228 IR Emission Completeness Advanced Edge Compatibility Workpack (Shard 1) Expectations (C016)

Contract ID: `objc3c-ir-emission-completeness-advanced-edge-compatibility-workpack-shard1/m228-c016-v1`
Status: Accepted
Scope: lane-C IR-emission advanced edge compatibility workpack (shard 1) closure on top of C015 advanced-core-shard1 governance.

## Objective

Execute issue `#5242` by locking deterministic lane-C advanced-edge-compatibility-shard1
continuity over IR-emission readiness surfaces, typed/parse edge-compatibility
handoff alignment, and emitted IR metadata anchors so readiness remains
fail-closed when dependency, key-transport, or sequencing drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M228-C015`
- `M228-C015` remains a mandatory prerequisite:
  - `docs/contracts/m228_ir_emission_completeness_advanced_core_workpack_shard1_c015_expectations.md`
  - `scripts/check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py`
  - `spec/planning/compiler/m228/m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_packet.md`
- Packet/checker/test assets for C016 remain mandatory:
  - `spec/planning/compiler/m228/m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_packet.md`
  - `scripts/check_m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C016
   advanced-edge-compatibility shard1 guardrails:
   - `pass_graph_advanced_edge_compatibility_shard1_ready`
   - `parse_artifact_advanced_edge_compatibility_shard1_consistent`
   - `typed_handoff_advanced_edge_compatibility_shard1_consistent`
   - `advanced_edge_compatibility_shard1_consistent`
   - `advanced_edge_compatibility_shard1_key_transport_ready`
   - `core_feature_advanced_edge_compatibility_shard1_ready`
   - `pass_graph_advanced_edge_compatibility_shard1_key`
   - `parse_artifact_advanced_edge_compatibility_shard1_key`
   - `typed_handoff_advanced_edge_compatibility_shard1_key`
   - `advanced_edge_compatibility_shard1_key`
2. `BuildObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Key(...)`
   remains deterministic and keyed by C015 closure continuity plus parse/typed
   advanced-edge-compatibility shard1 alignment.
3. `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
   advanced-edge-compatibility shard1 fail-closed from C015 continuity and
   parse/typed advanced-edge-compatibility consistency plus key-transport continuity.
4. `IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(...)`
   fails closed when advanced-edge-compatibility shard1 consistency/readiness
   or key-transport continuity drifts.
5. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C016
   through `IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(...)`
   with deterministic diagnostic code `O3L334`.
6. IR metadata transport includes C016 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_edge_compatibility_shard1_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_edge_compatibility_shard1_key`
   - IR text lines:
     - `; ir_emission_core_feature_advanced_edge_compatibility_shard1 = ...`
     - `; ir_emission_core_feature_advanced_edge_compatibility_shard1_ready = ...`

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c016-ir-emission-completeness-advanced-edge-compatibility-workpack-shard1-contract`
  - add/check `test:tooling:m228-c016-ir-emission-completeness-advanced-edge-compatibility-workpack-shard1-contract`
  - add/check `check:objc3c:m228-c016-lane-c-readiness` chained from C015 -> C016
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add M228 lane-C C016 contract and validation command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C016 advanced-edge-compatibility shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C016 fail-closed advanced-edge-compatibility shard1 wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C016 advanced-edge-compatibility shard1 metadata anchors

## Validation

- `python scripts/check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract.py --summary-out tmp/reports/m228/M228-C016/ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m228-c016-lane-c-readiness`

## Evidence Path

- `tmp/reports/m228/M228-C016/ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract_summary.json`
- `tmp/reports/m228/M228-C016/closeout_validation_report.md`
