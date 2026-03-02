# M226 Frontend Build and Invocation Conformance Matrix Expectations (D009)

Contract ID: `objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1`
Status: Accepted
Scope: Concrete build/invocation conformance matrix for frontend wrapper profiles.

## Objective

Freeze an explicit conformance matrix that binds accepted/rejected invocation
profiles to deterministic, fail-closed wrapper behavior.

## Required Invariants

1. Build script emits a D009 conformance matrix artifact:
   - `tmp/artifacts/objc3c-native/frontend_conformance_matrix.json`
2. D009 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1`
3. D009 artifact depends on:
   - `objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1`
   - `objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1`
4. D009 matrix is concrete and row-based:
   - acceptance rows are keyed by `cache_mode|backend_mode|routing_mode|capability_summary_mode`
   - rejection rows pin deterministic diagnostics and fail-closed exit code `2`
5. Wrapper validates D009 in both no-cache and cache-aware paths.
6. Wrapper derives an invocation profile key and fail-closes unless a matching
   acceptance row exists.

## Conformance Matrix

| Case ID | Profile Key | Expected Result |
|---|---|---|
| `D009-C001` | `no-cache|default|manual|none` | Accept |
| `D009-C002` | `no-cache|default|manual|present` | Accept |
| `D009-C003` | `no-cache|default|capability-route|present` | Accept |
| `D009-C010` | `no-cache|llvm-direct|manual|none` | Accept |
| `D009-C018` | `cache-aware|llvm-direct|capability-route|present` | Accept |
| `D009-R001` | `any|any|capability-route|none` | Reject (`--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary`) |
| `D009-R004` | `any|any|any|path-parent-segment` | Reject (`--llvm-capabilities-summary must not contain '..' relative segments`) |

## Validation

- `python scripts/check_m226_d009_frontend_build_invocation_conformance_matrix_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d009_frontend_build_invocation_conformance_matrix_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-D009/frontend_build_invocation_conformance_matrix_summary.json`
