# M226 Frontend Build and Invocation Integration Closeout Expectations (D011)

Contract ID: `objc3c-frontend-build-invocation-integration-closeout/m226-d011-v1`
Status: Accepted
Scope: Build/invocation integration closeout and gate sign-off coverage.

## Objective

Require a deterministic closeout gate artifact and wrapper assertion layer that
signs off D010 corpus coverage, D009 profile conformance, and fail-closed
determinism requirements before compile invocation.

## Required Invariants

1. Build script emits a D011 integration closeout artifact:
   - `tmp/artifacts/objc3c-native/frontend_integration_closeout.json`
2. D011 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-integration-closeout/m226-d011-v1`
3. D011 artifact depends on:
   - `objc3c-frontend-build-invocation-conformance-corpus/m226-d010-v1`
   - `objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1`
   - `objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1`
4. Closeout gate metadata is fail-closed and deterministic:
   - `build_integration_gate_signoff = true`
   - `invocation_profile_gate_signoff = true`
   - `corpus_coverage_gate_signoff = true`
   - `deterministic_fail_closed_exit_code = 2`
5. Wrapper validates D011 in both no-cache and cache-aware paths.

## Validation

- `python scripts/check_m226_d011_frontend_build_invocation_integration_closeout_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d011_frontend_build_invocation_integration_closeout_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-D011/frontend_build_invocation_integration_closeout_summary.json`
