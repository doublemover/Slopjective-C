# M257-C001 Accessor And Layout Lowering Contract And Architecture Freeze Packet

Packet: `M257-C001`
Milestone: `M257`
Wave: `W49`
Lane: `C`
Issue: `#7150`
Contract ID: `objc3c-executable-property-accessor-layout-lowering/m257-c001-v1`
Dependencies: None

## Objective

Freeze the current property/accessor/layout lowering boundary that binds
sema-approved property metadata tables, synthesized binding identities, and
ivar layout records into emitted metadata artifacts.

## Canonical Accessor/Layout Boundary

- contract id `objc3c-executable-property-accessor-layout-lowering/m257-c001-v1`
- property-table model
  `property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records`
- ivar-layout model
  `ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records`
- accessor-binding model
  `effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis`
- scope model
  `ast-sema-property-layout-handoff-ir-object-metadata-publication`
- fail-closed model
  `no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation`
- emitted IR comment `; executable_property_accessor_layout_lowering = ...`

## Acceptance Criteria

- Add explicit accessor/layout boundary constants in
  `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add a deterministic boundary summary helper in
  `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/ast/objc3_ast.h` explicit that AST owns declaration
  property/accessor/layout identities.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema
  owns property/accessor/layout compatibility and canonical synthesized
  binding/layout identities.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish the accessor/layout
  boundary directly before emitted metadata artifacts are written.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over property-heavy and mixed metadata fixtures
  must still emit non-empty `module.obj` artifacts.

## Dynamic Probes

1. Native compile probe over
   `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`
   (`m257_property_ivar_source_model_completion_positive.objc3`) proving
   emitted IR/object output carries:
   - `; executable_property_accessor_layout_lowering = ...`
   - property descriptor aggregate retention
   - ivar descriptor aggregate retention
   - synchronized attribute/accessor/synthesized-binding/layout entry counts
   - successful `module.obj` emission.
2. Native compile probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   (`m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`)
   proving the same boundary for mixed class/property/ivar metadata emission.

## Non-Goals

- `M257-C001` does not add synthesized accessor body emission.
- `M257-C001` does not add runtime storage allocation.
- `M257-C001` does not add instance layout realization.
- `M257-C001` does not reinterpret parser/AST/sema legality.

## Validation Commands

- `python scripts/check_m257_c001_accessor_and_layout_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m257_c001_accessor_and_layout_lowering_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m257-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m257/M257-C001/accessor_and_layout_lowering_contract_summary.json`
