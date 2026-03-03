# M227 Typed Sema-to-Lowering Advanced Core Workpack (Shard 1) Expectations (C015)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-core-shard1/m227-c015-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced core workpack (shard 1) on top of C014 release-candidate/replay dry-run.

## Objective

Execute issue `#5135` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-core-shard1 consistency/readiness invariants,
with deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C014`
- `M227-C014` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_release_candidate_replay_dry_run_c014_expectations.md`
  - `scripts/check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
  - `spec/planning/compiler/m227/m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-core-shard1 fields:
   - `typed_advanced_core_shard1_consistent`
   - `typed_advanced_core_shard1_ready`
   - `typed_advanced_core_shard1_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-core-shard1 fields:
   - `typed_sema_advanced_core_shard1_consistent`
   - `typed_sema_advanced_core_shard1_ready`
   - `typed_sema_advanced_core_shard1_key`
3. Parse/lowering readiness fails closed when typed advanced-core-shard1
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c015-typed-sema-to-lowering-advanced-core-shard1-contract`
  - `test:tooling:m227-c015-typed-sema-to-lowering-advanced-core-shard1-contract`
  - `check:objc3c:m227-c015-lane-c-readiness`

## Validation

- `python scripts/check_m227_c014_typed_sema_to_lowering_release_candidate_replay_dry_run_contract.py`
- `python scripts/check_m227_c015_typed_sema_to_lowering_advanced_core_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c015_typed_sema_to_lowering_advanced_core_shard1_contract.py -q`
- `npm run check:objc3c:m227-c015-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C015/typed_sema_to_lowering_advanced_core_shard1_contract_summary.json`
