# Runtime-Facing Type Metadata Modular Split and Scaffolding Expectations (M227-D002)

Contract ID: `objc3c-runtime-facing-type-metadata-modular-split-scaffold/m227-d002-v1`
Status: Accepted
Scope: Runtime-facing type metadata modular split/scaffold boundaries across sema pass orchestration and frontend artifact projection.

## Objective

Preserve deterministic runtime-facing type metadata behavior while enforcing scaffold-oriented sema modular split boundaries so follow-on runtime/type-semantic shards avoid re-monolithizing sema orchestration.

## Required Invariants

1. Runtime-facing sema handoff scaffold remains explicit:
   - `native/objc3c/src/sema/objc3_parser_sema_handoff_scaffold.h`
   - `BuildObjc3ParserSemaHandoffScaffold(...)`
   - `kObjc3ParserSemaHandoffScaffoldBuilderMaxLines`
2. Sema pass-flow scaffold remains the sole pass-flow finalization boundary:
   - `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.h`
   - `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`
   - `MarkObjc3SemaPassExecuted(...)`
   - `FinalizeObjc3SemaPassFlowSummary(...)`
3. Sema pass manager consumes scaffold entry points before and after runtime-facing type metadata handoff:
   - `BuildObjc3ParserSemaHandoffScaffold(...)` invoked before type metadata handoff derivation.
   - `BuildSemanticTypeMetadataHandoff(...)` and `IsDeterministicSemanticTypeMetadataHandoff(...)` remain wired.
   - `FinalizeObjc3SemaPassFlowSummary(...)` remains wired after type metadata handoff readiness is established.
4. Runtime-facing metadata transport/projection remains deterministic:
   - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` transports `sema_type_metadata_handoff` and `sema_parity_surface`.
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` derives runtime-shim/retain-release lowering contracts from parity surfaces and projects deterministic metadata + replay keys.
5. Build manifests keep scaffold compilation wiring:
   - `native/objc3c/CMakeLists.txt` includes `src/sema/objc3_sema_pass_flow_scaffold.cpp`.
   - `scripts/build_objc3c_native.ps1` includes `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`.
6. D002 contract tooling remains fail-closed and emits deterministic summary artifacts under `tmp/reports/m227/`.

## Validation

- `python scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json`
