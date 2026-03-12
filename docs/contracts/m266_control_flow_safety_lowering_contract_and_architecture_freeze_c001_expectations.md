# M266-C001 Expectations

## Scope

Freeze the truthful native-lowering boundary for the admitted Part 5 control-flow surface.

The compiler must now publish one lowering-owned packet at `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract`.

## Truthful boundary

- `guard` remains admitted in frontend/sema and fail-closed in native LLVM lowering.
- statement-form `match` remains admitted in frontend/sema and fail-closed in native LLVM lowering.
- source-only `defer { ... }` remains admitted in frontend/sema and fail-closed for runnable native lowering.
- current fail-closed lowering for admitted `guard`/`match` probes must terminate with deterministic `O3L300` diagnostics.
- current runnable `defer` probe must terminate with deterministic `O3S221` because the source-only defer surface is not yet runnable.

## Packet requirements

The lowering packet must:

- use contract id `objc3c-part5-control-flow-safety-lowering/m266-c001-v1`
- use surface path `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract`
- point back to semantic contract `objc3c-part5-control-flow-semantic-model/m266-b001-v1`
- publish explicit guard, match, defer, authority, and fail-closed models
- publish deterministic replay data
- keep all live lowering counters at `0`
- publish fail-closed counters that exactly cover the currently admitted source/sema surface

## Dynamic proof

The issue-local checker must prove all of the following:

- a source-only guard+match probe emits the lowering packet in `module.manifest.json`
- a source-only defer probe emits the lowering packet in `module.manifest.json`
- the guard+match runnable probe fails with `O3L300`
- the guard-only runnable probe fails with `O3L300`
- the defer runnable probe fails with `O3S221`

## Required anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`
