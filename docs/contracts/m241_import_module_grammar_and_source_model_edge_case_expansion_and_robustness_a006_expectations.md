# M241 Qualifier/Generic Grammar Normalization Contract and Architecture Freeze Expectations (A006)

Contract ID: `objc3c-import-module-grammar-and-source-model/m241-a006-v1`
Status: Accepted
Scope: M241 lane-A qualifier/generic grammar normalization edge-case expansion and robustness for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-A qualifier/generic grammar normalization anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6235` defines canonical lane-A contract-freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m241/m241_a006_import_module_grammar_and_source_model_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m241_a006_import_module_grammar_and_source_model_contract.py`
  - `tests/tooling/test_check_m241_a006_import_module_grammar_and_source_model_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M241 lane-A A006 qualifier/generic grammar normalization fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A qualifier/generic grammar normalization fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A qualifier/generic grammar normalization metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m241-a006-import-module-grammar-and-source-model-contract`.
- `package.json` includes `test:tooling:m241-a006-import-module-grammar-and-source-model-contract`.
- `package.json` includes `check:objc3c:m241-a006-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m241_a006_import_module_grammar_and_source_model_contract.py`
- `python -m pytest tests/tooling/test_check_m241_a006_import_module_grammar_and_source_model_contract.py -q`
- `npm run check:objc3c:m241-a006-lane-a-readiness`

## Evidence Path

- `tmp/reports/m241/M241-A006/import_module_grammar_and_source_model_contract_summary.json`






