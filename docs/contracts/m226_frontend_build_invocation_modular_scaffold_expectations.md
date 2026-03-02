# M226 Frontend Build and Invocation Modular Scaffold Expectations (D002)

Contract ID: `objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1`
Status: Accepted
Scope: Build/invocation modular split scaffold for native frontend integration.

## Objective

Establish deterministic module-oriented scaffold metadata for native frontend
build/invocation paths so D-lane integration can validate structure without
relying on implicit source-list coupling.

## Required Invariants

1. Build script groups frontend shared sources by explicit modules.
2. Build script emits scaffold artifact:
   - `tmp/artifacts/objc3c-native/frontend_modular_scaffold.json`
3. Scaffold artifact includes stable contract id:
   - `objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1`
4. Invocation wrapper validates scaffold presence and schema-level invariants
   before invoking compiler execution.
5. Required module names are enforced:
   - `driver`
   - `diagnostics-io`
   - `ir`
   - `lex-parse`
   - `frontend-api`
   - `lowering`
   - `pipeline`
   - `sema`

## Validation

- `python scripts/check_m226_d002_frontend_build_invocation_modular_scaffold_contract.py`
- `python -m pytest tests/tooling/test_check_m226_d002_frontend_build_invocation_modular_scaffold_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-D002/frontend_build_invocation_modular_scaffold_summary.json`
