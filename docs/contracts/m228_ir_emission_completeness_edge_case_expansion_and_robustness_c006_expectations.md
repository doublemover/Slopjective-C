# M228 IR Emission Completeness Edge-Case Expansion and Robustness Expectations (C006)

Contract ID: `objc3c-ir-emission-completeness-edge-case-expansion-and-robustness/m228-c006-v1`
Status: Accepted
Scope: lane-C edge-case expansion and robustness hardening for IR emission completeness on top of C005 compatibility completion.

## Objective

Expand C005 compatibility closure with explicit edge-case expansion consistency
and edge-case robustness readiness/key transport so direct LLVM IR emission
remains deterministic and fail-closed on robustness drift.

## Dependency Scope

- Dependencies: `M228-C005`
- M228-C005 edge-case compatibility completion anchors remain mandatory
  prerequisites:
  - `docs/contracts/m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md`
  - `scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
  - `spec/planning/compiler/m228/m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_packet.md`
- Packet/checker/test assets for C006 remain mandatory:
  - `spec/planning/compiler/m228/m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C006
   edge-case expansion/robustness guardrails:
   - `pass_graph_edge_case_robustness_ready`
   - `edge_case_expansion_consistent`
   - `parse_artifact_edge_case_robustness_ready`
   - `edge_case_robustness_key_transport_ready`
   - `core_feature_edge_case_robustness_ready`
   - `pass_graph_edge_case_robustness_key`
   - `parse_artifact_edge_case_expansion_key`
   - `edge_case_robustness_key`
2. `BuildObjc3IREmissionCoreFeatureEdgeCaseRobustnessKey(...)` remains
   deterministic and keyed by C005 compatibility closure plus edge-case
   expansion/robustness transport anchors.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C006
   through `IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(...)` with
   deterministic diagnostic code `O3L317`.
4. IR metadata transport includes C006 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_edge_case_robustness_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_edge_case_robustness_key`
   - IR text lines:
     - `; ir_emission_core_feature_edge_case_robustness = ...`
     - `; ir_emission_core_feature_edge_case_robustness_ready = ...`
5. Failure reasons remain explicit for edge-case expansion inconsistency,
   parse-edge robustness readiness drift, pass-graph robustness drift, and
   edge-case robustness key transport drift.

## Shared-File Deltas (Required, Not Lane-Owned)

The following shared files require explicit C006 anchor updates by their
owners:

- `package.json`
  - add/check commands:
    - `check:objc3c:m228-c006-ir-emission-completeness-edge-case-expansion-and-robustness-contract`
    - `test:tooling:m228-c006-ir-emission-completeness-edge-case-expansion-and-robustness-contract`
    - `check:objc3c:m228-c006-lane-c-readiness`
  - chain C005 readiness into C006 lane-C readiness.
- `native/objc3c/src/ARCHITECTURE.md`
  - add lane-C C006 edge-case expansion and robustness anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add deterministic fail-closed C006 IR-emission edge-case robustness anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C006 robustness readiness/key metadata anchor text.

## Validation

- `python scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py -q`

## Evidence Path

- `tmp/reports/m228/M228-C006/ir_emission_completeness_edge_case_expansion_and_robustness_contract_summary.json`
- `tmp/reports/m228/M228-C006/closeout_validation_report.md`
