# M254 Startup Registration Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-runtime-startup-registration-gate/m254-e001-v1`

## Objective

Freeze one fail-closed lane-E gate over the live startup-registration/bootstrap evidence chain before `M254-E002` cross-lane closeout.

## Required implementation

1. Add a canonical expectations document for the startup-registration gate.
2. Add this packet, a deterministic checker, and tooling tests:
   - `scripts/check_m254_e001_startup_registration_gate.py`
   - `tests/tooling/test_check_m254_e001_startup_registration_gate.py`
3. Add `M254-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `tests/tooling/runtime/README.md`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `native/objc3c/src/io/objc3_process.cpp`
4. Keep the gate fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m254/M254-A002/registration_manifests_and_constructor_root_ownership_summary.json`
   - `tmp/reports/m254/M254-B002/bootstrap_semantics_summary.json`
   - `tmp/reports/m254/M254-C003/registration_table_image_local_initialization_summary.json`
   - `tmp/reports/m254/M254-D003/deterministic_reset_replay_summary.json`
   - `tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops reporting `ok: true`, stops reporting dynamic probes, or drops the invariants that prove startup registration, realization, reset, replay, and operator launch behavior.
6. `package.json` must wire:
   - `check:objc3c:m254-e001-startup-registration-gate`
   - `test:tooling:m254-e001-startup-registration-gate`
   - `check:objc3c:m254-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M254-E002`.

## Non-goals

- No new startup-registration runtime feature implementation.
- No new emitted bootstrap metadata families.
- No new launch-path heuristics.
- No new probe corpus beyond the already-landed upstream proofs.

## Evidence

- `tmp/reports/m254/M254-E001/startup_registration_gate_summary.json`
