# M254-C001 Bootstrap Lowering Contract and Architecture Freeze Packet

Packet: `M254-C001`
Milestone: `M254`
Lane: `C`
Issue: `#7105`

## Goal

Freeze the lowering-owned startup-bootstrap boundary so later ctor-root/init-stub/registration-table materialization consumes one authoritative packet built from the emitted registration manifest plus the live bootstrap semantics contract.

## Scope

- lower the preserved bootstrap names into one canonical boundary summary
- publish a semantic-surface packet for bootstrap lowering
- mirror the lowering contract into `module.runtime-registration-manifest.json`
- prove via native emission that the packet exists while the actual bootstrap globals remain intentionally absent

## Dependencies

None

## Canonical Anchors

- contract id `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
- surface path `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract`
- boundary model `registration-manifest-driven-constructor-root-init-stub-and-registration-table-lowering`
- constructor root symbol `__objc3_runtime_register_image_ctor`
- init-stub symbol prefix `__objc3_runtime_register_image_init_stub_`
- registration-table symbol prefix `__objc3_runtime_registration_table_`
- global-ctor list model `llvm.global_ctors-single-root-priority-65535`
- constructor-root emission state `deferred-until-m254-c002`
- init-stub emission state `deferred-until-m254-c002`
- registration-table emission state `deferred-until-m254-c002`

## Required Code Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `tests/tooling/runtime/README.md`

## Acceptance

- deterministic lowering summary exists in lowering code and emitted IR
- semantic surface and flattened manifest summary publish the same contract
- emitted registration manifest carries the bootstrap-lowering fields
- native hello probe proves the packet exists and the actual bootstrap globals are still absent
- evidence lands at `tmp/reports/m254/M254-C001/bootstrap_lowering_contract_summary.json`
