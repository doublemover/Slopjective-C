# M263-C002 Registration-Descriptor Lowering And Multi-Image Root Emission Core Feature Implementation Packet

Packet: `M263-C002`
Milestone: `M263`
Lane: `C`
Issue: `#7226`
Dependencies: `M263-A002`, `M263-B003`, `M263-C001`

## Goal

Materialize the `M263-A002` registration-descriptor and image-root identities as first-class emitted LLVM/object artifacts while preserving the frozen `M263-C001` constructor-root/init-array lowering boundary.

## Scope

- publish one canonical lowering summary for descriptor/image-root emission
- thread the authoritative identifiers from the frontend closure into the IR handoff
- emit dedicated registration-descriptor and image-root globals in their own runtime sections
- retain those globals through `@llvm.used`
- prove explicit and module-derived fixtures preserve their identifiers through descriptor JSON, IR, and object output

## Canonical Anchors

- contract id `objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1`
- lowering model `frontend-identifiers-drive-emitted-registration-descriptor-and-image-root-globals`
- logical sections `objc3.runtime.registration_descriptor` and `objc3.runtime.image_root`
- symbol prefixes `__objc3_runtime_registration_descriptor_` and `__objc3_runtime_image_root_`
- payload models:
  - `registration-descriptor-record-points-at-image-root-image-descriptor-registration-table-linker-anchor-and-init-state`
  - `image-root-record-points-at-module-name-image-descriptor-registration-table-and-discovery-root`

## Required Code Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Acceptance

- emitted IR publishes one `runtime_registration_descriptor_image_root_lowering` boundary line
- emitted IR contains real retained `image_root` and `registration_descriptor` globals in the canonical runtime sections
- emitted object files carry non-empty `objc3.runtime.image_root` and `objc3.runtime.registration_descriptor` sections
- emitted object files expose the corresponding root/descriptor symbols
- default and explicit fixtures preserve their identifier spellings through descriptor JSON and IR/object emission
- evidence lands at `tmp/reports/m263/M263-C002/registration_descriptor_lowering_and_multi_image_root_emission_summary.json`

## Non-Goals

- no cross-translation-unit image-root merging yet
- no runtime multi-image fanout execution yet
- no registration-table ABI expansion in this issue
