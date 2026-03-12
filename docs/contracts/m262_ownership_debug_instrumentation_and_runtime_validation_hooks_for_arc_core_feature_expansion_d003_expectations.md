# M262 Ownership Debug Instrumentation And Runtime Validation Hooks For ARC Core Feature Expansion Expectations (D003)

Contract ID: `objc3c-runtime-arc-debug-instrumentation/m262-d003-v1`

## Required boundary

- emitted IR publishes:
  - `; runtime_arc_debug_instrumentation = ...`
  - `!objc3.objc_runtime_arc_debug_instrumentation = !{!85}`
- ARC debug hooks remain private to `runtime/objc3_runtime_bootstrap_internal.h`
- the public runtime header remains unchanged

## Required runtime debug surface

- deterministic counters for:
  - `objc3_runtime_retain_i32`
  - `objc3_runtime_release_i32`
  - `objc3_runtime_autorelease_i32`
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
  - `objc3_runtime_read_current_property_i32`
  - `objc3_runtime_write_current_property_i32`
  - `objc3_runtime_exchange_current_property_i32`
  - `objc3_runtime_load_weak_current_property_i32`
  - `objc3_runtime_store_weak_current_property_i32`
- deterministic last-value fields for the helper families above
- deterministic last property context:
  - receiver
  - property name
  - property owner identity

## Required validation

- the ARC property fixture continues to compile with `-fobjc-arc`
- the ARC block/autorelease-return fixture continues to compile with `-fobjc-arc`
- `tests/tooling/runtime/m262_d003_arc_debug_instrumentation_probe.cpp` links
  against `artifacts/lib/objc3_runtime.lib` and executes successfully
- runtime probe output proves:
  - helper counters advance on the supported ARC slice
  - property/weak helper counters are deterministic
  - the last property context is published for the final weak getter path

## Non-goals

- no public ARC debug ABI
- no user-facing ownership tracing hooks
- no broader ARC runtime completeness claim beyond the supported runnable slice
