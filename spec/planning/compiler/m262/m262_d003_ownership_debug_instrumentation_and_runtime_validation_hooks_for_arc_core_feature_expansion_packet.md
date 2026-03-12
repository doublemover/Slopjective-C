# M262-D003 Ownership Debug Instrumentation And Runtime Validation Hooks For ARC Core Feature Expansion Packet

Packet: `M262-D003`
Issue: `#7205`
Milestone: `M262`
Lane: `D`

## Objective

Extend the live ARC helper/runtime surface from `M262-D002` with deterministic
private debug snapshots and validation hooks for the supported runnable slice.

## Dependency Chain

- `M262-D002` supplies the live private ARC helper runtime support surface.
- `M262-C004` supplies the supported ARC/block/autorelease-return lowering
  slice that must remain truthful under the new debug hooks.
- `M260-D002` supplies the retained memory-management baseline underneath the
  ARC helper layer.

## Implementation Requirements

1. Add one explicit D003 ARC-debug contract to the lowering contract surface
   and emitted IR metadata.
2. Keep the debug surface private:
   - no public runtime header widening
   - no user-facing ownership tracing API
3. Add one private bootstrap-internal ARC debug snapshot type and one
   `copy_*_for_testing` entrypoint.
4. Publish deterministic counters for:
   - retain/release/autorelease helpers
   - autoreleasepool push/pop helpers
   - current-property read/write/exchange helpers
   - weak current-property load/store helpers
5. Publish deterministic last-value and last property-context state.
6. Prove the surface with targeted ARC fixtures plus one dedicated runtime
   probe linked against `artifacts/lib/objc3_runtime.lib`.

## Dynamic Proof Model

### Property fixture

Compile `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
with `-fobjc-arc` and verify:

- native compile exits `0`
- `module.manifest.json`, `module.ll`, and `module.obj` exist
- emitted IR publishes the D003 boundary line and named metadata
- emitted IR still carries the supported ARC helper calls for current-property
  and weak current-property paths

### Block/autorelease-return fixture

Compile `tests/tooling/fixtures/native/m262_arc_block_autorelease_return_positive.objc3`
with `-fobjc-arc` and verify:

- native compile exits `0`
- `module.manifest.json`, `module.ll`, and `module.obj` exist
- emitted IR publishes the D003 boundary line and named metadata
- emitted IR still carries the supported autorelease helper traffic

### Runtime debug probe

Compile and run
`tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp` against
`artifacts/lib/objc3_runtime.lib` and verify:

- the probe links successfully
- the probe executes successfully
- ARC debug counters advance for the supported helper families
- helper last-value fields are deterministic
- the final weak getter path publishes `weakValue` as the last property name

## Non-goals

- no public ARC debug ABI
- no user-facing ownership tracing hooks
- no broader ARC runtime completeness claim beyond the supported runnable slice

## Required Outputs

- `docs/contracts/m262_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion_d003_expectations.md`
- `scripts/check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py`
- `scripts/run_m262_d003_lane_d_readiness.py`
- `tests/tooling/test_check_m262_d003_ownership_debug_instrumentation_and_runtime_validation_hooks_for_arc_core_feature_expansion.py`
- `tmp/reports/m262/M262-D003/arc_debug_instrumentation_summary.json`

## Next Issue

- `M262-E001`
