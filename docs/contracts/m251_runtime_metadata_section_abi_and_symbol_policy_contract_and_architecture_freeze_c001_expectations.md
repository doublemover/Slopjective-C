# M251 Runtime Metadata Section ABI and Symbol Policy Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-runtime-metadata-section-abi-symbol-policy-freeze/m251-c001-v1`
Status: Accepted
Scope: M251 lane-C runtime metadata section ABI and symbol policy contract freeze for deterministic lowering and object-emission continuity.

## Objective

Fail closed unless lane-C runtime metadata section ABI and symbol policy anchors remain explicit, deterministic, and published through manifest/IR surfaces before physical object-section emission begins.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m251/m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract.py`
  - `tests/tooling/test_check_m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract.py`

## Required Invariants

1. `Objc3RuntimeMetadataSectionAbiFreezeSummary` remains the canonical lane-C freeze packet for runtime metadata section inventory and symbol policy.
2. The logical section inventory remains frozen for `objc3.runtime.image_info`, `objc3.runtime.class_descriptors`, `objc3.runtime.protocol_descriptors`, `objc3.runtime.category_descriptors`, `objc3.runtime.property_descriptors`, and `objc3.runtime.ivar_descriptors`.
3. Descriptor prefix `__objc3_meta_`, aggregate prefix `__objc3_sec_`, and image info symbol `__objc3_image_info` remain canonical.
4. descriptor prefix `__objc3_meta_`, aggregate prefix `__objc3_sec_`, and image info symbol `__objc3_image_info` remain canonical.
5. Descriptor linkage `private`, aggregate linkage `internal`, metadata visibility `hidden`, and retention root `llvm.used` remain canonical.
6. Manifest JSON and emitted LLVM IR publish the same contract surface through `runtime_metadata_section_abi_contract_id` and `!objc3.objc_runtime_metadata_section_abi`.
7. `driver/objc3_objc3_path.cpp` preserves manifest emission as the published ABI surface until physical section emission lands.
8. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicit test-only evidence and not the metadata section/object retention implementation.

## Non-Goals and Fail-Closed Rules

- `M251-C001` does not emit physical object-file sections yet.
- `M251-C001` does not allocate LLVM globals for runtime metadata payloads yet.
- `M251-C001` does not replace the shim with the native runtime library.
- The freeze packet must therefore remain fail-closed until the runtime metadata source ownership, legality, and enforcement packets are all ready.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m251-c001-runtime-metadata-section-abi-and-symbol-policy-contract`.
- `package.json` includes `test:tooling:m251-c001-runtime-metadata-section-abi-and-symbol-policy-contract`.
- `package.json` includes `check:objc3c:m251-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract.py`
- `python -m pytest tests/tooling/test_check_m251_c001_runtime_metadata_section_abi_and_symbol_policy_contract.py -q`
- `npm run check:objc3c:m251-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m251/M251-C001/runtime_metadata_section_abi_and_symbol_policy_contract_summary.json`
