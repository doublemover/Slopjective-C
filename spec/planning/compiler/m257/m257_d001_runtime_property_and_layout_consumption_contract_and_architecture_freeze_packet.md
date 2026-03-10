# M257-D001 Runtime Property And Layout Consumption Contract And Architecture Freeze Packet

Packet: `M257-D001`
Milestone: `M257`
Wave: `W49`
Lane: `D`
Issue: `#7153`
Contract ID: `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
Dependencies: `M257-C003`

## Objective

Freeze the current runtime-owned property/layout boundary above `M257-C003` so later runtime allocation work extends one explicit surface instead of reinterpreting emitted metadata or source declarations out of band.

## Canonical Runtime Surface

- contract id `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
- descriptor model `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
- allocator model `alloc-new-return-one-canonical-realized-instance-identity-per-class-before-true-instance-slot-allocation`
- storage model `synthesized-accessor-execution-uses-lane-c-storage-globals-pending-runtime-instance-slots`
- fail-closed model `no-layout-rederivation-no-reflective-property-registration-no-per-instance-allocation-yet`
- emitted IR comment `; runtime_property_layout_consumption = ...`
- proof source `tests/tooling/runtime/m257_d001_property_layout_runtime_probe.cpp`

## Acceptance Criteria

- Add explicit D001 constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add one deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/ast/objc3_ast.h` explicit that synthesized binding/layout identities remain parser/sema-owned facts.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that runtime consumes emitted selector/binding/layout facts instead of rederiving them from source.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish `; runtime_property_layout_consumption = ...` adjacent to the `M257-C003` lowering summary.
- Keep `native/objc3c/src/runtime/objc3_runtime.h` and `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h` as the stable public/private proof surface for the current property/layout runtime boundary.
- Update `native/objc3c/src/runtime/objc3_runtime.cpp` documentation anchors so the current single-instance allocation and lane-C storage-backed execution boundary is explicit and deterministic.
- Add deterministic docs/spec/package/checker/test evidence under `tmp/reports/m257/M257-D001/`.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m257_synthesized_accessor_property_lowering_positive.objc3` proving the emitted artifact set contains:
   - `module.manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.object-backend.txt`
2. The emitted IR must publish:
   - `; executable_synthesized_accessor_property_lowering = ...`
   - `; runtime_property_layout_consumption = contract=objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
3. Runtime probe execution proves:
   - the first and second `alloc` results are identical for `Widget`
   - writes through `setCount:`, `setEnabled:`, and `setValue:` are observable through the second allocated receiver
   - synthesized getter/setter cache entries resolve with the canonical owner identities and parameter counts
4. Runtime registration and selector-table snapshots remain non-empty and observable through the existing private testing API.

## Non-Goals

- `M257-D001` does not add true instance allocation.
- `M257-D001` does not add per-instance slot storage.
- `M257-D001` does not add reflective property registration.
- `M257-D001` does not widen the public runtime ABI.

## Validation

- `python scripts/check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m257_d001_runtime_property_and_layout_consumption_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m257-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m257/M257-D001/property_layout_runtime_contract_summary.json`
