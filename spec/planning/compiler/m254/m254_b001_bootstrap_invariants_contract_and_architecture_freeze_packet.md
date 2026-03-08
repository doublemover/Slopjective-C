# M254-B001 Bootstrap Invariants Contract and Architecture Freeze Packet

Packet: `M254-B001`
Milestone: `M254`
Lane: `B`
Freeze date: `2026-03-08`
Dependencies: none

## Purpose

Freeze the canonical startup/bootstrap semantic packet so later implementation
work extends one deterministic contract over duplicate registration,
realization order, failure mode, and image-local initialization.

## Scope Anchors

- Contract:
  `docs/contracts/m254_bootstrap_invariants_contract_and_architecture_freeze_b001_expectations.md`
- Checker:
  `scripts/check_m254_b001_bootstrap_invariants_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m254_b001_bootstrap_invariants_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m254-b001-bootstrap-invariants-contract`
  - `test:tooling:m254-b001-bootstrap-invariants-contract`
  - `check:objc3c:m254-b001-lane-b-readiness`
- Code anchors:
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `tests/tooling/runtime/README.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Fail-Closed Boundary

- Contract id `objc3c-runtime-startup-bootstrap-invariants/m254-b001-v1`
- Manifest surface
  `frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_invariants`
- Upstream manifest contract id
  `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- Frozen semantic policies:
  - duplicate registration `fail-closed-by-translation-unit-identity-key`
  - realization order `constructor-root-then-registration-manifest-order`
  - failure mode `abort-before-user-main-no-partial-registration-commit`
  - image-local initialization
    `runtime-owned-image-local-registration-state`
  - constructor-root uniqueness
    `one-startup-root-per-translation-unit-identity`
  - constructor-root consumption model
    `startup-root-consumes-registration-manifest`
  - startup execution mode `deferred-until-m254-c001`
- `M254-B002` must preserve this freeze while implementing live bootstrap
  semantics.

## Non-Goals

- no live startup execution yet
- no duplicate-registration runtime enforcement yet
- no image-local realization yet
- no partial-registration commit behavior

## Gate Commands

- `python scripts/check_m254_b001_bootstrap_invariants_contract.py`
- `python -m pytest tests/tooling/test_check_m254_b001_bootstrap_invariants_contract.py -q`
- `npm run check:objc3c:m254-b001-lane-b-readiness`

## Evidence Output

- `tmp/reports/m254/M254-B001/bootstrap_invariants_contract_summary.json`
