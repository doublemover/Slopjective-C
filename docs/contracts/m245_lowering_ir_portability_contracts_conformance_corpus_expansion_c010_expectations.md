# M245 Lowering/IR Portability Contracts Conformance Corpus Expansion Expectations (C010)

Contract ID: `objc3c-lowering-ir-portability-contracts-conformance-corpus-expansion/m245-c010-v1`
Status: Accepted
Dependencies: `M245-C009`
Scope: M245 lane-C lowering/IR portability contracts conformance corpus expansion continuity with explicit `M245-C009` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts conformance corpus
expansion anchors remain explicit, deterministic, and traceable across
dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6645` defines canonical lane-C conformance corpus expansion scope.
- Dependency token: `M245-C009`.
- Upstream C009 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_conformance_matrix_implementation_c009_expectations.md`
  - `spec/planning/compiler/m245/m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_packet.md`
  - `scripts/check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`
- C010 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_packet.md`
  - `scripts/check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C010 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and C009 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py`
- `python scripts/check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c010_lowering_ir_portability_contracts_conformance_corpus_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C010/lowering_ir_portability_contracts_conformance_corpus_expansion_summary.json`
