# M227 Typed Sema-to-Lowering Modular Split and Scaffolding Expectations (C002)

Contract ID: `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`
Status: Accepted
Scope: Typed sema-to-lowering scaffold boundary in `native/objc3c/src/pipeline/*`.

## Objective

Split typed sema-to-lowering readiness into a dedicated scaffold module so semantic handoff guarantees stay fail-closed and deterministic while future lowering contracts expand.

## Required Invariants

1. Dedicated typed sema-to-lowering scaffold module exists:
   - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
2. Frontend pipeline computes the typed scaffold surface:
   - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` includes
     `pipeline/objc3_typed_sema_to_lowering_contract_surface.h`.
   - `RunObjc3FrontendPipeline(...)` assigns
     `result.typed_sema_to_lowering_contract_surface = BuildObjc3TypedSemaToLoweringContractSurface(result, options);`.
3. Parse-lowering readiness consumes typed scaffold output:
   - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h` includes
     `pipeline/objc3_typed_sema_to_lowering_contract_surface.h`.
   - Readiness construction derives semantic/lowering handoff booleans from
     `BuildObjc3TypedSemaToLoweringContractSurface(...)`.
4. Pipeline result surface exports typed scaffold output:
   - `native/objc3c/src/pipeline/objc3_frontend_types.h` includes
     `Objc3TypedSemaToLoweringContractSurface`.
   - `Objc3FrontendPipelineResult` exposes
     `typed_sema_to_lowering_contract_surface`.
5. Build integration exposes C002 check/test wiring:
   - `package.json` includes
     `check:objc3c:m227-c002-typed-sema-to-lowering-modular-split-contract`.
   - `package.json` includes
     `test:tooling:m227-c002-typed-sema-to-lowering-modular-split-contract`.
6. Checker + tests remain fail-closed and write summaries under `tmp/reports/m227/`.

## Validation

- `python scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py -q`

## Evidence Path

- `tmp/reports/m227/m227_c002_typed_sema_to_lowering_modular_split_contract_summary.json`
