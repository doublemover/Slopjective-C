# M267 Error-Model Conformance Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-error-model-conformance-gate/m267-e001-v1`

Issue: `#7280`

## Objective

Freeze one truthful lane-E gate over the already-landed Part 6 error-model slice.
The gate must prove runnable throws/result-like/NSError bridging behavior using
upstream M267 evidence, not a synthetic runtime feature.

## Required implementation

1. Add the planning packet, deterministic checker, tooling test, and direct
   lane-E readiness runner:
   - `scripts/check_m267_e001_error_model_conformance_gate_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m267_e001_error_model_conformance_gate_contract_and_architecture_freeze.py`
   - `scripts/run_m267_e001_lane_e_readiness.py`
2. Add explicit `M267-E001` anchor text to:
   - `docs/objc3c-native/src/30-semantics.md`
   - `docs/objc3c-native.md`
   - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
   - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
   - `package.json`
3. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m267/M267-A002/error_bridge_marker_surface_summary.json`
   - `tmp/reports/m267/M267-B003/bridging_legality_summary.json`
   - `tmp/reports/m267/M267-C003/result_and_bridging_artifact_replay_completion_summary.json`
   - `tmp/reports/m267/M267-D003/cross_module_error_surface_preservation_summary.json`
4. The checker must reject drift if any upstream summary disappears or stops
   reporting successful coverage.
5. The checker must compile
   `tests/tooling/fixtures/native/m267_c003_part6_artifact_replay_producer.objc3`
   and verify the emitted manifest and replay sidecar still publish the current
   runnable error-model replay keys and ready-for-runtime-execution surface.
6. `package.json` must wire:
   - `check:objc3c:m267-e001-error-model-conformance-gate`
   - `test:tooling:m267-e001-error-model-conformance-gate`
   - `check:objc3c:m267-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M267-E002`.

## Canonical models

- Evidence model:
  `b003-c003-d003-summary-chain-plus-provider-replay`
- Execution gate model:
  `lane-e-frozen-error-model-gate-consumes-source-sema-lowering-runtime-and-cross-module-proofs`
- Failure model:
  `fail-closed-on-error-model-evidence-drift`

## Non-goals

- No new language-surface implementation beyond the already landed M267
  surfaces and D003 evidence.
- No matrix expansion yet; that belongs to `M267-E002`.
- No broader Part 6 claims beyond the currently supported throws/result-like/
  NSError bridging slice.

## Evidence

- `tmp/reports/m267/M267-E001/error_model_conformance_gate_summary.json`
