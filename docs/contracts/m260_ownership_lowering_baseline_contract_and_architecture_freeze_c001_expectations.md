# M260 Ownership Lowering Baseline Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-ownership-lowering-baseline-freeze/m260-c001-v1`

Issue: `#7173`

## Objective

Freeze the truthful lane-C lowering boundary for runtime-backed object ownership.
Runtime-backed ownership metadata and storage legality are already live, but the
compiler still represents executable ownership behavior through legacy lowering
summary lanes instead of emitting live retain, release, autorelease,
autoreleasepool, or weak runtime hooks.

## Required implementation

1. Add the issue-local assets:
   - `docs/contracts/m260_ownership_lowering_baseline_contract_and_architecture_freeze_c001_expectations.md`
   - `spec/planning/compiler/m260/m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze_packet.md`
   - `scripts/check_m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m260_c001_ownership_lowering_baseline_contract_and_architecture_freeze.py`
   - `scripts/run_m260_c001_lane_c_readiness.py`
2. Add explicit anchors to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `package.json`
3. Freeze the truthful lowering boundary around:
   - ownership qualifier lowering remaining published through the legacy `m161` replay surface
   - retain/release lowering remaining published through the legacy `m162` replay surface
   - autoreleasepool lowering remaining published through the legacy `m163` replay surface
   - weak/unowned lowering remaining published through the legacy `m164` replay surface
   - runtime-backed ownership metadata and storage legality already being live from `M260-A002/B002`
4. The checker must prove the frozen boundary with one live compile of:
   - `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`
   It must fail closed unless the manifest and IR still show the canonical
   `m161`/`m162`/`m163`/`m164` lowering replay surfaces, the new
   `ownership_lowering_baseline` summary line, and the absence of emitted live
   ownership runtime hook calls.
5. `package.json` must wire:
   - `check:objc3c:m260-c001-ownership-lowering-baseline-contract`
   - `test:tooling:m260-c001-ownership-lowering-baseline-contract`
   - `check:objc3c:m260-c001-lane-c-readiness`
6. The contract must explicitly hand off to `M260-C002`.

## Canonical models

- Ownership qualifier model:
  `ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata`
- Runtime hook model:
  `retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission`
- Autoreleasepool model:
  `autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks`
- Failure model:
  `no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet`

## Non-goals

- No live retain/release/autorelease runtime hook emission yet.
- No live autoreleasepool push/pop emission yet.
- No live weak side-table runtime hook emission yet.
- No destruction-order lowering yet.

## Evidence

- `tmp/reports/m260/M260-C001/ownership_lowering_baseline_contract_summary.json`
