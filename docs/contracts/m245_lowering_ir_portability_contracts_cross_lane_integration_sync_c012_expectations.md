# M245 Lowering/IR Portability Contracts Cross-Lane Integration Sync Expectations (C012)

Contract ID: `objc3c-lowering-ir-portability-contracts-cross-lane-integration-sync/m245-c012-v1`
Status: Accepted
Dependencies: `M245-C011`
Scope: M245 lane-C lowering/IR portability contracts cross-lane integration sync continuity with explicit `M245-C011` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts cross-lane
integration sync anchors remain explicit, deterministic, and traceable across
dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6647` defines canonical lane-C cross-lane integration sync scope.
- Dependency token: `M245-C011`.
- Upstream C011 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_performance_and_quality_guardrails_c011_expectations.md`
  - `spec/planning/compiler/m245/m245_c011_lowering_ir_portability_contracts_performance_and_quality_guardrails_packet.md`
  - `scripts/check_m245_c011_lowering_ir_portability_contracts_performance_and_quality_guardrails_contract.py`
  - `tests/tooling/test_check_m245_c011_lowering_ir_portability_contracts_performance_and_quality_guardrails_contract.py`
- C012 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_packet.md`
  - `scripts/check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py`
  - `tests/tooling/test_check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C012 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and M245-C011 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py`
- `python scripts/check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c012_lowering_ir_portability_contracts_cross_lane_integration_sync_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C012/lowering_ir_portability_contracts_cross_lane_integration_sync_contract_summary.json`
