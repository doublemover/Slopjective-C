# M257 Property And Ivar Executable Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-executable-property-ivar-source-closure/m257-a001-v1`

## Objective

Freeze the source-surface boundary for the property, ivar, and accessor subset
that `M257` will make truly executable.

## Required implementation

1. Add a canonical expectations document for the executable property/ivar source
   closure boundary.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m257_a001_property_and_ivar_executable_source_closure_contract_and_architecture_freeze.py`
   - `scripts/run_m257_a001_lane_a_readiness.py`
3. Add `M257-A001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Freeze the current source-surface contract around:
   - `Objc3PropertyDecl`
   - `Objc3PropertyDecl.ivar_binding_symbol`
   - `Objc3InterfaceDecl.property_synthesis_symbols_lexicographic`
   - `Objc3InterfaceDecl.ivar_binding_symbols_lexicographic`
   - `Objc3ImplementationDecl.property_synthesis_symbols_lexicographic`
   - `Objc3ImplementationDecl.ivar_binding_symbols_lexicographic`
   - `frontend.pipeline.sema_pass_manager.lowering_property_synthesis_ivar_binding_replay_key`
5. The checker must prove the boundary with one live compile of:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   It must fail closed unless the manifest surface, replay key, and emitted IR
   line up with the frozen property/ivar source model.
6. `package.json` must wire:
   - `check:objc3c:m257-a001-property-and-ivar-executable-source-closure`
   - `test:tooling:m257-a001-property-and-ivar-executable-source-closure`
   - `check:objc3c:m257-a001-lane-a-readiness`
7. The contract must explicitly hand off to `M257-A002`.

## Canonical models

- Source-surface model:
  `property-ivar-executable-source-closure-freezes-decls-synthesis-bindings-and-accessor-selectors-before-storage-realization`
- Evidence model:
  `class-protocol-property-ivar-fixture-manifest-and-ir-replay-key`
- Failure model:
  `fail-closed-on-property-ivar-source-surface-drift-before-layout-and-accessor-expansion`

## Non-goals

- No ivar layout realization yet.
- No synthesized accessor body emission yet.
- No instance storage allocation yet.
- No runtime property/ivar execution semantics yet.

## Evidence

- `tmp/reports/m257/M257-A001/property_ivar_executable_source_closure_summary.json`
