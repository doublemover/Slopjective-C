# M226 Frontend Build and Invocation Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1`
Status: Accepted
Scope: Build/invocation core feature expansion guardrails for native frontend entrypoints.

## Objective

Expand lane-D build/invocation guardrails with a deterministic core-feature artifact
that binds:

- the D002 modular scaffold contract surface,
- the D003 invocation lock-manifest surface, and
- invocation backend-routing/cache feature constraints

into a single fail-closed validation surface consumed by the compile wrapper.

## Required Invariants

1. Build script emits a D004 core-feature artifact:
   - `tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json`
2. D004 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1`
3. D004 artifact declares dependency contract ids:
   - `objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1`
   - `objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1`
4. D004 artifact declares required module set and backend routing capability set:
   - required modules include `driver`, `diagnostics-io`, `ir`, `lex-parse`, `frontend-api`, `lowering`, `pipeline`, `sema`
   - allowed IR object backends include `clang` and `llvm-direct`
5. Invocation wrapper validates D004 artifact existence/schema/contract id/dependency contracts
   and required feature fields before compiler execution.
6. Invocation wrapper validates compile-request feature compatibility:
   - requested `--objc3-ir-object-backend` must be declared in D004 allowed backends
   - `--objc3-route-backend-from-capabilities` requires `--llvm-capabilities-summary`
7. Invocation wrapper enforces D004 checks in both no-cache and cache-aware compile paths.

## Validation

- `python scripts/check_m226_d004_frontend_build_invocation_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d004_frontend_build_invocation_core_feature_expansion_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D004/smoke_out --emit-prefix module`

## Evidence Path

- `tmp/reports/m226/M226-D004/frontend_build_invocation_core_feature_expansion_summary.json`
