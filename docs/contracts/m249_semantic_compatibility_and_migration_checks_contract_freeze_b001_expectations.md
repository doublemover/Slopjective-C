# M249 Semantic Compatibility and Migration Checks Contract Freeze Expectations (B001)

Contract ID: `objc3c-semantic-compatibility-and-migration-checks-freeze/m249-b001-v1`
Status: Accepted
Scope: Lane-B semantic compatibility and migration checks across sema pass-flow and parse/lowering readiness surfaces.

## Objective

Freeze lane-B semantic compatibility and migration checks so canonical/legacy
compatibility mode behavior, migration assist gating, and compatibility-handoff
determinism remain fail-closed as contract and architecture anchors.

## Required Invariants

1. `sema/objc3_sema_pass_manager_contract.h` remains the canonical semantic
   compatibility/migration contract anchor:
   - `Objc3SemaCompatibilityMode` keeps canonical/legacy values.
   - `Objc3SemaMigrationHints` and pass-flow fields remain explicit.
2. `sema/objc3_sema_pass_flow_scaffold.cpp` keeps fail-closed compatibility
   handoff synthesis:
   - migration assist requires canonical compatibility mode.
   - compatibility mode is restricted to canonical/legacy.
   - pass-flow fingerprint and deterministic handoff key include compatibility
     and migration evidence.
3. `pipeline/objc3_parse_lowering_readiness_surface.h` keeps compatibility
   handoff key and readiness gates deterministic:
   - compatibility key encodes compatibility mode, migration assist, and legacy
     literal counters.
   - readiness fails closed when compatibility handoff is inconsistent.
4. Shared architecture/spec/metadata anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
5. `package.json` keeps dedicated B001 checker/test/readiness scripts and
   milestone optimization input hooks.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-b001-semantic-compatibility-migration-checks-contract`.
- `package.json` includes
  `test:tooling:m249-b001-semantic-compatibility-migration-checks-contract`.
- `package.json` includes `check:objc3c:m249-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m249_b001_semantic_compatibility_and_migration_checks_contract.py`
- `python -m pytest tests/tooling/test_check_m249_b001_semantic_compatibility_and_migration_checks_contract.py -q`
- `npm run check:objc3c:m249-b001-lane-b-readiness`
- `npm run test:objc3c:sema-pass-manager-diagnostics-bus`
- `npm run test:objc3c:lowering-regression`

## Evidence Path

- `tmp/reports/m249/M249-B001/semantic_compatibility_and_migration_checks_contract_summary.json`
