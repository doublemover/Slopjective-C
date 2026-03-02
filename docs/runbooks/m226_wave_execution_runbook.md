# M226 Wave Execution Runbook

## Scope

This runbook tracks current M226 lane sync coverage for:

- `objc3c-parser-performance-quality-guardrails-contract/m226-a011-v1`
- `objc3c-parser-sema-core-handoff-contract/m226-b003-v1`
- `objc3c-parse-lowering-core-readiness-contract/m226-c003-v1`
- `objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1`
- `objc3c-lane-e-integration-gate-core-evidence-contract/m226-e003-v1`
- `objc3c-cross-lane-integration-sync-contract/m226-a012-v1`

## Canonical Validation Sequence

1. Build native binaries:
   - `npm run build:objc3c-native`
2. Run smoke compile through invocation wrapper:
   - `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m226-wave-smoke`
3. Validate cross-lane sync gate:
   - `python scripts/check_m226_a012_cross_lane_integration_sync_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a012_cross_lane_integration_sync_contract.py -q`

## Evidence

Persist wave evidence under:

- `tmp/reports/m226/`
