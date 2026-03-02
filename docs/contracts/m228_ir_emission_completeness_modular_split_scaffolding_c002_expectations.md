# M228 IR Emission Completeness Modular Split Scaffolding Expectations (C002)

Contract ID: `objc3c-ir-emission-completeness-modular-split-scaffolding/m228-c002-v1`
Status: Accepted
Scope: lane-C modular split/scaffolding continuity for IR emission completeness across `native/objc3c/src/pipeline/*` and direct IR metadata wiring.

## Objective

Split IR emission completeness closure into a dedicated scaffold so pass-graph
core/expansion/edge compatibility readiness and replay keys remain deterministic
and fail-closed while being transported into direct LLVM IR metadata.

## Required Invariants

1. Dedicated IR emission completeness scaffold module exists:
   - `native/objc3c/src/pipeline/objc3_ir_emission_completeness_scaffold.h`
   - `native/objc3c/src/pipeline/objc3_ir_emission_completeness_scaffold.cpp`
2. Frontend pipeline wires the scaffold deterministically:
   - `RunObjc3FrontendPipeline(...)` includes `objc3_ir_emission_completeness_scaffold.h`.
   - `result.ir_emission_completeness_scaffold = BuildObjc3IREmissionCompletenessScaffold(result);`
3. Artifact emission remains fail-closed and diagnostics-stable:
   - `BuildObjc3FrontendArtifacts(...)` validates scaffold-driven
     core/expansion/edge compatibility readiness.
   - failure paths keep deterministic diagnostic codes `O3L302`/`O3L303`/`O3L304`.
4. IR metadata transport is scaffold-owned:
   - pass-graph core/expansion/edge keys come from
     `ir_emission_completeness_scaffold`.
   - metadata includes `ir_emission_completeness_modular_split` and
     `ir_emission_completeness_modular_split_ready`.
5. Build and architecture/spec anchors remain explicit:
   - `native/objc3c/CMakeLists.txt` and `scripts/build_objc3c_native.ps1` include
     `objc3_ir_emission_completeness_scaffold.cpp`.
   - `native/objc3c/src/ARCHITECTURE.md` includes M228-C002 modular split note.
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` and
     `spec/MODULE_METADATA_AND_ABI_TABLES.md` include C002 modular split anchors.
6. C001 freeze remains a required prerequisite:
   - `docs/contracts/m228_ir_emission_completeness_contract_freeze_c001_expectations.md`

## Validation

- `python scripts/check_m228_c002_ir_emission_completeness_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c002_ir_emission_completeness_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m228-c002-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-C002/ir_emission_completeness_modular_split_scaffolding_contract_summary.json`
