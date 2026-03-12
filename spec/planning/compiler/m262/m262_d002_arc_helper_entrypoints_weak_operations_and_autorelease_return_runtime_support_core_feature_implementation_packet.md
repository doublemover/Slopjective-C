# M262-D002 ARC Helper Entrypoints, Weak Operations, And Autorelease-Return Runtime Support Core Feature Implementation Packet

Packet: `M262-D002`
Issue: `#7204`
Milestone: `M262`
Lane: `D`

## Objective

Promote the private ARC helper ABI frozen by `M262-D001` into a truthful live
runtime capability for the currently supported ARC property/weak and
autorelease-return slice.

## Dependency Chain

- `M260-D002` supplies the runtime memory-management baseline for retain,
  release, weak side-table behavior, and autoreleasepool scopes.
- `M262-C004` supplies the ARC lowering path that reaches the autorelease
  helper surface from supported escaping-block and autorelease-return code.
- `M262-D001` freezes the private helper ABI and public/private header
  separation that `D002` must preserve.

## Implementation Requirements

1. Add one explicit D002 runtime-support contract to the lowering contract
   surface and emitted IR metadata.
2. Keep the helper ABI private:
   - no public runtime header widening
   - no user-facing ARC helper include surface
3. Prove the weak/property path truthfully using
   `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`.
4. Prove the autorelease-return execution path truthfully using
   `tests/tooling/fixtures/native/m262_arc_block_autorelease_return_positive.objc3`.
5. Use the runtime library already published by the native build rather than a
   separate ad hoc runtime stub.
6. Keep the execution proof focused on the helper runtime itself rather than
   the broader bootstrap/executable-launch surface that is owned by `M263`.

## Dynamic Proof Model

### Property Helper Probe

Compile the ARC property fixture with `-fobjc-arc` and verify:

- native compile exits `0`
- `module.manifest.json`, `module.ll`, and `module.obj` exist
- emitted IR publishes the D002 boundary line and named metadata
- emitted IR contains weak current-property helper calls:
  - `objc3_runtime_load_weak_current_property_i32`
  - `objc3_runtime_store_weak_current_property_i32`
- emitted runtime metadata source records keep:
  - `currentValue` as `strong-owned`
  - `weakValue` as `weak`

### Autorelease-Return Lowering Probe

Compile the ARC autorelease-return fixture with `-fobjc-arc` and verify:

- native compile exits `0`
- `module.manifest.json`, `module.ll`, `module.obj`, and
  `module.runtime-registration-manifest.json` exist
- emitted IR publishes the D002 boundary line and named metadata
- emitted IR contains `objc3_runtime_autorelease_i32`

### Runtime Helper Execution Probe

Compile and run `tests/tooling/runtime/m262_d002_arc_helper_runtime_support_probe.cpp`
against `artifacts/lib/objc3_runtime.lib` and verify:

- the probe links successfully against the runtime library
- the probe executes successfully
- retain/release helper calls round-trip the value `9`
- autoreleasepool push/pop and autorelease bookkeeping publish deterministic
  snapshot state before and after pool drain

## Non-Goals

- no public ARC helper API
- no new debug or ownership instrumentation hooks
- no widening beyond the currently supported ARC property/weak and
  autorelease-return slice
- no attempt to treat broader bootstrap/executable-launch behavior as already
  closed out inside `M262-D002`

## Required Outputs

- `docs/contracts/m262_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation_d002_expectations.md`
- `scripts/check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py`
- `scripts/run_m262_d002_lane_d_readiness.py`
- `tests/tooling/test_check_m262_d002_arc_helper_entrypoints_weak_operations_and_autorelease_return_runtime_support_core_feature_implementation.py`
- `tmp/reports/m262/M262-D002/arc_helper_runtime_support_summary.json`

## Next Issue

- `M262-D003`
