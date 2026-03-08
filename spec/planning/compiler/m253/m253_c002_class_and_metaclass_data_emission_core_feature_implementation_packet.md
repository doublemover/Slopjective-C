# M253-C002 Class And Metaclass Data Emission Core Feature Implementation Packet

Packet: `M253-C002`
Milestone: `M253`
Wave: `W45`
Lane: `C`
Issue: `#7091`
Contract ID: `objc3c-runtime-class-metaclass-data-emission/m253-c002-v1`
Dependencies:
- `M253-C001`
- `M252-C002`
- `M253-B002`

## Objective

Emit the first real executable metadata payload family into object files by replacing class-family placeholders with declaration-owner-ordered class/metaclass descriptor bundles.

## Canonical Emission Boundary

- contract id `objc3c-runtime-class-metaclass-data-emission/m253-c002-v1`
- payload model `class-source-record-descriptor-bundles-with-inline-metaclass-records`
- name model `shared-class-name-cstring-per-bundle`
- super-link model `nullable-super-source-record-bundle-pointer`
- method-list reference model `count-plus-owner-identity-pointer-method-list-ref`
- emitted IR comment `; runtime_metadata_class_metaclass_emission = ...`
- emitted LLVM metadata node `!objc3.objc_runtime_class_metaclass_emission`

## Acceptance Criteria

- Add explicit C002 boundary constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Publish the deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` responsible for:
  - metadata-only IR admission on ready metadata fixtures,
  - declaration-owner-ordered bundle construction,
  - fail-closed superclass owner resolution.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` emit real class/metaclass bundle globals in `objc3.runtime.class_descriptors` rather than class-family placeholder bytes.
- Keep `native/objc3c/src/io/objc3_process.cpp` explicit that llvm-direct object emission preserves the inline bundle payloads.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` must emit both `module.ll` and `module.obj` with the `llvm-direct` backend.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` proving:
   - empty diagnostics,
   - `module.object-backend.txt == llvm-direct`,
   - typed-handoff/source-graph/export/scaffold continuity,
   - IR bundle count `4`, instance-method refs `4`, class-method refs `4`,
   - four real `@__objc3_meta_class_####` globals and four owner-identity strings,
   - class aggregate `{ i64, [4 x ptr] }`,
   - no class-family `[1 x i8] zeroinitializer` placeholder globals,
   - `llvm-readobj --sections module.obj` exposes `objc3.runtime.class_descriptors` with nontrivial bytes/relocations,
   - `llvm-objdump --syms module.obj` exposes `objc3.runtime.class_descriptors` and `__objc3_sec_class_descriptors`.

## Non-Goals

- `M253-C002` does not add standalone metaclass sections.
- `M253-C002` does not add selector/string-pool payloads.
- `M253-C002` does not add standalone method/property/ivar list payload sections.
- `M253-C002` does not add runtime registration/bootstrap.

## Validation Commands

- `python scripts/check_m253_c002_class_and_metaclass_data_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_c002_class_and_metaclass_data_emission_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m253/M253-C002/class_and_metaclass_data_emission_summary.json`
