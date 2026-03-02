# M226 Parse-Lowering Recovery Determinism Hardening Expectations (C008)

Contract ID: `objc3c-parse-lowering-recovery-determinism-hardening-contract/m226-c008-v1`
Status: Accepted
Scope: Parse/lowering recovery-determinism hardening coverage in `native/objc3c/src/pipeline/*`.

## Objective

Require fail-closed parse-to-lowering readiness recovery invariants that bind
parser contract determinism, replay keys, diagnostics hardening, and edge-case
robustness into one deterministic recovery hardening gate.

## Required Invariants

1. Readiness surface tracks recovery/determinism hardening evidence:
   - `parse_recovery_determinism_hardening_consistent`
   - `parse_recovery_determinism_hardening_key`
2. Readiness builder computes a dedicated recovery hardening key via:
   - `BuildObjc3ParseRecoveryDeterminismHardeningKey(...)`
3. Recovery hardening consistency is fail-closed and composes parser contract,
   replay, diagnostics hardening, and edge robustness invariants.
4. Parse snapshot replay readiness requires recovery hardening consistency.
5. Readiness failure reason includes:
   - `parse recovery/determinism hardening is inconsistent`
6. Manifest projection includes recovery hardening fields under
   `parse_lowering_readiness`:
   - `parse_recovery_determinism_hardening_consistent`
   - `parse_recovery_determinism_hardening_key`
7. Validation entrypoints are pinned:
   - `python scripts/check_m226_c008_parse_lowering_recovery_determinism_hardening_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_c008_parse_lowering_recovery_determinism_hardening_contract.py -q`

## Validation

- `python scripts/check_m226_c008_parse_lowering_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c008_parse_lowering_recovery_determinism_hardening_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/m226_c008_parse_lowering_recovery_determinism_hardening_contract_summary.json`
