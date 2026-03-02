# M226 Parse-Lowering Modular Split Expectations (C002)

Contract ID: `objc3c-parse-lowering-modular-split-contract/m226-c002-v1`
Status: Accepted
Scope: Parse-to-lowering readiness modular split and fail-closed scaffolding in `native/objc3c/src/pipeline/*`.

## Objective

Introduce a concrete modular boundary for parse artifacts entering lowering so
readiness is computed once, projected deterministically, and enforced fail-closed
before lowering contracts and IR emission proceed.

## Required Invariants

1. Dedicated readiness module exists:
   - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
2. Pipeline wiring computes readiness surface during frontend execution:
   - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` includes
     `pipeline/objc3_parse_lowering_readiness_surface.h`.
   - `RunObjc3FrontendPipeline(...)` assigns
     `result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);`.
3. Artifact wiring enforces fail-closed readiness before lowering:
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` recomputes readiness via
     `BuildObjc3ParseLoweringReadinessSurface(...)`.
   - `IsObjc3ParseLoweringReadinessSurfaceReady(...)` gates emission and returns deterministic
     diagnostics on failure.
4. Artifact surface exports readiness projection:
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.h` includes
     `Objc3ParseLoweringReadinessSurface parse_lowering_readiness_surface;`.
   - Manifest projection includes `parse_lowering_readiness` with
     `ready_for_lowering`, `lowering_boundary_ready`, and replay/failure fields.
5. Checker + tests remain fail-closed and write summaries under `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_c002_parse_lowering_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c002_parse_lowering_modular_split_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_c002_parse_lowering_modular_split_contract_summary.json`
