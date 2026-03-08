# M251-E003 Developer Runbooks and Environment Publication for Runtime Foundation Packet

Packet: `M251-E003`
Milestone: `M251`
Lane: `E`
Issue: `#7070`
Dependencies: `M251-E002`

## Objective

Publish the canonical operator-facing runtime-foundation runbook and fail
closed unless the exact documented build, object-inspection, and smoke commands
match the current local toolchain path on this machine.

## Scope anchors

- Contract:
  `docs/contracts/m251_developer_runbooks_and_environment_publication_for_runtime_foundation_e003_expectations.md`
- Runbook:
  `docs/runbooks/m251_runtime_foundation_developer_runbook.md`
- Checker:
  `scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`
- Tooling tests:
  `tests/tooling/test_check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`
- Dependency continuity from `M251-E002` remains fail-closed through:
  - `docs/contracts/m251_cross_lane_runtime_foundation_gate_and_bootstrap_proof_e002_expectations.md`
  - `spec/planning/compiler/m251/m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof_packet.md`
  - `scripts/check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`
  - `tests/tooling/test_check_m251_e002_cross_lane_runtime_foundation_gate_and_bootstrap_proof.py`

## Required implementation

1. Publish one canonical runbook under
   `docs/runbooks/m251_runtime_foundation_developer_runbook.md`.
2. Publish the exact PowerShell/LLVM commands for:
   - `npm run build:objc3c-native`
   - native object emission through `.\artifacts\bin\objc3c-native.exe`
   - object inspection through `llvm-readobj.exe` and `llvm-objdump.exe`
   - runtime-linked smoke replay through
     `scripts/check_objc3c_native_execution_smoke.ps1`
3. Publish the required evidence paths for the build artifacts, emitted object,
   manifest, and smoke summary.
4. Keep local-tool prerequisites explicit for this machine:
   - `C:\Program Files\PowerShell\7\pwsh.exe`
   - `C:\Program Files\LLVM\bin\llc.exe`
   - `C:\Program Files\LLVM\bin\llvm-readobj.exe`
   - `C:\Program Files\LLVM\bin\llvm-objdump.exe`
5. Add lane-E E003 anchors to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `tests/tooling/runtime/README.md`
   - `package.json`
6. Require the checker to load the canonical `M251-E002` summary and fail
   closed if it is missing or no longer reports `ok: true`.
7. Require the checker to replay the documented build/object-inspection/smoke
   path and fail closed if the runbook no longer matches actual local commands.

## Determinism and fail-closed rules

- `M251-E003` is a runbook/publication issue only; it does not add new runtime
  features.
- The checker must therefore fail closed if dependency continuity drifts, if
  required local tools disappear, if the documented commands change without the
  runbook changing, or if the documented command path stops producing the
  published evidence artifacts.
- The runtime shim remains explicit test-only evidence and must not be
  reframed as the canonical runtime archive.

## Validation

- `python scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`
- `python -m pytest tests/tooling/test_check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py -q`
- `npm run check:objc3c:m251-e003-lane-e-readiness`

## Evidence

- `tmp/reports/m251/M251-E003/runtime_foundation_developer_runbook_summary.json`
