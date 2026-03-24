# M271-C001 Packet: System Extension Lowering Contract - Contract And Architecture Freeze

## Objective

Freeze one truthful Part 8 lowering contract for cleanup/resource ownership, borrowed boundaries, explicit capture inventories, and retainable-family callable inventories.

## Implementation Requirements

1. Add a dedicated lowering contract in `native/objc3c/src/lower/objc3_lowering_contract.h` and validation/replay-key support in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
2. Build the contract in `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` from the existing M271 B-lane semantic summaries.
3. Publish the contract in emitted manifest JSON under `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract`.
4. Thread replay-stable Part 8 lowering metadata into emitted LLVM IR frontend metadata.
5. Add issue-local checker, readiness runner, pytest, expectations doc, and positive fixture.

## Truth Constraints

- Keep the issue fail-closed and lowering-focused.
- Do not claim live cleanup runtime execution, borrowed lifetime runtime interop, or runnable retainable-family execution behavior.
- Preserve later M271 lane-D runtime work as the only place allowed to widen this boundary.
