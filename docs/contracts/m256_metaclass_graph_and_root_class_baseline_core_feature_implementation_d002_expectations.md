# M256 Metaclass Graph and Root-Class Baseline Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1`

## Scope
- Runtime must publish a realized class/metaclass graph keyed by stable receiver base identities.
- Root classes must realize with null superclass links and remain dispatchable for both instance and class methods.
- Known-class and class-self receivers must continue to normalize onto one metaclass cache key.
- Validation evidence must land at `tmp/reports/m256/M256-D002/metaclass_graph_and_root_class_baseline_summary.json`.

## Required models
- `runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records`
- `root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch`
- `missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch`

## Required fixture and probe
- Fixture: `tests/tooling/fixtures/native/m256_d002_metaclass_graph_root_class_library.objc3`
- Probe: `tests/tooling/runtime/m256_d002_metaclass_graph_root_class_probe.cpp`

## Dynamic proof requirements
- Root class `RootObject` must realize as a root node with null superclass links.
- Subclass `Widget` must realize against `RootObject` with both superclass and super-metaclass owner identities preserved.
- `objc3_runtime_dispatch_i32(1026, "shared", ...)` must return `19`.
- `objc3_runtime_dispatch_i32(1043, "shared", ...)` must inherit the root class method and return `19`.
- `objc3_runtime_dispatch_i32(1041, "shared", ...)` must hit the same normalized metaclass cache entry and return `19`.
- `objc3_runtime_dispatch_i32(1042, "rootValue", ...)` must inherit the root instance method and return `17`.
- `objc3_runtime_dispatch_i32(1042, "widgetValue", ...)` must resolve the subclass instance method and return `23`.
