# M228 IR Emission Completeness Performance and Quality Guardrails Expectations (C011)

Contract ID: `objc3c-ir-emission-completeness-performance-quality-guardrails/m228-c011-v1`
Status: Accepted
Scope: lane-C IR-emission performance and quality guardrails closure on top of C010 conformance corpus expansion governance.

## Objective

Execute issue `#5227` by locking deterministic lane-C performance/quality
guardrails continuity over canonical dependency anchors, command sequencing, and
evidence paths so readiness remains fail-closed when dependency or sequencing
drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M228-C010`
- `M228-C010` remains a mandatory prerequisite:
  - `docs/contracts/m228_ir_emission_completeness_conformance_corpus_expansion_c010_expectations.md`
  - `scripts/check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py`
  - `spec/planning/compiler/m228/m228_c010_ir_emission_completeness_conformance_corpus_expansion_packet.md`
- Packet/checker/test assets for C011 remain mandatory:
  - `spec/planning/compiler/m228/m228_c011_ir_emission_completeness_performance_quality_guardrails_packet.md`
  - `scripts/check_m228_c011_ir_emission_completeness_performance_quality_guardrails_contract.py`
  - `tests/tooling/test_check_m228_c011_ir_emission_completeness_performance_quality_guardrails_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C011
   performance/quality guardrail anchors:
   - `pass_graph_performance_quality_guardrails_ready`
   - `parse_artifact_performance_quality_guardrails_consistent`
   - `performance_quality_guardrails_consistent`
   - `performance_quality_guardrails_key_transport_ready`
   - `core_feature_performance_quality_guardrails_ready`
   - `pass_graph_performance_quality_guardrails_key`
   - `parse_artifact_performance_quality_guardrails_key`
   - `performance_quality_guardrails_key`
2. `BuildObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsKey(...)` remains
   deterministic and keyed by C010 conformance-corpus closure plus pass-graph
   and parse performance/quality continuity.
3. `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
   performance/quality fail-closed from conformance-corpus readiness and
   pass-graph/parse performance-quality consistency plus key transport
   continuity.
4. `IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(...)` fails
   closed when performance/quality consistency/readiness or key transport
   drifts.
5. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C011
   through `IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(...)`
   with deterministic diagnostic code `O3L331`.
6. IR metadata transport includes C011 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_performance_quality_guardrails_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_performance_quality_guardrails_key`
   - IR text lines:
     - `; ir_emission_core_feature_performance_quality_guardrails = ...`
     - `; ir_emission_core_feature_performance_quality_guardrails_ready = ...`

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c011-ir-emission-completeness-performance-quality-guardrails-contract`
  - add/check `test:tooling:m228-c011-ir-emission-completeness-performance-quality-guardrails-contract`
  - add/check `check:objc3c:m228-c011-lane-c-readiness` chained from C010 -> C011
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C011 performance and quality guardrails anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C011 fail-closed performance-quality guardrails wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C011 performance-quality guardrails metadata anchors

## Validation

- `python scripts/check_m228_c010_ir_emission_completeness_conformance_corpus_expansion_contract.py`
- `python scripts/check_m228_c011_ir_emission_completeness_performance_quality_guardrails_contract.py --summary-out tmp/reports/m228/M228-C011/ir_emission_completeness_performance_quality_guardrails_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c011_ir_emission_completeness_performance_quality_guardrails_contract.py -q`
- `npm run check:objc3c:m228-c011-lane-c-readiness`

## Evidence Path

- `tmp/reports/m228/M228-C011/ir_emission_completeness_performance_quality_guardrails_contract_summary.json`
- `tmp/reports/m228/M228-C011/closeout_validation_report.md`
