# M227 Semantic Pass Diagnostics Hardening Expectations (A007)

Contract ID: `objc3c-sema-pass-diagnostics-hardening/m227-a007-v1`
Status: Accepted
Scope: semantic pass diagnostics accounting, parity transport, and artifact projection.

## Objective

Harden diagnostics determinism by explicitly carrying diagnostics accounting/canonicalization consistency through sema pass flow summaries and parity surfaces.

## Deterministic Invariants

1. `Objc3SemaPassFlowSummary` carries diagnostics hardening fields:
   - `diagnostics_accounting_consistent`
   - `diagnostics_bus_publish_consistent`
   - `diagnostics_canonicalized`
   - `diagnostics_hardening_satisfied`
2. `Objc3SemaParityContractSurface` carries the same diagnostics hardening fields and requires them in readiness.
3. `RunObjc3SemaPassManager(...)` computes diagnostics hardening booleans from per-pass canonicalization + bus accounting and wires them into:
   - manager result fields
   - pass flow summary
   - parity surface
4. Artifact projection emits diagnostics hardening booleans for both parity and pass-flow payloads.

## Validation

- `python scripts/check_m227_a007_semantic_pass_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a007_semantic_pass_diagnostics_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A007/semantic_pass_diagnostics_hardening_contract_summary.json`
