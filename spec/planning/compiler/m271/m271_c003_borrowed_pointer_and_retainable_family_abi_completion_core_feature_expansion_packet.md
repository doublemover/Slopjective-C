# M271-C003 Packet: Borrowed-Pointer And Retainable-Family ABI Completion - Core Feature Expansion

Packet: `M271-C003`
Issue: `#7329`
Milestone: `M271`
Lane: `C`

## Objective

Implement the remaining compiler-side ABI/artifact/replay layer for the supported Part 8 borrowed-return and retainable-family call-boundary slice.

## Scope

- keep `M271-C001` as the only Part 8 lowering contract
- publish one dedicated ABI/artifact packet above the frozen lowering contract
- preserve borrowed-return attribute counts and retainable-family operation/compatibility-alias counts through emitted manifests and IR metadata
- prove the supported native direct-call path with a positive fixture that emits `module.ll`, `module.obj`, `module.manifest.json`, and `module.object-backend.txt`

## Non-goals

- no second Part 8 lowering packet
- no lane-D runtime/helper ABI freeze work
- no borrowed lifetime runtime interop claims
- no escaping move-capture ownership-transfer claims
- no runnable retainable-family runtime execution claims

## Required evidence

- deterministic source anchors in:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- native positive fixture at `tests/tooling/fixtures/native/m271_c003_borrowed_retainable_abi_completion_positive.objc3`
- issue-local checker, readiness runner, and pytest harness
- summary written to `tmp/reports/m271/M271-C003/borrowed_retainable_abi_completion_summary.json`
- next issue remains `M271-D001`
