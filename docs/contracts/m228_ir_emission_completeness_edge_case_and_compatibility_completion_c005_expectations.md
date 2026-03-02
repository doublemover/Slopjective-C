# M228 IR Emission Completeness Edge-Case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`
Status: Accepted
Scope: lane-C edge-case and compatibility completion for IR emission completeness on top of C004 core-feature expansion.

## Objective

Complete lane-C IR-emission closure by threading pass-graph edge-case
compatibility, parse/lowering compatibility handoff consistency, and
edge-case key transport into the IR core-feature surface so emission remains
deterministic and fail-closed on compatibility drift.

## Dependency Scope

- Dependencies: `M228-C004`
- M228-C004 core-feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m228_ir_emission_completeness_core_feature_expansion_c004_expectations.md`
  - `scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py`
  - `spec/planning/compiler/m228/m228_c004_ir_emission_completeness_core_feature_expansion_packet.md`
- Packet/checker/test assets for C005 remain mandatory:
  - `spec/planning/compiler/m228/m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C005
   compatibility completion guardrails:
   - `pass_graph_edge_case_compatibility_ready`
   - `compatibility_handoff_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `parse_artifact_edge_case_robustness_consistent`
   - `parse_artifact_replay_key_deterministic`
   - `edge_case_compatibility_key_transport_ready`
   - `core_feature_edge_case_compatibility_ready`
   - `edge_case_compatibility_key`
2. `BuildObjc3IREmissionCoreFeatureEdgeCaseCompatibilityKey(...)` remains
   deterministic and keyed by C004 expansion readiness plus parse/lowering
   compatibility handoff and edge-case key transport anchors.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C005
   through `IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(...)` with
   deterministic diagnostic code `O3L316`.
4. IR metadata transport includes C005 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_edge_case_compatibility_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_edge_case_compatibility_key`
   - IR text lines:
     - `; ir_emission_core_feature_edge_case_compatibility = ...`
     - `; ir_emission_core_feature_edge_case_compatibility_ready = ...`
5. Failure reasons remain explicit for pass-graph compatibility drift,
   compatibility handoff inconsistency, parse edge-case robustness drift,
   parse replay determinism drift, and key transport readiness drift.

## Shared-File Deltas (Required, Not Lane-Owned)

The following shared files require explicit C005 anchor updates by their owners:

- `package.json`
  - add/check commands:
    - `check:objc3c:m228-c005-ir-emission-completeness-edge-case-and-compatibility-completion-contract`
    - `test:tooling:m228-c005-ir-emission-completeness-edge-case-and-compatibility-completion-contract`
    - `check:objc3c:m228-c005-lane-c-readiness`
  - chain C004 readiness into C005 lane-C readiness.
- `native/objc3c/src/ARCHITECTURE.md`
  - add lane-C C005 edge-case compatibility completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add deterministic fail-closed C005 IR-emission compatibility completion anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C005 readiness/key metadata anchor text.

## Validation

- `python scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py`
- `python scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py -q`

## Evidence Path

- `tmp/reports/m228/M228-C005/ir_emission_completeness_edge_case_and_compatibility_completion_contract_summary.json`
- `tmp/reports/m228/M228-C005/closeout_validation_report.md`
