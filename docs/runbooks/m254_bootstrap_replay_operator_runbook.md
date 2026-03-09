# M254 Bootstrap Replay Operator Runbook

## Scope

This runbook publishes the exact commands needed to reproduce the current
startup-registration/bootstrap closeout slice for `M254` from the repository
root in PowerShell 7.

## Required local tools

- `C:\Program Files\PowerShell\7\pwsh.exe`
- `C:\Program Files\LLVM\bin\llc.exe`
- `python` with `pytest`
- `node`
- `npm`

The canonical native artifacts for this tranche are:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`

## Build the native compiler and runtime archive

```powershell
npm run build:objc3c-native
```

## Re-run the upstream startup-registration gate

```powershell
npm run check:objc3c:m254-e001-lane-e-readiness
```

Expected upstream gate evidence:

- `tmp/reports/m254/M254-E001/startup_registration_gate_summary.json`

## Run the live bootstrap replay/operator smoke closeout

```powershell
$env:OBJC3C_NATIVE_EXECUTION_RUN_ID='m254_e002_bootstrap_runbook_closeout'; $env:OBJC3C_NATIVE_EXECUTION_LLC_PATH='C:\Program Files\LLVM\bin\llc.exe'; & 'C:\Program Files\PowerShell\7\pwsh.exe' -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_native_execution_smoke.ps1
```

Expected smoke evidence:

- `tmp/artifacts/objc3c-native/execution-smoke/m254_e002_bootstrap_runbook_closeout/summary.json`
- summary `status = PASS`
- summary `compile_wrapper = scripts/objc3c_native_compile.ps1`
- summary `runtime_launch_contract_script = scripts/objc3c_runtime_launch_contract.ps1`
- summary `runtime_library = artifacts/lib/objc3_runtime.lib`

## Canonical lane-E closeout command

```powershell
npm run check:objc3c:m254-e002-lane-e-readiness
```

Expected lane-E evidence:

- `tmp/reports/m254/M254-E002/replay_bootstrap_runbook_closeout_summary.json`
