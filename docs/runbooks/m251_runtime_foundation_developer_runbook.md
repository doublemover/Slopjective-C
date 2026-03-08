# M251 Runtime Foundation Developer Runbook

## Scope

This runbook publishes the exact build, object-inspection, and execution-smoke
commands needed to exercise the current M251 runtime-foundation tranche from a
fresh clone on this machine.

Run every command from the repository root in PowerShell 7.

## Required local tools

- `C:\Program Files\PowerShell\7\pwsh.exe`
- `C:\Program Files\LLVM\bin\clang++.exe`
- `C:\Program Files\LLVM\bin\llvm-lib.exe`
- `C:\Program Files\LLVM\bin\llc.exe`
- `C:\Program Files\LLVM\bin\llvm-readobj.exe`
- `C:\Program Files\LLVM\bin\llvm-objdump.exe`
- `python` with `pytest`
- `node`
- `npm`

The runtime shim in `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains
test-only evidence. The canonical runtime archive for this tranche is
`artifacts/lib/objc3_runtime.lib`.

## Build the native compiler and runtime archive

```powershell
npm run build:objc3c-native
```

Expected build artifacts:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`

## Emit an inspectable runtime-metadata object

```powershell
& .\artifacts\bin\objc3c-native.exe .\tests\tooling\fixtures\native\m251_runtime_metadata_object_inspection_zero_descriptor.objc3 --out-dir .\tmp\artifacts\compilation\objc3c-native\m251\runbook\object_inspection --emit-prefix module --llc "C:\Program Files\LLVM\bin\llc.exe"
```

Expected emission evidence:

- `tmp/artifacts/compilation/objc3c-native/m251/runbook/object_inspection/module.manifest.json`
- `tmp/artifacts/compilation/objc3c-native/m251/runbook/object_inspection/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m251/runbook/object_inspection/module.obj`

## Inspect runtime-metadata sections and retained symbols

```powershell
& "C:\Program Files\LLVM\bin\llvm-readobj.exe" --sections .\tmp\artifacts\compilation\objc3c-native\m251\runbook\object_inspection\module.obj
& "C:\Program Files\LLVM\bin\llvm-objdump.exe" --syms .\tmp\artifacts\compilation\objc3c-native\m251\runbook\object_inspection\module.obj
```

The emitted object should retain:

- `objc3.runtime.image_info`
- `objc3.runtime.class_descriptors`
- `objc3.runtime.protocol_descriptors`
- `objc3.runtime.category_descriptors`
- `objc3.runtime.property_descriptors`
- `objc3.runtime.ivar_descriptors`
- `__objc3_image_info`
- `__objc3_sec_class_descriptors`
- `__objc3_sec_protocol_descriptors`
- `__objc3_sec_category_descriptors`
- `__objc3_sec_property_descriptors`
- `__objc3_sec_ivar_descriptors`

## Replay runtime-linked execution smoke

```powershell
$env:OBJC3C_NATIVE_EXECUTION_RUN_ID='m251_e003_runtime_foundation_runbook_smoke'; $env:OBJC3C_NATIVE_EXECUTION_LLC_PATH='C:\Program Files\LLVM\bin\llc.exe'; & "C:\Program Files\PowerShell\7\pwsh.exe" -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_native_execution_smoke.ps1
```

Expected smoke evidence:

- `tmp/artifacts/objc3c-native/execution-smoke/m251_e003_runtime_foundation_runbook_smoke/summary.json`
- summary `status = PASS`
- summary `runtime_library = artifacts/lib/objc3_runtime.lib`

## Dependency gate reference

`M251-E003` depends on the integrated `M251-E002` gate. The canonical E002
evidence remains:

- `tmp/reports/m251/M251-E002/cross_lane_runtime_foundation_gate_summary.json`

## Canonical lane-E closeout command

```powershell
npm run check:objc3c:m251-e003-lane-e-readiness
```

Expected lane-E evidence:

- `tmp/reports/m251/M251-E003/runtime_foundation_developer_runbook_summary.json`
