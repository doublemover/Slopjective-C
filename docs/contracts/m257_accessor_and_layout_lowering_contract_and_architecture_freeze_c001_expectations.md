# M257 Accessor And Layout Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-executable-property-accessor-layout-lowering/m257-c001-v1`
Status: Accepted
Issue: `#7150`
Scope: M257 lane-C freeze of the current property/ivar lowering boundary that republishes sema-approved property metadata tables, synthesized binding identities, and ivar layout records into emitted IR/object artifacts without yet adding synthesized accessor execution or runtime storage realization.

## Objective

Freeze the accessor and layout lowering boundary that already exists in the
native IR/object path. The current compiler emits property and ivar descriptor
bundles, sema-approved synthesized binding identities, and deterministic ivar
layout symbols/slots/sizes/alignment. This issue makes that boundary explicit
so later implementation can extend it without moving AST/sema/runtime
responsibilities.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point
   for:
   - `objc3c-executable-property-accessor-layout-lowering/m257-c001-v1`
   - property-table model
     `property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records`
   - ivar-layout model
     `ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records`
   - accessor-binding model
     `effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis`
   - scope model
     `ast-sema-property-layout-handoff-ir-object-metadata-publication`
   - fail-closed model
     `no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation`.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic boundary
   summary for the current accessor/layout lowering surface.
3. `ast/objc3_ast.h` remains explicit that declaration-owned property
   attribute/accessor profiles, synthesized binding identities, and ivar layout
   identities originate upstream of lowering.
4. `sema/objc3_semantic_passes.cpp` remains explicit that sema owns
   property/accessor/layout compatibility and canonical synthesized
   binding/layout identities, but does not synthesize executable accessor
   bodies.
5. `ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR
   through `; executable_property_accessor_layout_lowering = ...` and carries
   the current property/ivar descriptor and layout counts.
6. Happy-path native emission must keep producing real object artifacts where
   property and ivar descriptor aggregates remain non-empty for property-heavy
   fixtures and the boundary line advertises the current lowering surface.

## Dynamic Coverage

1. Native compile probe over
   `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`
   proves the emitted IR carries the new accessor/layout boundary line, emits
   property and ivar descriptor aggregates, and produces a non-empty
   `module.obj`.
2. Native compile probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   proves the same boundary holds for mixed class/property/ivar metadata
   emission without widening the runtime surface.

## Non-Goals and Fail-Closed Rules

- `M257-C001` does not introduce synthesized accessor body emission.
- `M257-C001` does not introduce runtime storage allocation or instance layout
  realization.
- `M257-C001` does not reinterpret AST/sema legality inside IR/object
  emission.
- `M257-C001` does not add runtime property getter/setter execution semantics.
- If the accessor/layout lowering boundary drifts, later lane-C implementation
  must fail closed rather than silently widening the binding surface.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m257-c001-accessor-and-layout-lowering-contract`.
- `package.json` includes
  `test:tooling:m257-c001-accessor-and-layout-lowering-contract`.
- `package.json` includes `check:objc3c:m257-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m257_c001_accessor_and_layout_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m257_c001_accessor_and_layout_lowering_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m257-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m257/M257-C001/accessor_and_layout_lowering_contract_summary.json`
