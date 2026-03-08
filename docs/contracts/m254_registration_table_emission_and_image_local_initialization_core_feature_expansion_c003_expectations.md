# M254 Registration Table Emission and Image-Local Initialization Core Feature Expansion Expectations (C003)

Contract ID: `objc3c-runtime-registration-table-image-local-initialization/m254-c003-v1`

Scope: Expand the lowering-owned startup bootstrap path so emitted registration tables are self-describing, carry deterministic section-root pointers for later runtime image walks, and publish one image-local init-state cell that the generated init stub guards before calling `objc3_runtime_register_image`.

## Required outcomes

1. `native/objc3c/src/ir/objc3_ir_emitter.cpp` emits one additional IR boundary line for C003 with:
   - the exact derived registration-table symbol,
   - layout model `abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-image-local-init-state`,
   - ABI version `1`,
   - pointer-field count `11`,
   - the canonical class/protocol/category/property/ivar section-root symbols,
   - selector/string pool roots or `null`, and
   - the exact derived image-local init-state symbol.
2. The emitted registration table is no longer `{ ptr, ptr, ptr }`. It is a self-describing constant table with leading ABI/version integers and pointers for:
   - image descriptor,
   - discovery root,
   - linker anchor,
   - class/protocol/category/property/ivar aggregates,
   - selector pool root,
   - string pool root,
   - image-local init-state cell.
3. The emitted init stub guards startup with one image-local init-state cell before calling `objc3_runtime_register_image`, and stores success back into that cell on the happy path.
4. `module.runtime-registration-manifest.json` publishes:
   - `bootstrap_registration_table_layout_model`,
   - `bootstrap_image_local_initialization_model`,
   - `bootstrap_image_local_init_state_symbol_prefix` with prefix value `__objc3_runtime_image_local_init_state_`,
   - the exact derived `bootstrap_image_local_init_state_symbol`,
   - `bootstrap_registration_table_abi_version`, and
   - `bootstrap_registration_table_pointer_field_count`.
5. Evidence must cover at least:
   - the metadata-library fixture from `M254-C002`, and
   - one category-bearing metadata-only library fixture.
6. Validation evidence lands at `tmp/reports/m254/M254-C003/registration_table_image_local_initialization_summary.json`.
