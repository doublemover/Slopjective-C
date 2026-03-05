# M235 Qualifier/Generic Grammar Normalization Integration Closeout and Gate Sign-Off Expectations (A017)

Contract ID: `objc3c-qualifier-and-generic-grammar-normalization-integration-closeout-and-gate-signoff/m235-a017-v1`
Status: Accepted
Dependencies: `M235-A016`
Scope: M235 lane-A qualifier/generic grammar normalization integration closeout and gate sign-off dependency continuity and fail-closed readiness governance.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization
integration closeout and gate sign-off dependency anchors remain explicit,
deterministic, and traceable across dependency surfaces, including code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5780` defines canonical lane-A integration closeout and gate sign-off scope.
- `M235-A016` advanced edge compatibility workpack (shard 1) anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_a016_expectations.md`
  - `spec/planning/compiler/m235/m235_a016_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m235_a016_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_a016_qualifier_and_generic_grammar_normalization_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `scripts/run_m235_a016_lane_a_readiness.py`
- Packet/checker/test assets for A017 remain mandatory:
  - `spec/planning/compiler/m235/m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_packet.md`
  - `scripts/check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
  - `tests/tooling/test_check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
  - `scripts/run_m235_a017_lane_a_readiness.py`

## Deterministic Invariants

1. Lane-A integration closeout and gate sign-off dependency references
   remain explicit and fail closed when dependency tokens drift.
2. integration-closeout-and-gate-signoff command sequencing and
   integration-closeout-and-gate-signoff-key continuity remain
   deterministic and fail-closed across lane-A readiness wiring.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- Canonical command names for this contract:
  - `check:objc3c:m235-a017-qualifier-and-generic-grammar-normalization-integration-closeout-and-gate-signoff-contract`
  - `test:tooling:m235-a017-qualifier-and-generic-grammar-normalization-integration-closeout-and-gate-signoff-contract`
  - `check:objc3c:m235-a017-lane-a-readiness`
- Lane-A readiness chaining expected by this contract remains deterministic and fail-closed:
  - `python scripts/run_m235_a016_lane_a_readiness.py`
  - `python scripts/check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
  - `python -m pytest tests/tooling/test_check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py -q`

## Milestone Optimization Inputs

- `compile:objc3c`
- `test:objc3c:perf-budget`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Validation

- `python scripts/check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m235_a017_qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract.py -q`
- `python scripts/run_m235_a017_lane_a_readiness.py`
- `npm run check:objc3c:m235-a017-lane-a-readiness`

## Evidence Path

- `tmp/reports/m235/M235-A017/qualifier_and_generic_grammar_normalization_integration_closeout_and_gate_signoff_contract_summary.json`
