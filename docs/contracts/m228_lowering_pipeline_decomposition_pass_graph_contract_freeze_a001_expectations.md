# M228 Lowering Pipeline Decomposition and Pass-Graph Contract Freeze (A001)

Contract ID: `objc3c-lowering-pipeline-pass-graph-freeze/m228-a001-v1`
Status: Accepted
Scope: `native/objc3c/src/pipeline/*`, `native/objc3c/src/lower/*`, and architecture anchors.

## Objective

Freeze lowering-pipeline decomposition and pass-graph ordering so frontend
stage orchestration, parse/sema/lowering handoff surfaces, and direct LLVM IR
emission entrypoints remain deterministic and fail-closed before M228 expansion
work starts.

## Scope Anchors

- `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ARCHITECTURE.md`

## Deterministic Invariants

1. Frontend pipeline contract remains explicit and ordered:
   - `StageId::{Lex,Parse,Sema,Lower,Emit}`
   - `kStageOrder` preserves lex->parse->sema->lower->emit ordering
   - `FrontendPipelineOutput` includes lower/emit stage result surfaces
2. Frontend pipeline orchestration remains wrapper-based and deterministic:
   - parse handoff via `BuildObjc3AstFromTokens(...)`
   - sema handoff via `RunObjc3SemaPassManager(...)`
   - lowering-readiness and stability scaffolds built in canonical order
3. Artifact generation remains fail-closed on readiness and lowering contracts:
   - `IsObjc3ParseLoweringReadinessSurfaceReady(...)` gate
   - lowering contract builders (`BuildMessageSendSelectorLoweringContract`, `BuildDispatchAbiMarshallingContract`)
   - direct IR emission through `EmitObjc3IRText(...)`
4. Lowering boundary normalization and replay-key derivation remain deterministic:
   - `TryBuildObjc3LoweringIRBoundary(...)`
   - `Objc3LoweringIRBoundaryReplayKey(...)`
   - `Objc3RuntimeDispatchDeclarationReplayKey(...)`
5. Architecture contract explicitly records M228 lane-A A001 freeze anchors.

## Validation

- `python scripts/check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py -q`
- `python scripts/check_m226_c020_parse_lowering_integration_closeout_gate_signoff_contract.py`

## Evidence Path

- `tmp/reports/m228/m228_a001_lowering_pipeline_decomposition_pass_graph_contract_summary.json`
