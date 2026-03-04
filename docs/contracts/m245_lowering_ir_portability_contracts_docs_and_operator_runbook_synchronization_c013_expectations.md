# M245 Lowering/IR Portability Contracts Docs and Operator Runbook Synchronization Expectations (C013)

Contract ID: `objc3c-lowering-ir-portability-contracts-docs-and-operator-runbook-synchronization/m245-c013-v1`
Status: Accepted
Dependencies: `M245-C012`
Scope: M245 lane-C lowering/IR portability contracts docs and operator runbook synchronization continuity with explicit `M245-C012` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts docs and operator
runbook synchronization anchors remain explicit, deterministic, and traceable
across dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6648` defines canonical lane-C docs and operator runbook synchronization scope.
- Dependency token: `M245-C012`.
- Upstream C012 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_cross_lane_integration_sync_c012_expectations.md`
  - `spec/planning/compiler/m245/m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_packet.md`
  - `scripts/check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py`
  - `tests/tooling/test_check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py`
- C013 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_packet.md`
  - `scripts/check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py`
  - `tests/tooling/test_check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C013 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and M245-C012 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py`
- `python scripts/check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C013/lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract_summary.json`
