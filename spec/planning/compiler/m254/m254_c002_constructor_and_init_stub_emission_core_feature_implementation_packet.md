# M254-C002 Constructor and Init-Stub Emission Core Feature Implementation Packet

Packet: `M254-C002`
Milestone: `M254`
Lane: `C`
Dependencies: `M254-C001`, `M254-A002`, `M254-B002`

## Objective

Turn the frozen bootstrap-lowering boundary into a real emitted startup path.

## Required implementation

- Contract id `objc3c-runtime-constructor-init-stub-emission/m254-c002-v1`
- Emit one real `@llvm.global_ctors` entry rooted at `__objc3_runtime_register_image_ctor`
- Emit one derived init stub using prefix `__objc3_runtime_register_image_init_stub_`
- Emit one derived registration table using prefix `__objc3_runtime_registration_table_`
- Emit one derived image descriptor that matches `objc3_runtime_image_descriptor`
- Call `objc3_runtime_register_image` from the init stub
- Fail closed through `abort()` if registration returns a non-zero status
- Publish exact derived init-stub and registration-table symbols in `module.runtime-registration-manifest.json`
- Preserve deterministic COFF startup lowering through `.CRT$XCU`

## Code anchors

- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `tests/tooling/runtime/m254_c002_constructor_startup_probe.cpp`
- `tests/tooling/fixtures/native/m254_c002_runtime_bootstrap_metadata_library.objc3`

## Acceptance criteria

- Constructor and init-stub lowering is a real emitted capability, not a manifest-only placeholder.
- The registration manifest and emitted IR agree on the derived init-stub and registration-table symbols.
- The emitted object carries a startup constructor section on COFF.
- The linked startup probe proves registration happens before `main` and matches manifest counts/identity.
- Validation evidence lands at `tmp/reports/m254/M254-C002/constructor_init_stub_emission_summary.json`.
