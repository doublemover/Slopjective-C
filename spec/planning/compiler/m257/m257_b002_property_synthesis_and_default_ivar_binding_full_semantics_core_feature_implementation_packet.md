# M257-B002 Property Synthesis and Default Ivar Binding Full Semantics Core Feature Implementation Packet

Packet: `M257-B002`
Milestone: `M257`
Lane: `B`
Issue: `#7148`
Contract ID: `objc3c-property-default-ivar-binding-semantics/m257-b002-v1`
Dependencies: `M257-B001`, `M257-A002`
Next issue: `M257-B003`

## Summary

Implement live lane-B property synthesis semantics so matched class
implementations consume interface-declared properties as authoritative default
ivar binding sites while treating implementation redeclarations as optional,
compatibility-checked overlays.

## Acceptance criteria

- Matched class implementations synthesize from interface-declared properties even when the implementation does not redeclare the property.
- Optional implementation redeclarations are permitted only when both the property signature and the resolved default ivar binding remain compatible with the interface-owned property.
- Category implementations remain outside default ivar synthesis.
- Manifest and semantic-surface publication distinguish interface-owned synthesis sites from implementation redeclaration sites.
- Positive proof compiles through `artifacts/bin/objc3c-native.exe` on the `llvm-direct` backend.
- Evidence lands at `tmp/reports/m257/M257-B002/property_synthesis_default_ivar_binding_full_semantics_summary.json`.

## Ownership boundary

- AST owns the canonical default-binding resolution model string.
- Sema owns authoritative synthesis-site selection plus binding compatibility.
- IR owns proof publication only and must not rebuild synthesis sites from implementation-local redeclarations.

## Required anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Evidence

- Canonical summary path:
  `tmp/reports/m257/M257-B002/property_synthesis_default_ivar_binding_full_semantics_summary.json`

## Validation

- `python scripts/check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m257-b002-lane-b-readiness`
