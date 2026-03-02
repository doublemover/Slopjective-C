# M226 Frontend Build and Invocation Manifest Guard Expectations (D003)

Contract ID: `objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1`
Status: Accepted
Scope: Frontend build/invocation lock-manifest guard for native compiler entrypoints.

## Objective

Enforce a fail-closed handoff between build and invocation by requiring a
deterministic lock manifest that binds produced frontend binaries and scaffold
metadata to SHA-256 fingerprints.

## Required Invariants

1. Build script emits lock artifact:
   - `tmp/artifacts/objc3c-native/frontend_invocation_lock.json`
2. Lock artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1`
3. Lock artifact includes required binary entries with stable names/paths:
   - `objc3c-native` -> `artifacts/bin/objc3c-native.exe`
   - `objc3c-frontend-c-api-runner` -> `artifacts/bin/objc3c-frontend-c-api-runner.exe`
4. Lock artifact includes `sha256` for each required binary and scaffold file.
5. Invocation wrapper validates lock artifact existence, JSON schema, contract id,
   scaffold contract id, required binary path set, and hash integrity before
   compiler execution.
6. Invocation wrapper fail-closes with exit code `2` on lock-manifest drift.

## Validation

- `python scripts/check_m226_d003_frontend_build_invocation_manifest_guard_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d003_frontend_build_invocation_manifest_guard_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1`

## Evidence Path

- `tmp/reports/m226/M226-D003/frontend_build_invocation_manifest_guard_summary.json`
