# M262-C001 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Packet

Packet: `M262-C001`
Milestone: `M262`
Wave: `W54`
Lane: `C`
Issue: `#7199`
Contract ID: `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
Dependencies: None

## Objective

Freeze the current ARC lowering ABI/cleanup boundary before lane-C implementation begins inserting new ARC helper calls or cleanup scopes.

## Canonical ARC Lowering Boundary

- contract id `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
- source model `arc-semantic-packets-plus-unwind-cleanup-summary-form-the-current-lowering-boundary`
- ABI model `owned-value-lowering-targets-private-runtime-retain-release-autorelease-and-weak-helper-entrypoints-without-public-runtime-abi-expansion`
- cleanup model `cleanup-scheduling-remains-explicit-summary-and-helper-boundary-only-until-m262-c002`
- fail-closed model `no-implicit-cleanup-scope-insertion-no-helper-rebinding-no-public-runtime-abi-widening-before-later-lane-c-runtime-work`
- emitted IR comment `; arc_lowering_abi_cleanup_model = ...`

## Acceptance Criteria

- Add explicit ARC lowering ABI/cleanup constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add one deterministic boundary summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema owns semantic ARC packets only.
- Keep `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` explicit that the handoff does not claim cleanup scheduling or helper placement.
- Keep `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` explicit that the scaffold is not itself a lowering schedule.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish the boundary directly into emitted IR.
- Add deterministic docs/spec/package/checker/test evidence.
- Native compile probes over property-interaction and autorelease-return ARC fixtures must keep emitting non-empty `module.ll`/`module.obj` artifacts with the new boundary line.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3` proving emitted IR/object output carries:
   - `; arc_lowering_abi_cleanup_model = ...`
   - the current private retain/release helper boundary
   - the current weak helper boundary.
2. Native compile probe over `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3` proving the same boundary for autorelease helper publication.

## Non-Goals

- `M262-C001` does not add automatic retain/release/autorelease insertion.
- `M262-C001` does not add general cleanup-scope emission.
- `M262-C001` does not add generalized weak load/store lowering.
- `M262-C001` does not widen private helper entrypoints into a public runtime ABI.
- `M262-C002` is the explicit next handoff after this freeze closes.

## Validation Commands

- `python scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m262-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m262/M262-C001/arc_lowering_abi_cleanup_model_summary.json`
