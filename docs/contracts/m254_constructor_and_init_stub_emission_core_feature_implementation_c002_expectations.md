# M254 Constructor and Init-Stub Emission Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-runtime-constructor-init-stub-emission/m254-c002-v1`

Scope:

- materialize one real startup bootstrap path in emitted LLVM IR and object output
- preserve the frozen lowering-owned names from `M254-C001`
- prove the happy path registers one emitted metadata image before probe `main`

Required evidence:

1. `module.ll` publishes `; runtime_bootstrap_ctor_init_emission = ...`.
2. `module.ll` emits:
   - `@llvm.global_ctors`
   - `__objc3_runtime_register_image_ctor`
   - a derived `__objc3_runtime_register_image_init_stub_*`
   - a derived `__objc3_runtime_registration_table_*`
   - a derived `__objc3_runtime_image_descriptor_*`
3. `module.runtime-registration-manifest.json` carries the exact derived init-stub and registration-table symbols.
4. `module.obj` keeps a live startup section for the ctor list on COFF (`.CRT$XCU`).
5. Linking `tests/tooling/runtime/m254_c002_constructor_startup_probe.cpp` with the emitted metadata-only object and `artifacts/lib/objc3_runtime.lib` proves:
   - `registered_image_count == 1`
   - `registered_descriptor_total == total_descriptor_count`
   - `next_expected_registration_order_ordinal == 2`
   - `last_successful_registration_order_ordinal == 1`
   - `last_registration_status == 0`
   - `last_registered_translation_unit_identity_key` matches the emitted registration manifest
6. Validation evidence lands at `tmp/reports/m254/M254-C002/constructor_init_stub_emission_summary.json`.
