# M227 Semantic Pass Decomposition and Symbol Flow Freeze Expectations (A001)

Contract ID: `objc3c-sema-pass-decomposition-and-symbol-flow-freeze/m227-a001-v1`
Status: Accepted
Scope: `native/objc3c/src/sema/*`, semantic pipeline handoff surfaces, and architecture anchors.

## Objective

Freeze semantic pass decomposition and symbol-flow invariants for M227 so type-system expansion can proceed without regressing deterministic pass execution order or sema-to-pipeline handoff stability.

## Required Invariants

1. Semantic pass manager contract defines explicit pass-flow summary:
   - `Objc3SemaPassFlowSummary` exists in `sema/objc3_sema_pass_manager_contract.h`.
   - Summary captures pass order, per-pass execution, diagnostics progression, and symbol/type-metadata count consistency.
2. Semantic pass manager execution populates pass-flow summary deterministically:
   - `RunObjc3SemaPassManager` marks each pass execution and pass-order conformance.
   - `diagnostics_after_pass` is recorded and monotonic.
   - Symbol counts from integration surface match type metadata handoff counts.
3. Parity surface embeds pass-flow freeze signal:
   - `Objc3SemaParityContractSurface` contains `sema_pass_flow_summary`.
   - `IsReadyObjc3SemaParityContractSurface` requires `IsReadyObjc3SemaPassFlowSummary(...)`.
4. Frontend pipeline transports sema pass-flow summary through contract boundaries:
   - `Objc3FrontendPipelineResult` includes `sema_pass_flow_summary`.
   - `pipeline/objc3_frontend_pipeline.cpp` assigns it from `Objc3SemaPassManagerResult`.
5. Architecture anchor remains authoritative:
   - `native/objc3c/src/ARCHITECTURE.md` documents M227 sema pass-order and symbol-flow freeze boundary.

## Validation

- `python scripts/check_m227_a001_semantic_pass_decomposition_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a001_semantic_pass_decomposition_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A001/semantic_pass_decomposition_contract_summary.json`
