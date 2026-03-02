# M227 Semantic Pass Recovery and Determinism Hardening Expectations (A008)

Contract ID: `objc3c-sema-pass-recovery-determinism-hardening/m227-a008-v1`
Status: Accepted
Scope: semantic pass-flow recovery replay invariants and deterministic replay-key transport.

## Objective

Harden sema pass-flow recovery/determinism gates so parser recovery replay invariants are explicitly encoded in pass-flow summary, parity surface, and projected artifacts.

## Deterministic Invariants

1. `Objc3SemaPassFlowSummary` encodes recovery replay gates:
   - `parser_recovery_replay_ready`
   - `parser_recovery_replay_case_present`
   - `parser_recovery_replay_case_passed`
   - `recovery_replay_contract_satisfied`
   - `recovery_replay_key`
   - `recovery_replay_key_deterministic`
   - `recovery_determinism_hardening_satisfied`
2. `IsReadyObjc3SemaPassFlowSummary(...)` requires all recovery replay gates and non-empty deterministic replay key.
3. `RunObjc3SemaPassManager(...)` derives recovery replay contract booleans from parser/sema conformance matrix+corpus and wires those fields into pass-flow summary.
4. Parity surface carries pass-flow recovery replay hardening fields and includes them in readiness.
5. Frontend artifact projection emits pass-flow recovery replay fields for both parity and pass-flow payloads.

## Validation

- `python scripts/check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A008/semantic_pass_recovery_determinism_hardening_contract_summary.json`
