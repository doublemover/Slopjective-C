# M227 Semantic Pass Edge Robustness Expectations (A006)

Contract ID: `objc3c-sema-pass-edge-robustness/m227-a006-v1`
Status: Accepted
Scope: duplicate/missing pass execution accounting and robustness guardrails for semantic pass-flow summaries.

## Objective

Expand semantic pass-flow edge robustness so duplicate pass execution, missing pass execution, and transition consistency are explicitly tracked and fail-closed.

## Required Invariants

1. Pass-flow summary includes robustness fields:
   - `duplicate_pass_execution_count`
   - `missing_pass_execution_count`
   - `robustness_guardrails_satisfied`
2. Pass execution marker tracks duplicates:
   - `MarkObjc3SemaPassExecuted(...)` increments duplicate counter when the same pass is observed more than once.
3. Finalization computes missing count and robustness guardrails:
   - Missing pass count derives from `pass_executed` coverage.
   - Guardrails require zero duplicates, zero missing passes, stable transition count, and diagnostics consistency.
4. Frontend artifacts project robustness fields under `sema_pass_manager`:
   - `pass_flow_duplicate_execution_count`
   - `pass_flow_missing_execution_count`
   - `pass_flow_robustness_guardrails_satisfied`

## Validation

- `python scripts/check_m227_a006_semantic_pass_edge_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a006_semantic_pass_edge_robustness_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A006/semantic_pass_edge_robustness_contract_summary.json`
