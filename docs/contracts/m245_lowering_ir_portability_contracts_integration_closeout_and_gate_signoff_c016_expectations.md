# M245 Lowering/IR Portability Contracts Integration Closeout and Gate Sign-Off Expectations (C016)

Contract ID: `objc3c-lowering-ir-portability-contracts-integration-closeout-and-gate-signoff/m245-c016-v1`
Status: Accepted
Dependencies: `M245-C015`
Scope: M245 lane-C lowering/IR portability contracts integration closeout and gate sign-off continuity with explicit `M245-C015` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts integration
closeout and gate sign-off anchors remain explicit, deterministic, and
traceable across dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6651` defines canonical lane-C integration closeout and gate sign-off scope.
- Dependency token: `M245-C015`.
- Upstream C015 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_advanced_core_workpack_shard1_c015_expectations.md`
  - `spec/planning/compiler/m245/m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_packet.md`
  - `scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
- C016 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c016_lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_packet.md`
  - `scripts/check_m245_c016_lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_contract.py`
  - `tests/tooling/test_check_m245_c016_lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C016 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and M245-C015 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c016_lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_contract.py`
- `python scripts/check_m245_c016_lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c016_lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C016/lowering_ir_portability_contracts_integration_closeout_and_gate_signoff_contract_summary.json`
