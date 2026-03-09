# M263-C001 Constructor-Root and Init-Array Lowering Contract and Architecture Freeze Packet

Packet: `M263-C001`
Milestone: `M263`
Lane: `C`
Issue: `#7225`
Dependencies: `M263-B002`

## Goal

Freeze the live lowering boundary that now materializes constructor roots, derived init stubs, registration tables, image-local init-state cells, and `llvm.global_ctors` from the emitted registration-descriptor artifact plus the emitted translation-unit registration manifest.

## Scope

- publish the live lowering contract through the existing `objc_runtime_bootstrap_lowering_contract` frontend packet
- carry the registration-descriptor handoff explicitly in that lowering packet
- prove explicit and default bootstrap fixtures emit matching descriptor/manifest/IR/object artifacts
- preserve the canonical ctor-root/init-stub/registration-table/image-local-init naming families for later multi-image lowering

## Canonical Anchors

- contract id `objc3c-runtime-constructor-root-init-array-lowering/m263-c001-v1`
- surface path `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract`
- descriptor handoff contract id `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
- descriptor artifact `module.runtime-registration-descriptor.json`
- lowering boundary model `registration-descriptor-and-registration-manifest-drive-constructor-root-init-stub-registration-table-and-platform-init-array-lowering`
- constructor root symbol `__objc3_runtime_register_image_ctor`
- init-stub prefix `__objc3_runtime_register_image_init_stub_`
- registration-table prefix `__objc3_runtime_registration_table_`
- image-local-init prefix `__objc3_runtime_image_local_init_state_`
- global ctor model `llvm.global_ctors-single-root-priority-65535`

## Required Code Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Acceptance

- one canonical lowering packet carries registration-manifest continuity, bootstrap-semantics continuity, and registration-descriptor continuity
- emitted registration manifest and emitted descriptor artifact agree on the startup symbol family
- emitted IR publishes the live lowering boundary and the live ctor/init emission path
- object emission remains non-empty and `llvm-direct`
- evidence lands at `tmp/reports/m263/M263-C001/constructor_root_and_init_array_lowering_contract_summary.json`

## Non-Goals

- no multi-image root fanout yet
- no replay partitioning/teardown policy yet
- no late linker synthesis outside the frozen lowering boundary
