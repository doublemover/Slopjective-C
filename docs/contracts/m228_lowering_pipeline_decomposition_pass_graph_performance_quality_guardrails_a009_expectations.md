# M228 Lowering Pipeline Decomposition and Pass-Graph Performance and Quality Guardrails Expectations (A011)

Contract ID: `objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`
Status: Accepted
Scope: pass-graph performance/quality guardrails synthesis, fail-closed artifact gate hardening, IR metadata projection, and architecture/spec anchors.

## Objective

Extend A010 conformance-corpus closure with explicit performance/quality
consistency and readiness/key gates so pass-graph quality drift fails closed
before IR emission.

## Required Invariants

1. Core-feature surface carries performance/quality fields:
   - `performance_quality_guardrails_consistent`
   - `performance_quality_guardrails_ready`
   - `performance_quality_guardrails_key`
2. Performance/quality key synthesis is deterministic:
   - `BuildObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsKey(...)`
3. Frontend artifacts fail-close on lane-A performance/quality before IR emission:
   - `IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady(...)`
   - deterministic diagnostic code `O3L315`.
4. IR frontend metadata carries performance/quality readiness and key:
   - `Objc3IRFrontendMetadata::lowering_pass_graph_performance_quality_guardrails_ready`
   - `Objc3IRFrontendMetadata::lowering_pass_graph_performance_quality_guardrails_key`
   - IR output includes:
     - `; lowering_pass_graph_performance_quality_guardrails = ...`
     - `; lowering_pass_graph_performance_quality_guardrails_ready = ...`
5. A010 remains a mandatory prerequisite:
   - `docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_conformance_corpus_expansion_a010_expectations.md`
   - `scripts/check_m228_a010_lowering_pipeline_decomposition_pass_graph_conformance_corpus_contract.py`
   - `tests/tooling/test_check_m228_a010_lowering_pipeline_decomposition_pass_graph_conformance_corpus_contract.py`
6. A011 planning/checker/test anchors remain mandatory:
   - `spec/planning/compiler/m228/m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_packet.md`
   - `scripts/check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py`
   - `tests/tooling/test_check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py`
7. Architecture/spec anchors are updated:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py -q`
- `npm run check:objc3c:m228-a011-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A011/lowering_pipeline_pass_graph_performance_quality_guardrails_contract_summary.json`
