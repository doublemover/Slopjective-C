# M266-C002 Defer And Guard Lowering With Cleanup Insertion Core Feature Implementation Packet

Packet: `M266-C002`
Issue: `#7263`
Milestone: `M266`
Lane: `C`
Wave: `W59`

## Why This Issue Exists

`M266-C001` froze the truthful lowering boundary:

- pure `guard` syntax is admitted upstream
- statement-form `match` syntax is admitted upstream
- source-only `defer` legality is admitted upstream
- native lowering still fails closed for that admitted surface

`M266-C002` is the first lane-C implementation step that converts part of that frozen boundary into runnable native behavior.

## Implementation Goal

Make the native LLVM/object path truthfully runnable for:

- pure `guard` lowering
- `defer` lowering with lexical-scope cleanup insertion

Do not overclaim statement-form `match`. If `match` is still fail-closed after this issue, the implementation is still acceptable so long as the boundary packet says so explicitly.

## Required Implementation Shape

1. Introduce one live lowering packet at:
   - `frontend.pipeline.semantic_surface.objc_part5_defer_guard_lowering_implementation`
2. Use contract id:
   - `objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1`
3. Publish explicit upstream dependencies:
   - `objc3c-part5-control-flow-safety-lowering/m266-c001-v1`
   - `objc3c-part5-control-flow-semantic-model/m266-b001-v1`
4. Publish explicit live/deferred scope in the packet:
   - pure `guard` lowering is live
   - runnable `defer` cleanup insertion is live
   - statement-form `match` may remain deferred
5. Emit a boundary comment and named metadata in the generated IR so the implementation is inspectable without relying on fragile block-label spelling.

## Truthful Acceptance Criteria

- A pure-`guard` native probe compiles successfully and emits the C002 lowering packet.
- An ordinary-exit `defer` native probe compiles successfully and emits the C002 lowering packet.
- An early-return `defer` native probe compiles successfully and emits the C002 lowering packet.
- The packet proves live cleanup insertion counts for `defer` and live short-circuit lowering counts for pure `guard`.
- The packet keeps `match` explicitly outside scope unless native `match` lowering has independently landed.
- Evidence is written under `tmp/reports/m266/M266-C002/`.

## Recommended Checker Strategy

- Generate temporary native probes under `tmp/` rather than adding committed fixtures in this bundle-only step.
- Require the new lowering packet in successful manifests.
- Require emitted `module.ll` and `module.obj` on successful probes.
- Record mixed `guard + match` behavior in the summary, but do not gate C002 on runnable `match`.

## Required Anchor Targets For The Future Implementation

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Validation Commands

- `python scripts/check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py -q`
- `python scripts/run_m266_c002_lane_c_readiness.py`

## Evidence Output

- `tmp/reports/m266/M266-C002/defer_and_guard_lowering_with_cleanup_insertion_summary.json`
