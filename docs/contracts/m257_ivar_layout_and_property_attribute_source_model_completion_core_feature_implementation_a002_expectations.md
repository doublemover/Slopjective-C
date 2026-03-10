# M257 Ivar Layout And Property Attribute Source-Model Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-executable-property-ivar-source-model-completion/m257-a002-v1`

## Objective

Broaden the `M257-A001` freeze into a real compiler capability that publishes
completed property attribute, accessor ownership, synthesized binding, and ivar
layout source-model data before runtime storage realization starts.

## Required implementation

1. Add a canonical expectations document for the completed property/ivar source
   model.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py`
   - `tests/tooling/test_check_m257_a002_ivar_layout_and_property_attribute_source_model_completion_core_feature_implementation.py`
   - `scripts/run_m257_a002_lane_a_readiness.py`
3. Add `M257-A002` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Complete the source-model handoff around:
   - `Objc3PropertyDecl.property_attribute_profile`
   - `Objc3PropertyDecl.effective_getter_selector`
   - `Objc3PropertyDecl.effective_setter_available`
   - `Objc3PropertyDecl.effective_setter_selector`
   - `Objc3PropertyDecl.accessor_ownership_profile`
   - `Objc3PropertyDecl.executable_synthesized_binding_kind`
   - `Objc3PropertyDecl.executable_synthesized_binding_symbol`
   - `Objc3PropertyDecl.executable_ivar_layout_symbol`
   - `Objc3PropertyDecl.executable_ivar_layout_slot_index`
   - `Objc3PropertyDecl.executable_ivar_layout_size_bytes`
   - `Objc3PropertyDecl.executable_ivar_layout_alignment_bytes`
   - `frontend.lowering.executable_property_ivar_source_model_replay_key`
5. The checker must prove the capability with one live compile of:
   - `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`
   It must fail closed unless the manifest property/ivar records, ownership
   profiles, layout data, and emitted IR replay key remain synchronized.
6. `package.json` must wire:
   - `check:objc3c:m257-a002-ivar-layout-and-property-attribute-source-model-completion`
   - `test:tooling:m257-a002-ivar-layout-and-property-attribute-source-model-completion`
   - `check:objc3c:m257-a002-lane-a-readiness`
7. The contract must explicitly hand off to `M257-B001`.

## Canonical models

- Layout model:
  `property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization`
- Attribute model:
  `property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles`
- Evidence model:
  `property-layout-fixture-manifest-and-ir-replay-key`
- Failure model:
  `fail-closed-on-property-attribute-accessor-ownership-or-layout-drift-before-storage-realization`

## Non-goals

- No synthesized accessor body emission yet.
- No runtime storage allocation yet.
- No instance layout realization in the runtime library yet.
- No runtime property execution semantics yet.

## Evidence

- `tmp/reports/m257/M257-A002/property_ivar_source_model_completion_summary.json`
