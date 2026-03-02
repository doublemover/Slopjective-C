# M226 Frontend Build and Invocation Recovery Determinism Hardening Expectations (D008)

Contract ID: `objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1`
Status: Accepted
Scope: Recovery and determinism hardening for wrapper cache replay and staged artifact contracts.

## Objective

Add a deterministic D008 recovery/determinism artifact and enforce fail-closed,
cache-entry metadata validation in the wrapper so corrupted cache state recovers
deterministically to cache-miss behavior.

## Required Invariants

1. Build script emits an explicit D008 recovery/determinism artifact:
   - `tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json`
2. D008 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1`
3. D008 artifact dependencies include:
   - `objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1`
   - `objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1`
4. D008 artifact pins cache entry contract and required files:
   - `objc3c-native-cache-entry/m226-d008-v1`
   - `files`, `exit_code.txt`, `ready.marker`, `metadata.json`
5. Wrapper validates D008 artifact in both no-cache and cache-aware paths.
6. Wrapper validates cache metadata deterministically before cache replay and
   emits deterministic recovery signals:
   - `cache_recovery=metadata_missing`
   - `cache_recovery=metadata_invalid`
   - `cache_recovery=metadata_contract_mismatch`
   - `cache_recovery=metadata_cache_key_mismatch`
   - `cache_recovery=metadata_exit_code_mismatch`
   - `cache_recovery=metadata_digest_mismatch`
   - `cache_recovery=restore_failed`
7. Cache entries are written with deterministic metadata and replay digest.

## Validation

- `python scripts/check_m226_d008_frontend_build_invocation_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d008_frontend_build_invocation_recovery_determinism_hardening_contract.py -q`
- `npm run build:objc3c-native`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D008/smoke_out --emit-prefix module`

## Evidence Path

- `tmp/reports/m226/M226-D008/frontend_build_invocation_recovery_determinism_hardening_summary.json`
