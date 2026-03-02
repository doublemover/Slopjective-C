# M228 IR Emission Completeness for ObjC Patterns Contract and Architecture Freeze (C001)

Contract ID: `objc3c-ir-emission-completeness-freeze/m228-c001-v1`
Status: Accepted
Scope: lane-C freeze anchors for direct LLVM IR emission completeness surfaces, fail-closed pipeline routing, and metadata replay anchors.

## Objective

Freeze IR emission completeness contracts for ObjC lowering patterns so direct
LLVM IR emission remains deterministic, metadata-rich, and fail-closed before
lane-C modular split work.

## Scope Anchors

- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ARCHITECTURE.md`

## Deterministic Invariants

1. IR emitter metadata contract remains explicit:
   - `Objc3IRFrontendMetadata`
   - `EmitObjc3IRText(...)`
2. Frontend artifacts route direct IR emission through fail-closed gates and
   metadata transport:
   - pass-graph scaffold/core/expansion gates (`O3L301`/`O3L302`/`O3L303`)
   - metadata assignment to `ir_frontend_metadata`
   - fail-closed `EmitObjc3IRText(...)` invocation.
3. Lowering boundary/runtime dispatch declaration replay key contracts stay
   deterministic:
   - `TryBuildObjc3LoweringIRBoundary(...)`
   - `Objc3LoweringIRBoundaryReplayKey(...)`
   - `Objc3RuntimeDispatchDeclarationReplayKey(...)`
4. IR textual output includes deterministic frontend metadata lines for
   pass-graph core/expansion readiness keys.
5. Architecture/spec anchors explicitly include M228 lane-C C001 freeze intent.

## Validation

- `python scripts/check_m228_c001_ir_emission_completeness_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c001_ir_emission_completeness_contract.py -q`
- `npm run check:objc3c:m228-c001-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-C001/ir_emission_completeness_contract_summary.json`
