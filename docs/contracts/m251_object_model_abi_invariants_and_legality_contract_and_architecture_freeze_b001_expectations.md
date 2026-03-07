# M251 Object-Model ABI Invariants and Legality Expectations (B001)

Contract ID: `objc3c-runtime-export-legality-freeze/m251-b001-v1`
Status: Accepted
Scope: M251 lane-B contract and architecture freeze for semantic legality of runtime-exported Objective-C declarations.

## Objective

Freeze the semantic boundary that decides whether class, protocol, category,
property, ivar, and method declarations are complete enough, deterministic
enough, and shape-stable enough to be exported into native runtime metadata.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines
   `Objc3RuntimeExportLegalityBoundary` as the canonical lane-B freeze packet.
2. `pipeline/objc3_frontend_pipeline.cpp` synthesizes the legality packet from:
   - runtime metadata source ownership readiness,
   - protocol/category export summary,
   - class/protocol/category linking summary,
   - selector normalization summary,
   - property attribute summary,
   - object-pointer/nullability/generics summary,
   - symbol-graph/scope-resolution summary,
   - property-synthesis/ivar-binding semantic summary.
3. The legality packet remains fail-closed while later metadata-export
   enforcement work is still pending.
4. `pipeline/objc3_frontend_artifacts.cpp` publishes legality evidence into the
   manifest and forwards it into IR frontend metadata.
5. `ir/objc3_ir_emitter.h` / `ir/objc3_ir_emitter.cpp` preserve the legality
   metadata node in emitted LLVM IR.
6. `driver/objc3_objc3_path.cpp` continues to preserve manifest emission before
   later fail-closed exits.
7. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicitly test-only
   evidence and not the native runtime implementation.

## Non-Goals and Pending Enforcement

- `M251-B001` does not yet reject duplicate runtime identities.
- `M251-B001` does not yet block incomplete declaration export.
- `M251-B001` does not yet reject illegal redeclaration mixes.
- `M251-B001` does not yet wire runtime export legality into registration or
  native object-model execution.
- The freeze packet must therefore record those enforcement lanes as pending
  while still being ready as a canonical boundary for `M251-B002`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m251-b001-object-model-abi-invariants-and-legality-contract`.
- `package.json` includes
  `test:tooling:m251-b001-object-model-abi-invariants-and-legality-contract`.
- `package.json` includes `check:objc3c:m251-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m251_b001_object_model_abi_invariants_and_legality_contract.py`
- `python -m pytest tests/tooling/test_check_m251_b001_object_model_abi_invariants_and_legality_contract.py -q`
- `npm run check:objc3c:m251-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m251/M251-B001/object_model_abi_invariants_and_legality_contract_summary.json`
