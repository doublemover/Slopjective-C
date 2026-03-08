# M254 Bootstrap Invariants Contract and Architecture Freeze Expectations (B001)

Contract ID: `objc3c-runtime-startup-bootstrap-invariants/m254-b001-v1`
Status: Accepted
Issue: `#7103`
Scope: M254 lane-B contract and architecture freeze for startup bootstrap
invariants.

## Objective

Freeze duplicate-registration, realization-order, failure-mode, and image-local
initialization semantics for later startup-registration/bootstrap work.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` remains the canonical declaration point
   for `Objc3RuntimeStartupBootstrapInvariantSummary`.
2. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical manifest
   publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_invariants`
   - flattened `runtime_startup_bootstrap_invariant_*` summary keys
3. The frozen packet stays explicitly tied to the live registration-manifest
   contract `objc3c-translation-unit-registration-manifest/m254-a002-v1`.
4. The frozen semantic boundary preserves:
   - duplicate-registration policy
     `fail-closed-by-translation-unit-identity-key`
   - realization-order policy
     `constructor-root-then-registration-manifest-order`
   - failure mode
     `abort-before-user-main-no-partial-registration-commit`
   - image-local initialization scope
     `runtime-owned-image-local-registration-state`
   - constructor-root uniqueness policy
     `one-startup-root-per-translation-unit-identity`
   - constructor-root consumption model
     `startup-root-consumes-registration-manifest`
5. `driver/objc3_objc3_path.cpp` and `io/objc3_process.cpp` remain explicit
   that the emitted registration manifest is the authoritative input boundary
   later bootstrap work must honor rather than bypass.
6. `tests/tooling/runtime/README.md` remains explicit that `M254-B001` is a
   freeze only and does not land live bootstrap execution yet.

## Non-Goals and Fail-Closed Rules

- `M254-B001` does not execute startup bootstrap.
- `M254-B001` does not enforce duplicate registration at runtime yet.
- `M254-B001` does not realize images or commit partial bootstrap state.
- `M254-B002` must preserve this boundary while implementing live semantics.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m254-b001-bootstrap-invariants-contract`.
- `package.json` includes
  `test:tooling:m254-b001-bootstrap-invariants-contract`.
- `package.json` includes `check:objc3c:m254-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m254_b001_bootstrap_invariants_contract.py`
- `python -m pytest tests/tooling/test_check_m254_b001_bootstrap_invariants_contract.py -q`
- `npm run check:objc3c:m254-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m254/M254-B001/bootstrap_invariants_contract_summary.json`
