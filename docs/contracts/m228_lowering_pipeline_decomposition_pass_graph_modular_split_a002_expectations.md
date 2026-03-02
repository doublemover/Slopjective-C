# M228 Lowering Pipeline Decomposition and Pass-Graph Modular Split Expectations (A002)

Contract ID: `objc3c-lowering-pipeline-pass-graph-modular-split/m228-a002-v1`
Status: Accepted
Scope: `native/objc3c/src/pipeline/*`, `native/objc3c/src/ir/objc3_ir_emitter.cpp`, `native/objc3c/src/lower/objc3_lowering_contract.cpp`, and build/spec anchors.

## Objective

Split lowering pass-graph readiness synthesis into dedicated scaffolding so
direct LLVM IR emission hardening can evolve without collapsing stage-gating
logic back into monolithic pipeline and artifact builders.

## Required Invariants

1. Dedicated pass-graph scaffold module exists:
   - `native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h`
   - `native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp`
2. Frontend pipeline consumes the scaffold during post-sema/lowering setup:
   - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` includes the scaffold header.
   - `result.lowering_pipeline_pass_graph_scaffold = BuildObjc3LoweringPipelinePassGraphScaffold(...)`.
3. Artifact emission is fail-closed on pass-graph readiness before IR text generation:
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` uses
     `IsObjc3LoweringPipelinePassGraphScaffoldReady(...)`.
   - failure path emits deterministic diagnostic code `O3L301`.
4. Build graphs wire the new module:
   - `native/objc3c/CMakeLists.txt` includes
     `src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp` in `objc3c_pipeline`.
   - `scripts/build_objc3c_native.ps1` source manifest includes
     `native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp`.
5. Scope anchors remain explicit in docs/spec:
   - `native/objc3c/src/ARCHITECTURE.md` includes M228-A002 modular split note.
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes pass-graph scaffold requirement.
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes lowering pass-graph replay anchor requirement.

## Validation

- `python scripts/check_m228_a002_lowering_pipeline_decomposition_pass_graph_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a002_lowering_pipeline_decomposition_pass_graph_modular_split_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-A002/lowering_pipeline_pass_graph_modular_split_contract_summary.json`
