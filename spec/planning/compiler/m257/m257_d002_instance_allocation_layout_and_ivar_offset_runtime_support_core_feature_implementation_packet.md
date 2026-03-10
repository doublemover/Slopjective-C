# M257-D002 Instance Allocation Layout And Ivar Offset Runtime Support Core Feature Implementation Packet

Packet: `M257-D002`
Issue: `#7154`
Contract ID: `objc3c-runtime-instance-allocation-layout-support/m257-d002-v1`

Summary:
Implement true runtime instance allocation and slot-backed synthesized property execution for the supported Objective-C 3 property/ivar subset.

Inputs:

- `tests/tooling/fixtures/native/m257_d002_instance_allocation_runtime_positive.objc3`
- `tests/tooling/runtime/m257_d002_instance_allocation_runtime_probe.cpp`
- emitted ivar offsets and layout tables from `M257-C002`
- emitted synthesized accessor/property metadata from `M257-C003`
- realized class graph/runtime bootstrap from `M256-D002` and `M257-D001`

Required implementation points:

1. `alloc` / `new` produce distinct runtime instance identities per allocation.
2. Runtime instance storage size is taken from realized class layout.
3. Synthesized getter/setter dispatch reads and writes per-instance slot storage.
4. Runtime graph snapshots publish live instance-allocation evidence.
5. Realized class entry snapshots publish runtime property accessor counts and instance size.
6. IR publishes `; runtime_instance_allocation_layout_support = ...` with deterministic inventory fields.
7. Historical `M257-C003` / `M257-D001` validation remains forward-compatible after D002 lands.

Evidence:

- `tmp/reports/m257/M257-D002/instance_allocation_layout_runtime_summary.json`

Touched surfaces:

- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/runtime/objc3_runtime.h`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `native/objc3c/src/runtime/README.md`
- `tests/tooling/runtime/README.md`
- `package.json`
