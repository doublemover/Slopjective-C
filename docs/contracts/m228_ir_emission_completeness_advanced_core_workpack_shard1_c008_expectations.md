# M228 IR Emission Completeness Advanced Core Workpack (Shard 1) Expectations (C015)

Contract ID: `objc3c-ir-emission-completeness-advanced-core-workpack-shard1/m228-c015-v1`
Status: Accepted
Scope: lane-C IR-emission advanced core workpack (shard 1) closure on top of C014 release/replay governance.

## Objective

Execute issue `#5241` by locking deterministic lane-C advanced-core-shard1
continuity over IR-emission readiness surfaces, typed/parse advanced-core
handoff alignment, and emitted IR metadata anchors so readiness remains
fail-closed when dependency, key-transport, or sequencing drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M228-C014`
- `M228-C014` remains a mandatory prerequisite:
  - `docs/contracts/m228_ir_emission_completeness_release_candidate_and_replay_dry_run_c014_expectations.md`
  - `scripts/check_m228_c014_ir_emission_completeness_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m228_c014_ir_emission_completeness_release_candidate_and_replay_dry_run_contract.py`
  - `spec/planning/compiler/m228/m228_c014_ir_emission_completeness_release_candidate_and_replay_dry_run_packet.md`
- Packet/checker/test assets for C015 remain mandatory:
  - `spec/planning/compiler/m228/m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_packet.md`
  - `scripts/check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C015
   advanced-core shard1 guardrails:
   - `pass_graph_advanced_core_shard1_ready`
   - `parse_artifact_advanced_core_shard1_consistent`
   - `typed_handoff_advanced_core_shard1_consistent`
   - `advanced_core_shard1_consistent`
   - `advanced_core_shard1_key_transport_ready`
   - `core_feature_advanced_core_shard1_ready`
   - `pass_graph_advanced_core_shard1_key`
   - `parse_artifact_advanced_core_shard1_key`
   - `typed_handoff_advanced_core_shard1_key`
   - `advanced_core_shard1_key`
2. `BuildObjc3IREmissionCoreFeatureAdvancedCoreShard1Key(...)` remains
   deterministic and keyed by C014 closure continuity plus parse/typed
   advanced-core shard1 alignment.
3. `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
   advanced-core shard1 fail-closed from cross-lane sync continuity and
   parse/typed advanced-core shard1 consistency plus key-transport continuity.
4. `IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(...)` fails closed when
   advanced-core shard1 consistency/readiness or key-transport continuity drifts.
5. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C015
   through `IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(...)` with
   deterministic diagnostic code `O3L333`.
6. IR metadata transport includes C015 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_core_shard1_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_advanced_core_shard1_key`
   - IR text lines:
     - `; ir_emission_core_feature_advanced_core_shard1 = ...`
     - `; ir_emission_core_feature_advanced_core_shard1_ready = ...`

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c015-ir-emission-completeness-advanced-core-workpack-shard1-contract`
  - add/check `test:tooling:m228-c015-ir-emission-completeness-advanced-core-workpack-shard1-contract`
  - add/check `check:objc3c:m228-c015-lane-c-readiness` chained from C014 -> C015
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add M228 lane-C C015 contract and validation command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C015 advanced-core shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C015 fail-closed advanced-core shard1 wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C015 advanced-core shard1 metadata anchors

## Validation

- `python scripts/check_m228_c014_ir_emission_completeness_release_candidate_and_replay_dry_run_contract.py`
- `python scripts/check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py --summary-out tmp/reports/m228/M228-C015/ir_emission_completeness_advanced_core_workpack_shard1_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m228-c015-lane-c-readiness`

## Evidence Path

- `tmp/reports/m228/M228-C015/ir_emission_completeness_advanced_core_workpack_shard1_contract_summary.json`
- `tmp/reports/m228/M228-C015/closeout_validation_report.md`
