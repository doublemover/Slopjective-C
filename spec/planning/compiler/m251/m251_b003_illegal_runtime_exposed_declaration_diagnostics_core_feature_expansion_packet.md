# M251-B003 Illegal Runtime-Exposed Declaration Diagnostics Packet

Packet: `M251-B003`
Milestone: `M251`
Lane: `B`
Issue: `#7061`
Dependencies: `M251-B002`
Contract ID: `objc3c-runtime-export-diagnostics/m251-b003-v1`

## Goal

Expand the fail-closed runtime export diagnostic surface so declarations that
parse successfully but still cannot participate in runtime export produce
precise, deterministic, source-anchored reasons.

## Scope

- Preserve the B002 runtime export enforcement packet and happy path.
- Add precise incomplete-interface runtime export diagnostics for class
  interfaces missing an `@implementation`.
- Add precise incomplete-category runtime export diagnostics for category
  interfaces missing an `@implementation`.
- Keep the generic B002 blocker as the fallback for non-declaration-specific
  runtime export failures.
- Extend deterministic checker/test coverage so regressions fail closed.

## Deterministic Probe Matrix

The checker runs three deterministic probes:

1. Manifest-only happy-path class fixture proving B002 readiness is preserved.
2. Interface-only negative fixture proving the diagnostic names the missing
   class implementation.
3. Category-interface-only negative fixture proving the diagnostic names the
   missing category implementation.

## Code Anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`

## Spec Anchors

- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `docs/objc3c-native.md`

## Implementation Notes

- B003 deliberately reuses `O3S260` so the B002 readiness chain remains stable.
- Precision is added in the diagnostic message and source anchor rather than by
  changing the B002 enforcement packet shape.
- Category semantic ownership refactors remain future work; B003 only expands
  the exported-declaration diagnostic surface.

## Evidence

- `tmp/reports/m251/M251-B003/illegal_runtime_exposed_declaration_diagnostics_summary.json`
