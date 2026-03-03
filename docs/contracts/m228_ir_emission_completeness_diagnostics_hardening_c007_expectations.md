# M228 IR Emission Completeness Diagnostics Hardening Expectations (C007)

Contract ID: `objc3c-ir-emission-completeness-diagnostics-hardening/m228-c007-v1`
Status: Accepted
Scope: lane-C diagnostics hardening closure for IR emission completeness on top of C006 edge-case expansion and robustness.

## Objective

Extend lane-C IR-emission closure with deterministic diagnostics hardening
consistency/readiness and diagnostics-key transport so direct LLVM IR emission
fails closed when diagnostics continuity drifts.

## Dependency Scope

- Dependencies: `M228-C006`
- M228-C006 edge-case expansion and robustness anchors remain mandatory
  prerequisites:
  - `docs/contracts/m228_ir_emission_completeness_edge_case_expansion_and_robustness_c006_expectations.md`
  - `scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
  - `spec/planning/compiler/m228/m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_packet.md`
- Packet/checker/test assets for C007 remain mandatory:
  - `spec/planning/compiler/m228/m228_c007_ir_emission_completeness_diagnostics_hardening_packet.md`
  - `scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C007
   diagnostics hardening guardrails:
   - `pass_graph_diagnostics_hardening_ready`
   - `diagnostics_hardening_consistent`
   - `parse_artifact_diagnostics_hardening_consistent`
   - `diagnostics_hardening_key_transport_ready`
   - `core_feature_diagnostics_hardening_ready`
   - `pass_graph_diagnostics_hardening_key`
   - `parse_artifact_diagnostics_hardening_key`
   - `diagnostics_hardening_key`
2. `BuildObjc3IREmissionCoreFeatureDiagnosticsHardeningKey(...)` remains
   deterministic and keyed by C006 robustness closure plus pass-graph/parse
   diagnostics continuity.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C007
   through `IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(...)` with
   deterministic diagnostic code `O3L318`.
4. IR metadata transport includes C007 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_diagnostics_hardening_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_diagnostics_hardening_key`
   - IR text lines:
     - `; ir_emission_core_feature_diagnostics_hardening = ...`
     - `; ir_emission_core_feature_diagnostics_hardening_ready = ...`
5. Failure reasons remain explicit for pass-graph diagnostics hardening drift,
   parse diagnostics hardening inconsistency, and diagnostics-key transport
   drift.

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add `check:objc3c:m228-c007-ir-emission-completeness-diagnostics-hardening-contract`
  - add `test:tooling:m228-c007-ir-emission-completeness-diagnostics-hardening-contract`
  - add `check:objc3c:m228-c007-lane-c-readiness` chained from C006 -> C007
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C007 diagnostics hardening anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C007 fail-closed diagnostics hardening wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C007 diagnostics hardening metadata anchors

## Validation

- `python scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
- `python scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py --summary-out tmp/reports/m228/M228-C007/ir_emission_completeness_diagnostics_hardening_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m228/M228-C007/ir_emission_completeness_diagnostics_hardening_contract_summary.json`
- `tmp/reports/m228/M228-C007/closeout_validation_report.md`
