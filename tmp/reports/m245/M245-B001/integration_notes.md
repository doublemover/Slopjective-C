# M245-B001 Integration Notes

## Expected `package.json` Script Snippets (contract-locked)

```json
"check:objc3c:m245-b001-semantic-parity-platform-constraints-contract": "python scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py"
"test:tooling:m245-b001-semantic-parity-platform-constraints-contract": "python -m pytest tests/tooling/test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py -q"
"check:objc3c:m245-b001-lane-b-readiness": "npm run check:objc3c:m245-b001-semantic-parity-platform-constraints-contract && npm run test:tooling:m245-b001-semantic-parity-platform-constraints-contract"
```

## Shared-Doc Anchors Expected by Checker

- `native/objc3c/src/ARCHITECTURE.md`
  - `` `pipeline/objc3_parse_lowering_readiness_surface.h` so compatibility handoff, ``
  - `typed sema handoff and parse/lowering readiness surfaces stay split while`
  - `typed semantic case accounting and parse conformance accounting remain`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `semantic/lowering test architecture governance shall preserve explicit lane-B`
  - `semantic compatibility and migration checks governance shall preserve explicit`
  - `lane-B compatibility-mode and migration-assist handoff anchors and fail`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `deterministic lane-B semantic/lowering metadata anchors for \`M248-B001\``
  - `deterministic lane-B semantic compatibility/migration metadata anchors for \`M249-B001\``
  - `with compile-route evidence and perf-budget continuity so platform`
  - `operation drift fails closed.`

## Validation Executed

- `python scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py`
- `python -m pytest tests/tooling/test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py -q`
