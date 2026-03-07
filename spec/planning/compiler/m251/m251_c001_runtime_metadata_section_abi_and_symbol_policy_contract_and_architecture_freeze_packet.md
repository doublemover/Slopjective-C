# M251-C001 Runtime Metadata Section ABI and Symbol Policy Contract and Architecture Freeze Packet

Packet: `M251-C001`
Milestone: `M251`
Lane: `C`
Issue: `#7062`
Dependencies: none
Contract ID: `objc3c-runtime-metadata-section-abi-symbol-policy-freeze/m251-c001-v1`

## Goal

Freeze the logical runtime metadata section inventory, symbol naming policy, linkage/visibility model, and retention-root policy before LLVM globals and physical object sections are emitted.

## Scope

- Freeze the runtime metadata section ABI as a single canonical lane-C packet.
- Publish the same ABI freeze through manifest JSON and emitted LLVM IR metadata.
- Keep the boundary fail-closed until runtime metadata ownership and runtime export enforcement prerequisites are ready.
- Preserve deterministic docs/spec/package anchors so the next implementation issue has an explicit boundary to keep.

## Deterministic Probe Matrix

The checker runs two deterministic probes:

1. Manifest-only frontend runner probe proving the runtime metadata section ABI packet is emitted and marked ready when the current A/B prerequisites hold.
2. Native IR emission probe proving LLVM IR publishes `; runtime_metadata_section_abi = ...` and `!objc3.objc_runtime_metadata_section_abi`.

## Code Anchors

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `tests/tooling/runtime/objc3_msgsend_i32_shim.c`

## Spec Anchors

- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `docs/objc3c-native.md`

## Implementation Notes

- C001 freezes logical section identifiers rather than physical COFF/ELF/Mach-O spellings.
- C001 does not reserve LLVM globals or emit physical object sections yet; C002 must preserve the freeze packet when that implementation lands.
- Manifest and IR publication are mandatory so later object-inspection harnesses can diff against one canonical boundary.

## Evidence

- `tmp/reports/m251/M251-C001/runtime_metadata_section_abi_and_symbol_policy_contract_summary.json`
