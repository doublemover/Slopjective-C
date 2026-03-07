# M251-B002 Metadata Completeness and Duplicate Suppression Semantics Packet

Packet: `M251-B002`
Milestone: `M251`
Lane: `B`
Issue: `#7060`
Dependencies: `M251-B001`, `M251-A002`
Contract ID: `objc3c-runtime-export-enforcement/m251-b002-v1`

## Goal

Implement real fail-closed runtime export enforcement semantics so the native
frontend only allows lowering/runtime export when runtime-owned Objective-C
metadata declarations are complete, non-duplicated, redeclaration-compatible,
and shape-stable.

## Scope

- Freeze `Objc3RuntimeExportEnforcementSummary` as the lane-B enforcement
  packet.
- Synthesize enforcement from runtime metadata source records plus the B001
  legality packet.
- Preserve the happy path for complete class/protocol/property/ivar fixtures.
- Reject duplicate runtime identities before lowering.
- Reject incomplete runtime export units before lowering.
- Reject illegal property/method redeclaration mixes before lowering.
- Reject metadata-shape drift before lowering.
- Publish enforcement fields into the manifest and LLVM IR metadata.

## Deterministic Probe Matrix

The checker runs five deterministic probes:

1. Manifest-only happy-path class fixture through the frontend C API runner.
2. Native-driver LLVM IR emission probe that preserves the B002 metadata node.
3. Duplicate-identity negative fixture that must fail with `O3S200`.
4. Incomplete-declaration negative fixture that must fail with `O3S260`.
5. Illegal property redeclaration negative fixture that must fail with `O3S206`.

## Code Anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`

## Spec Anchors

- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `docs/objc3c-native.md`

## Implementation Notes

- Forward `@protocol` declarations are not exportable runtime metadata units and
  must not be misclassified as incomplete export candidates.
- Property/method redeclaration classification must compare the runtime-visible
  interface/category interface record against the matching implementation record
  rather than relying only on coarse legality counters.
- The runtime shim remains test-only. B002 strengthens semantic gating without
  pretending that executable native runtime metadata registration already exists.

## Evidence

- `tmp/reports/m251/M251-B002/metadata_completeness_and_duplicate_suppression_semantics_summary.json`
