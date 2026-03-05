# M235-A017 Qualifier/Generic Grammar Normalization Integration Closeout and Gate Sign-Off Packet

Packet: `M235-A017`
Milestone: `M235`
Lane: `A`
Freeze date: `2026-03-05`
Issue: `#5780`
Dependencies: `M235-A016`

## Purpose

Freeze lane-A qualifier/generic grammar normalization integration closeout
and gate sign-off prerequisites so `M235-A016` dependency continuity stays
explicit, deterministic, and fail-closed, including code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_a017_expectations.md`
- Checker:
  `scripts/check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
- Readiness runner:
  `scripts/run_m235_a017_lane_a_readiness.py`
- Dependency anchors from `M235-A016`:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_a016_expectations.md`
  - `spec/planning/compiler/m235/m235_a016_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m235_a016_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_a016_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `scripts/run_m235_a016_lane_a_readiness.py`
- Canonical readiness command names:
  - `check:objc3c:m235-a017-qualifier-and-generic-grammar-normalization-integration-closeout-and-gate-signoff-contract`
  - `test:tooling:m235-a017-qualifier-and-generic-grammar-normalization-integration-closeout-and-gate-signoff-contract`
  - `check:objc3c:m235-a017-lane-a-readiness`
  - `check:objc3c:m235-a016-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `test:objc3c:perf-budget`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py -q`
- `python scripts/run_m235_a017_lane_a_readiness.py`
- `npm run check:objc3c:m235-a017-lane-a-readiness`

## Evidence Output

- `tmp/reports/m235/M235-A017/qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract_summary.json`
