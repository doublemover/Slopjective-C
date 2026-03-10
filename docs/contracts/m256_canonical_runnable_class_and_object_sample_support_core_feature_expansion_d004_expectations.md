# M256 Canonical Runnable Class and Object Sample Support Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1`

## Scope
- Runtime must admit the canonical executable object sample surface through runtime-owned builtin `alloc`, `new`, and `init` handling instead of requiring placeholder source bodies.
- Inherited class-method dispatch for canonical object samples must resolve through the realized superclass chain.
- Metadata-rich category/protocol behavior must remain proven through a dedicated library-plus-probe path until the wider runtime export gate is open.
- Validation evidence must land at `tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json`.

## Required models
- `canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch`
- `metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits`
- `metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open`

## Required proof assets
- Executable sample: `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_sample.objc3`
- Metadata-rich runtime library: `tests/tooling/fixtures/native/m256_d004_canonical_runnable_object_runtime_library.objc3`
- Probe: `tests/tooling/runtime/m256_d004_canonical_runnable_object_probe.cpp`

## Dynamic proof requirements
- The executable sample must compile, link, run, and return exit code `37`.
- The emitted IR for both proof fixtures must publish `; runtime_canonical_runnable_object_sample_support = ...`.
- `objc3_runtime_dispatch_i32(classReceiver, "alloc", ...)` must return the canonical instance receiver and cache owner `runtime-builtin:Widget::class_method:alloc`.
- `objc3_runtime_dispatch_i32(instanceReceiver, "init", ...)` must return the same instance receiver and cache owner `runtime-builtin:Widget::instance_method:init`.
- `objc3_runtime_dispatch_i32(classReceiver, "new", ...)` must return the canonical instance receiver and cache owner `runtime-builtin:Widget::class_method:new`.
- Attached-category dispatch must still return `13`, inherited instance dispatch must still return `7`, and concrete class dispatch must still return `11`.
- Runtime query `Widget -> Worker` must conform without an attachment owner match.
- Runtime query `Widget -> Tracer` must conform through attachment owner `category:Widget(Tracing)`.
