# M246-B002 Semantic Invariants for Optimization Legality Modular Split/Scaffolding Packet

Packet: `M246-B002`
Milestone: `M246`
Lane: `B`
Issue: `#5061`
Freeze date: `2026-03-03`
Dependencies: `M246-B001`

## Purpose

Freeze lane-B semantic invariants for optimization legality modular split/scaffolding prerequisites for M246 so optimizer pipeline integration and invariants governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m246_semantic_invariants_for_optimization_legality_modular_split_scaffolding_b002_expectations.md`
- Checker:
  `scripts/check_m246_b002_semantic_invariants_for_optimization_legality_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_b002_semantic_invariants_for_optimization_legality_modular_split_scaffolding_contract.py`
- Dependency anchors from `M246-B001`:
  - `docs/contracts/m246_semantic_invariants_for_optimization_legality_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m246/m246_b001_semantic_invariants_for_optimization_legality_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_b001_semantic_invariants_for_optimization_legality_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_b001_semantic_invariants_for_optimization_legality_contract_and_architecture_freeze_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m246-b002-semantic-invariants-optimization-legality-modular-split-scaffolding-contract`
  - `test:tooling:m246-b002-semantic-invariants-optimization-legality-modular-split-scaffolding-contract`
  - `check:objc3c:m246-b002-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m246_b002_semantic_invariants_for_optimization_legality_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m246_b002_semantic_invariants_for_optimization_legality_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m246-b002-lane-b-readiness`

## Evidence Output

- `tmp/reports/m246/M246-B002/semantic_invariants_optimization_legality_modular_split_scaffolding_summary.json`
