# M243 Lowering/Runtime Diagnostics Surfacing Expectations (C001)

Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-freeze/m243-c001-v1`
Status: Accepted
Scope: lowering/runtime fail-closed diagnostics surfacing across artifact
construction, diagnostics artifact emission, CLI path, and C API path.

## Objective

Freeze lane-C diagnostics surfacing contracts so M243 diagnostics UX and fix-it
expansion cannot regress fail-closed lowering/runtime diagnostics visibility.

## Required Invariants

1. Frontend artifact lowering/runtime fail-closed diagnostics remain explicit:
   - `pipeline/objc3_frontend_artifacts.cpp` initializes surfaced diagnostics via
     `bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);`.
   - Parse/lowering readiness failures surface deterministic lower-stage
     diagnostics (`O3L300`) using
     `LLVM IR emission failed: parse-to-lowering readiness check failed: ...`.
   - Lowering/runtime gate failures keep explicit pass-graph/IR checks and
     fail-closed diagnostics before IR emission.
2. Diagnostics artifact outputs remain merge-complete:
   - `io/objc3_diagnostics_artifacts.cpp` merges stage diagnostics and
     `post_pipeline_diagnostics` into a single diagnostics stream.
   - JSON diagnostics emission continues to parse each surfaced diagnostic with
     `ParseDiagSortKey(...)`.
3. CLI diagnostics publication remains wired:
   - `driver/objc3_objc3_path.cpp` calls `WriteDiagnosticsArtifacts(...)` with
     both `artifacts.stage_diagnostics` and
     `artifacts.post_pipeline_diagnostics`.
4. C API diagnostics publication remains wired:
   - `libobjc3c_frontend/frontend_anchor.cpp` seeds emit-stage diagnostics from
     `product.artifact_bundle.post_pipeline_diagnostics`.
   - C API diagnostics JSON output remains based on
     `BuildDiagnosticsJson(product.artifact_bundle.diagnostics)`.
   - Emit-stage summary continues to include accumulated `emit_diagnostics`.
5. Architecture anchor remains authoritative in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py`
- `python -m pytest tests/tooling/test_check_m243_c001_lowering_runtime_diagnostics_surfacing_contract.py -q`
- `npm run check:objc3c:m243-c001-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-C001/lowering_runtime_diagnostics_surfacing_contract_summary.json`
