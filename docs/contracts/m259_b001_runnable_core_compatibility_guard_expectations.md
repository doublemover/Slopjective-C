# M259 B001 Runnable Core Compatibility Guard Expectations

Contract ID: `objc3c-runnable-core-compatibility-guard/m259-b001-v1`

Issue: `#7210`

## Objective

Freeze the sema-owned boundary that distinguishes the current runnable
Objective-C 3 native core from later advanced surfaces that remain source-only,
metadata-only, or fail-closed unsupported.

## Required implementation

1. Add the issue-local contract/checker/test/readiness assets:
   - `docs/contracts/m259_b001_runnable_core_compatibility_guard_expectations.md`
   - `spec/planning/compiler/m259/m259_b001_runnable_core_compatibility_guard_packet.md`
   - `scripts/check_m259_b001_runnable_core_compatibility_guard.py`
   - `tests/tooling/test_check_m259_b001_runnable_core_compatibility_guard.py`
   - `scripts/run_m259_b001_lane_b_readiness.py`
2. Freeze the canonical runnable-core guard in:
   - `native/objc3c/src/token/objc3_token_contract.h`
   - `native/objc3c/src/sema/objc3_sema_contract.h`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
3. Publish explicit docs/spec/package/script anchors in:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
4. Preserve the truthful boundary:
   - `M259-A002` remains the current runnable live proof floor
   - compatibility mode and migration assist remain live semantic selections
   - migration-assist legacy-literal diagnostics remain fail-closed `O3S216`
   - currently landed unsupported-feature diagnostics remain fail-closed for
     `throws`, block literals, ARC ownership qualifiers, and `@autoreleasepool`
   - later advanced surfaces do not get promoted to runnable support by docs,
     package scripts, smoke coverage, or replay coverage in this issue
5. The contract must explicitly hand off to `M259-B002`.

## Canonical models

- Guard model:
  `runnable-core-distinguishes-live-runtime-backed-core-from-source-only-or-fail-closed-advanced-surfaces`
- Evidence model:
  `a002-live-runnable-core-proof-plus-sema-compatibility-selection-and-unsupported-claim-boundary`
- Failure model:
  `fail-closed-on-runnable-core-compatibility-guard-drift-or-overclaimed-advanced-surface-support`

## Evidence

- `tmp/reports/m259/M259-B001/runnable_core_compatibility_guard_summary.json`
