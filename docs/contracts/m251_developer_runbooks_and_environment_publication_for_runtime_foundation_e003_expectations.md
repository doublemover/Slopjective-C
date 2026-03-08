# M251 Developer Runbooks and Environment Publication for Runtime Foundation Expectations (E003)

Contract ID: `objc3c-runtime-foundation-developer-runbook-environment-publication/m251-e003-v1`
Status: Accepted
Issue: `#7070`
Dependencies: `M251-E002`
Scope: M251 lane-E developer runbook publication for the now-runnable runtime
foundation path so operator-facing build, inspection, and smoke commands remain
explicit, deterministic, and aligned with the real local toolchain.

## Objective

Fail closed unless the repository publishes one canonical runbook for the M251
runtime-foundation tranche and that runbook matches the actual commands needed
from a fresh clone on this machine.

## Required runbook surface

- Canonical runbook path:
  - `docs/runbooks/m251_runtime_foundation_developer_runbook.md`
- The runbook must publish the exact commands for:
  - `npm run build:objc3c-native`
  - native object emission through `.\artifacts\bin\objc3c-native.exe`
  - section inspection through `llvm-readobj.exe`
  - retained-symbol inspection through `llvm-objdump.exe`
  - runtime-linked smoke replay through
    `scripts/check_objc3c_native_execution_smoke.ps1`
- The runbook must publish stable evidence paths for:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/lib/objc3_runtime.lib`
  - `tmp/artifacts/compilation/objc3c-native/m251/runbook/object_inspection/module.manifest.json`
  - `tmp/artifacts/compilation/objc3c-native/m251/runbook/object_inspection/module.obj`
  - `tmp/artifacts/objc3c-native/execution-smoke/m251_e003_runtime_foundation_runbook_smoke/summary.json`

## Environment publication rules

- Tool prerequisites must remain explicit for this machine:
  - `C:\Program Files\PowerShell\7\pwsh.exe`
  - `C:\Program Files\LLVM\bin\llc.exe`
  - `C:\Program Files\LLVM\bin\llvm-readobj.exe`
  - `C:\Program Files\LLVM\bin\llvm-objdump.exe`
- The runbook must state that the documented commands are intended to be run
  from the repository root of a fresh clone on this machine.
- The runbook must keep the runtime shim framed as test-only evidence and must
  not reframe it as the canonical runtime archive.

## Dependency and anchor continuity

- `M251-E002` remains the immediate dependency and its summary must continue to
  report `ok: true`.
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m251/m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation_packet.md`
  - `scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`
  - `tests/tooling/test_check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`
- Documentation anchors must remain explicit in:
  - `docs/objc3c-native.md`
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `tests/tooling/runtime/README.md`

## Validation

- `python scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py`
- `python -m pytest tests/tooling/test_check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py -q`
- `npm run check:objc3c:m251-e003-lane-e-readiness`

## Evidence Path

- `tmp/reports/m251/M251-E003/runtime_foundation_developer_runbook_summary.json`
