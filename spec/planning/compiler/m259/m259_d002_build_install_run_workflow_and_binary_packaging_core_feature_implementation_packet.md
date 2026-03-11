# M259-D002 Build/install/run workflow and binary packaging Packet

Packet: `M259-D002`
Issue: `#7215`
Dependencies: `M259-D001`

## Scope

Implement the documented workflow and packaging surfaces needed to build and run
canonical object-model programs without ad hoc local steps.

## Acceptance

- Implement build, install, run workflow and binary packaging as a real
  compiler/runtime capability rather than a manifest-only summary.
- Stage a runnable toolchain package under `tmp/` that preserves the repo-
  relative layout expected by the current compile wrapper, runtime launch
  contract, execution smoke script, and execution replay proof script.
- Publish `artifacts/package/objc3c-runnable-toolchain-package.json` with the
  contract id `objc3c-runnable-build-install-run-package/m259-d002-v1`.
- Include `artifacts/bin/objc3c-frontend-c-api-runner.exe` in the staged bundle
  because the invocation-lock contract still validates both binary targets.
- Keep code/spec anchors explicit and deterministic across:
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `package.json`
- Prove the happy path by packaging the toolchain and then running the packaged:
  - compile wrapper
  - execution smoke script
  - execution replay proof script
- Validation evidence lands under
  `tmp/reports/m259/M259-D002/build_install_run_workflow_and_binary_packaging_summary.json`.

## Truthful boundary

- local staged package root only
- no system install claim
- no cross-platform packaging claim
- no toolchain auto-provisioning claim

## Next Issue

`M259-D003`
