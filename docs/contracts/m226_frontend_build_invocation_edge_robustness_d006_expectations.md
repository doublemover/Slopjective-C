# M226 Frontend Build and Invocation Edge Robustness Expectations (D006)

Contract ID: `objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1`
Status: Accepted
Scope: Build/invocation edge-robustness guardrails for native frontend entrypoints.

## Objective

Add a deterministic D006 robustness artifact and enforce wrapper fail-closed
guardrails for duplicate/single-value flags and empty equals-form values across
wrapper and compile-facing invocation paths.

## Required Invariants

1. Build script emits an explicit D006 edge-robustness artifact:
   - `tmp/artifacts/objc3c-native/frontend_edge_robustness.json`
2. D006 artifact contract id is fixed:
   - `objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1`
3. D006 artifact dependencies include:
   - `objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1`
   - `objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1`
4. D006 artifact declares wrapper robustness guardrails:
   - `wrapper_single_value_flags` includes `--use-cache` and `--out-dir`
   - `compile_single_value_flags` includes `--objc3-ir-object-backend`,
     `--llvm-capabilities-summary`, and
     `--objc3-route-backend-from-capabilities`
   - `reject_empty_equals_value_flags` includes wrapper/compile flags that must
     fail-closed when passed with empty equals-form values.
5. Wrapper validates D006 artifact in both no-cache and cache-aware paths.
6. Wrapper enforces fail-closed robustness rules:
   - wrapper single-value flags (`--use-cache`, `--out-dir`) can appear at most once
   - compile routing flag (`--objc3-route-backend-from-capabilities`) can appear at most once
   - empty equals-form values fail-closed for `--emit-prefix` and `--clang`
     (plus related D005 compile and wrapper equals-form guards)
7. Wrapper resolves the D006 artifact path through repo-bound resolution and
   includes it in required cache-path artifact checks.

## Validation

- `python scripts/check_m226_d006_frontend_build_invocation_edge_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d006_frontend_build_invocation_edge_robustness_contract.py -q`
- `npm run build:objc3c-native`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-D006/smoke_out --emit-prefix module`

## Evidence Path

- `tmp/reports/m226/M226-D006/frontend_build_invocation_edge_robustness_summary.json`
