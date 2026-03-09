# M256 Object Model Semantic Rules Contract and Architecture Freeze Expectations (B001)

Contract ID: `objc3c-object-model-semantic-rules/m256-b001-v1`
Status: Accepted
Issue: `#7132`
Scope: `M256` lane-B contract and architecture freeze for executable object-model
semantic rules.

## Objective

Freeze the semantic-rule boundary that later `M256` lane-B implementation work
must enforce for runnable class/protocol/category behavior without reinterpreting
the already-frozen source graph.

## Required Invariants

1. `parse/objc3_parser.cpp` remains source-only for:
   - raw superclass spellings
   - protocol adoption spellings
   - canonical category owner identities
2. `sema/objc3_semantic_passes.cpp` remains authoritative for:
   - realization legality
   - inheritance legality
   - override compatibility
   - declared protocol conformance policy
   - deterministic category merge policy
3. `ir/objc3_ir_emitter.cpp` remains proof-only for this boundary and does not
   claim executable enforcement yet.
4. The frozen semantic models are:
   - `interface-plus-implementation-pair-required-before-runtime-realization`
   - `single-superclass-no-cycles-rooted-in-source-closure-parent-identities`
   - `selector-kind-and-instance-class-ownership-must-remain-compatible-before-runtime-binding`
   - `declared-adoption-requires-required-member-coverage-optional-members-are-non-blocking`
   - `deterministic-declaration-order-with-fail-closed-conflict-detection-before-runtime-installation`
5. Validation evidence lands under
   `tmp/reports/m256/M256-B001/object_model_semantic_rules_contract_summary.json`.

## Non-Goals and Fail-Closed Rules

- `M256-B001` does not implement live realization enforcement.
- `M256-B001` does not implement override checking.
- `M256-B001` does not implement protocol member enforcement.
- `M256-B001` does not implement category merge execution.
- `M256-B001` must fail closed on drift before `M256-B002` begins semantic
  enforcement work.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m256-b001-object-model-semantic-rules-contract-and-architecture-freeze`.
- `package.json` includes
  `test:tooling:m256-b001-object-model-semantic-rules-contract-and-architecture-freeze`.
- `package.json` includes `check:objc3c:m256-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m256_b001_object_model_semantic_rules_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m256-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m256/M256-B001/object_model_semantic_rules_contract_summary.json`
