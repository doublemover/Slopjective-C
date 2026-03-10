# M257-B003 Accessor Legality and Ownership or Atomicity Attribute Interactions Core Feature Expansion Packet

Packet: `M257-B003`
Milestone: `M257`
Lane: `B`
Issue: `#7149`
Contract ID: `objc3c-property-accessor-attribute-interactions/m257-b003-v1`
Dependencies: `M257-B001`, `M257-B002`
Next issue: `M257-B004`

## Summary

Expand live lane-B property legality so runtime-visible accessor and storage
semantics fail closed on duplicate effective accessor selectors, ownership
modifiers on non-object properties, and unsupported atomic ownership-aware
property combinations.

## Acceptance criteria

- Interface, category-interface, implementation, and protocol property containers reject duplicate effective getter selectors.
- Interface, category-interface, implementation, and protocol property containers reject duplicate effective setter selectors.
- Runtime-managed ownership modifiers fail closed on non-object properties.
- Atomic ownership-aware properties fail closed until executable accessor storage semantics land.
- Positive proof compiles through `artifacts/bin/objc3c-native.exe` on the `llvm-direct` backend.
- Evidence lands at `tmp/reports/m257/M257-B003/accessor_legality_attribute_interactions_summary.json`.

## Ownership boundary

- AST owns the canonical accessor-selector uniqueness and ownership or atomicity interaction model strings.
- Sema owns duplicate selector detection and unsupported attribute interaction diagnostics.
- IR owns proof publication only and must not reopen property accessor legality once sema has admitted the source.

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
  `tmp/reports/m257/M257-B003/accessor_legality_attribute_interactions_summary.json`

## Validation

- `python scripts/check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py -q`
- `npm run check:objc3c:m257-b003-lane-b-readiness`
