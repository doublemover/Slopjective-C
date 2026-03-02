# M226 Parser-to-Sema Recovery and Determinism Hardening Expectations (B008)

Contract ID: `objc3c-parser-sema-recovery-determinism-hardening-contract/m226-b008-v1`
Status: Accepted
Scope: Parser->sema recovery and block-determinism handoff hardening (depends on `M226-B007`).

## Objective

Harden parser->sema handoff so recovery summary drift and block determinism drift
fail closed before downstream parity projection continues, while preserving
deterministic recovery and block baseline invariants.

## Required Invariants

1. Error diagnostics recovery handoff tightens fail-closed accounting:
   - `fail_closed_diagnostic_sites <= diagnostic_emit_sites`
2. Block determinism handoff requires metadata vector equivalence, not only
   summary equivalence:
   - `AreEquivalentBlockDeterminismPerfBaselineSites(...)`
3. Recovery + block determinism gating is fail-closed and contributes directly
   to type metadata handoff determinism:
   - `recovery_and_block_determinism_hardening_consistent`
   - `if (!recovery_and_block_determinism_hardening_consistent) { return result; }`
4. Parity/readiness contract keeps recovery fail-closed accounting aligned with
   emission bounds:
   - `fail_closed_diagnostic_sites <= diagnostic_emit_sites`
5. Checker and tests are fail-closed and write summary output under
   `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_b008_parser_sema_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b008_parser_sema_recovery_determinism_hardening_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B008/parser_sema_recovery_determinism_hardening_summary.json`
