# M235 Qualifier/Generic Semantic Inference Modular Split/Scaffolding Expectations (B002)

Contract ID: `objc3c-qualifier-and-generic-semantic-inference-modular-split-scaffolding/m235-b002-v1`
Status: Accepted
Scope: M235 lane-B modular split/scaffolding continuity for qualifier/generic semantic inference dependency wiring.

## Objective

Fail closed unless lane-B modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5782` defines canonical lane-B modular split/scaffolding scope.
- Dependencies: `M235-B001`
- M235-B001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualifier_and_generic_semantic_inference_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m235/m235_b001_qualifier_and_generic_semantic_inference_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_b001_qualifier_and_generic_semantic_inference_contract.py`
  - `tests/tooling/test_check_m235_b001_qualifier_and_generic_semantic_inference_contract.py`
- Packet/checker/test assets for B002 remain mandatory:
  - `spec/planning/compiler/m235/m235_b002_qualifier_and_generic_semantic_inference_modular_split_scaffolding_packet.md`
  - `scripts/check_m235_b002_qualifier_and_generic_semantic_inference_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m235_b002_qualifier_and_generic_semantic_inference_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M235 lane-B B002 qualifier/generic semantic inference modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B qualifier/generic semantic inference modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B qualifier/generic semantic inference modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m235-b002-qualifier-and-generic-semantic-inference-modular-split-scaffolding-contract`.
- `package.json` includes `test:tooling:m235-b002-qualifier-and-generic-semantic-inference-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m235-b002-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m235_b002_qualifier_and_generic_semantic_inference_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m235_b002_qualifier_and_generic_semantic_inference_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m235-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m235/M235-B002/qualifier_and_generic_semantic_inference_modular_split_scaffolding_summary.json`

