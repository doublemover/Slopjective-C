# M243 Lowering/Runtime Diagnostics Surfacing Modular Split Scaffolding Expectations (C002)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-modular-split-scaffolding/m243-c002-v1`
Status: Accepted
Scope: lane-C modular split/scaffolding continuity for lowering/runtime
diagnostics surfacing across `native/objc3c/src/pipeline/*`.

## Objective

Split lowering/runtime diagnostics surfacing readiness into a dedicated scaffold
so fail-closed diagnostics publication remains deterministic across
parse/lowering handoff boundaries before IR emission.

## Required Invariants

1. Dedicated lane-C diagnostics surfacing scaffold module exists:
   - `native/objc3c/src/pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h`
2. Frontend pipeline wires the scaffold deterministically:
   - `RunObjc3FrontendPipeline(...)` includes
     `objc3_lowering_runtime_diagnostics_surfacing_scaffold.h`.
   - `result.lowering_runtime_diagnostics_surfacing_scaffold = BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(result);`
3. Artifact emission remains fail-closed on scaffold drift:
   - `BuildObjc3FrontendArtifacts(...)` validates
     `IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(...)`.
   - Scaffold failure path emits deterministic `O3L301` diagnostics.
4. `Objc3LoweringRuntimeDiagnosticsSurfacingScaffold` remains the canonical C002
   modular split surface with deterministic readiness fields and replay keys.
5. Architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
6. C001 freeze remains a mandatory prerequisite:
   - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_c001_expectations.md`
   - `scripts/check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
   - `tests/tooling/test_check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
   - `spec/planning/compiler/m243/m243_c001_lowering_runtime_diagnostics_surfacing_contract_and_architecture_freeze_packet.md`

## Validation

- `python scripts/check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c002_lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m243-c002-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C002/lowering_runtime_diagnostics_surfacing_modular_split_scaffolding_contract_summary.json`
