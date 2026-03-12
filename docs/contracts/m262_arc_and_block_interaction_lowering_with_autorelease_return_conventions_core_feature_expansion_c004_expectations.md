# M262 ARC And Block-Interaction Lowering With Autorelease-Return Conventions Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-arc-block-autorelease-return-lowering/m262-c004-v1`

## Required Proof Surface

- Lane C publishes one canonical lowering contract for the supported escaping
  block plus autoreleasing-return edge inventory.
- Emitted IR publishes the boundary comment
  `; arc_block_autorelease_return_lowering = ...`.
- Emitted IR publishes named metadata
  `!objc3.objc_arc_block_autorelease_return_lowering = !{...}`.
- Supported lowering now materially proves:
  - escaping block promotion remains live under `-fobjc-arc`
  - terminal branch cleanup does not consume sibling-branch ARC cleanup state
    during code generation
  - autoreleasing returns still emit `objc3_runtime_autorelease_i32` and the
    required release/dispose cleanup after block interaction on both branch
    paths
- Validation evidence lands at
  `tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json`.

## Required Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Dynamic Proof Cases

- `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_block_autorelease_return_positive.objc3`
