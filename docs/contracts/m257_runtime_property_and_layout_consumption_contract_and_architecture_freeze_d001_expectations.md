# M257 Runtime Property And Layout Consumption Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
Status: Accepted
Issue: `#7153`
Scope: M257 lane-D freeze that captures the current truthful runtime boundary above `M257-C003`: emitted accessor implementation pointers and property/layout attachment identities are consumed through the existing runtime lookup/dispatch ABI, repeated `alloc` / `new` still materialize one canonical realized instance identity per class, and synthesized accessor execution still uses the lane-C storage globals pending true per-instance slot allocation.

## Objective

Freeze the current runtime property/layout boundary so `M257-D002` extends one explicit, evidence-backed implementation surface rather than rediscovering allocator, property, or layout behavior from source or IR heuristics.

## Required Invariants

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
   - descriptor model `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
   - allocator model `alloc-new-return-one-canonical-realized-instance-identity-per-class-before-true-instance-slot-allocation`
   - storage model `synthesized-accessor-execution-uses-lane-c-storage-globals-pending-runtime-instance-slots`
   - fail-closed model `no-layout-rederivation-no-reflective-property-registration-no-per-instance-allocation-yet`
2. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic `Objc3RuntimePropertyLayoutConsumptionSummary()` surface.
3. `native/objc3c/src/ast/objc3_ast.h` remains explicit that synthesized binding and ivar-layout identities are parser/sema-owned facts that runtime must consume rather than rederive.
4. `native/objc3c/src/sema/objc3_semantic_passes.cpp` remains explicit that runtime consumes emitted selector/binding/layout facts instead of recovering allocator/storage behavior from source.
5. `native/objc3c/src/ir/objc3_ir_emitter.cpp` publishes `; runtime_property_layout_consumption = ...` adjacent to the existing `M257-C003` lowering summary.
6. `native/objc3c/src/runtime/objc3_runtime.h`, `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`, and `native/objc3c/src/runtime/objc3_runtime.cpp` keep the current proof surface stable while documenting the current single-instance allocation and lane-C storage-backed execution boundary.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3` proves the emitted artifact set contains:
   - `module.manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.object-backend.txt`
2. The emitted IR must publish:
   - `; executable_synthesized_accessor_property_lowering = ...`
   - `; runtime_property_layout_consumption = contract=objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
3. The runtime probe `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp` proves the current boundary:
   - repeated `alloc` over `Widget` returns the same canonical realized instance identity
   - writing `count`, `enabled`, and `value` through one allocated receiver is observable through a second allocated receiver
   - synthesized accessor cache entries resolve with the canonical owner identities and parameter counts
4. Runtime registration and selector-table snapshots remain observable through the existing testing surface and remain non-empty for the probe object.

## Non-Goals and Fail-Closed Rules

- `M257-D001` does not add true instance allocation.
- `M257-D001` does not add per-instance slot storage.
- `M257-D001` does not add reflective property registration.
- `M257-D001` does not permit layout re-derivation from source or ad hoc runtime heuristics.
- If emitted accessor bindings or attachment identities drift, the runtime boundary must fail closed instead of silently inventing replacement storage or rebinding behavior.

## Architecture and Spec Anchors

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/runtime/README.md`
- `tests/tooling/runtime/README.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m257-d001-runtime-property-and-layout-consumption-contract`.
- `package.json` includes `test:tooling:m257-d001-runtime-property-and-layout-consumption-contract`.
- `package.json` includes `check:objc3c:m257-d001-lane-d-readiness`.

## Validation

- `python scripts/check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m257-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m257/M257-D001/property_layout_runtime_contract_summary.json`
