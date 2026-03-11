# M226 Frontend Build and Invocation Diagnostics Hardening Expectations (D007)

Contract ID: `objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1`
Status: Accepted
Scope: Build/invocation diagnostics hardening guardrails for native frontend entrypoints.

## Objective

Add a deterministic D007 diagnostics-hardening artifact and enforce fail-closed,
explicit wrapper diagnostics for missing and empty values across wrapper-level
and compile-facing argument forms.

## Required Invariants

1. Build script emits an explicit D007 diagnostics-hardening artifact:
   - `tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json`
2. D007 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1`
3. D007 artifact dependencies include:
   - `objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1`
   - `objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1`
4. D007 artifact declares required wrapper diagnostics hardening messages for:
   - duplicate/invalid `--use-cache`
   - duplicate/missing/empty `--out-dir`
   - missing/empty `--emit-prefix`
   - missing/empty `--clang`
5. Wrapper validates D007 artifact in both no-cache and cache-aware paths.
6. Wrapper enforces explicit fail-closed diagnostics for both paired and
   equals-form value handling:
   - `--out-dir`
   - `--emit-prefix`
   - `--clang`
7. Wrapper resolves the D007 artifact path through repo-bound resolution and
   includes it in required cache-path artifact checks.

## Validation

- `python scripts/check_m226_d007_frontend_build_invocation_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d007_frontend_build_invocation_diagnostics_hardening_contract.py -q`
- `npm run build:objc3c-native:full`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D007/smoke_out --emit-prefix module`

## Evidence Path

- `tmp/reports/m226/M226-D007/frontend_build_invocation_diagnostics_hardening_summary.json`

