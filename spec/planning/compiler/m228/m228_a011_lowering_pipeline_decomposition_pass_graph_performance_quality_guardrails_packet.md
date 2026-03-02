# M228-A011 Lowering Pipeline Decomposition and Pass-Graph Performance and Quality Guardrails Packet

Packet: `M228-A011`
Milestone: `M228`
Lane: `A`
Freeze date: `2026-03-02`
Dependencies: `M228-A010`

## Purpose

Freeze lane-A performance/quality guardrails closure for lowering pipeline
pass-graph decomposition so readiness/key transport remains deterministic and
fail-closed before IR emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_a011_expectations.md`
- Checker:
  `scripts/check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py`
- Dependency anchors (`M228-A010`):
  - `docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_conformance_corpus_expansion_a010_expectations.md`
  - `scripts/check_m228_a010_lowering_pipeline_decomposition_pass_graph_conformance_corpus_contract.py`
  - `tests/tooling/test_check_m228_a010_lowering_pipeline_decomposition_pass_graph_conformance_corpus_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m228-a011-lowering-pipeline-pass-graph-performance-quality-guardrails-contract`
  - `test:tooling:m228-a011-lowering-pipeline-pass-graph-performance-quality-guardrails-contract`
  - `check:objc3c:m228-a011-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a011_lowering_pipeline_decomposition_pass_graph_performance_quality_guardrails_contract.py -q`
- `npm run check:objc3c:m228-a011-lane-a-readiness`

## Evidence Output

- `tmp/reports/m228/M228-A011/lowering_pipeline_pass_graph_performance_quality_guardrails_contract_summary.json`
