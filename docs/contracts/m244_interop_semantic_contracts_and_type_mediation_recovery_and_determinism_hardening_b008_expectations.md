# M244 Interop Semantic Contracts and Type Mediation Recovery and Determinism Hardening Expectations (B008)

Contract ID: `objc3c-interop-semantic-contracts-and-type-mediation-recovery-and-determinism-hardening/m244-b008-v1`
Status: Accepted
Dependencies: `M244-B007`
Scope: lane-B interop semantic contracts/type mediation recovery and determinism hardening governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-B recovery and determinism hardening governance for interop semantic contracts
and type mediation on top of B007 diagnostics hardening assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6538` defines canonical lane-B recovery and determinism hardening scope.
- `M244-B007` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_semantic_contracts_and_type_mediation_diagnostics_hardening_b007_expectations.md`
  - `spec/planning/compiler/m244/m244_b007_interop_semantic_contracts_and_type_mediation_diagnostics_hardening_packet.md`
  - `scripts/check_m244_b007_interop_semantic_contracts_and_type_mediation_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m244_b007_interop_semantic_contracts_and_type_mediation_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. lane-B recovery and determinism hardening dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-B007` before `M244-B008`
   evidence checks run.
3. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-b008-interop-semantic-contracts-type-mediation-recovery-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m244-b008-interop-semantic-contracts-type-mediation-recovery-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m244-b008-lane-b-readiness`.
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-b007-lane-b-readiness`
  - `check:objc3c:m244-b008-lane-b-readiness`

## Milestone Optimization Improvements

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.
- Milestone optimization improvements are mandatory scope inputs.

## Validation

- `python scripts/check_m244_b008_interop_semantic_contracts_and_type_mediation_recovery_and_determinism_hardening_contract.py`
- `python scripts/check_m244_b008_interop_semantic_contracts_and_type_mediation_recovery_and_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_b008_interop_semantic_contracts_and_type_mediation_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m244-b008-lane-b-readiness`

## Evidence Path

- `tmp/reports/m244/M244-B008/interop_semantic_contracts_and_type_mediation_recovery_and_determinism_hardening_contract_summary.json`
