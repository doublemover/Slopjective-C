# M226 Frontend Build and Invocation Edge-Case Compatibility Completion Expectations (D005)

Contract ID: `objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1`
Status: Accepted
Scope: Build/invocation edge-case and compatibility completion guardrails for native frontend entrypoints.

## Objective

Complete lane-D edge-case compatibility by introducing a dedicated D005 artifact
and enforcing deterministic wrapper normalization for backend aliasing,
equals-form flags, and repository-bound path safety.

## Required Invariants

1. Build script emits an explicit D005 edge-compat artifact:
   - `tmp/artifacts/objc3c-native/frontend_edge_compat.json`
2. D005 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1`
3. D005 artifact dependencies include:
   - `objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1`
   - `objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1`
4. Wrapper validates D005 artifact in both no-cache and cache-aware paths.
5. Wrapper enforces edge compatibility rules fail-closed:
   - canonical backend alias normalization for `--objc3-ir-object-backend`
   - supports `--flag=value` and paired `--flag value` for route/capability flags
   - single-value flags (`--objc3-ir-object-backend`, `--llvm-capabilities-summary`) may appear at most once
   - repository-bound path enforcement rejects `..` path escapes for key repo-relative artifacts
6. Wrapper emits normalized compile arguments after compatibility processing and executes native compiler with normalized args.

## Validation

- `python scripts/check_m226_d005_frontend_build_invocation_edge_case_compatibility_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d005_frontend_build_invocation_edge_case_compatibility_contract.py -q`
- `npm run build:objc3c-native`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D005/smoke_out --emit-prefix module --objc3-ir-object-backend=llvm_direct`

## Evidence Path

- `tmp/reports/m226/M226-D005/frontend_build_invocation_edge_case_compatibility_summary.json`
