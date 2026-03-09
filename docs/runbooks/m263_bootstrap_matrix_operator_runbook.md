# M263 Bootstrap Matrix Operator Runbook

This runbook exercises the current runnable bootstrap surface after the `M263-E001` gate is already green.

## 1. Reconfirm the integrated gate

```powershell
npm run check:objc3c:m263-e001-lane-e-readiness
```

## 2. Publish the runnable bootstrap matrix summary

```powershell
$env:OBJC3C_BOOTSTRAP_MATRIX_RUN_ID='m263_e002_bootstrap_matrix_closeout'
$env:OBJC3C_BOOTSTRAP_MATRIX_LLC_PATH='C:\Program Files\LLVM\bin\llc.exe'
& 'C:\Program Files\PowerShell\7\pwsh.exe' -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_bootstrap_matrix.ps1
```

## 3. Inspect the matrix summary

The canonical operator evidence path is:

- `tmp/artifacts/objc3c-native/bootstrap-matrix/m263_e002_bootstrap_matrix_closeout/summary.json`

The summary must report five passing cases:

- `single-image-default`
- `single-image-explicit`
- `archive-backed-plain`
- `archive-backed-single-retained`
- `archive-backed-merged-retained`

## 4. Re-run the milestone closeout gate

```powershell
npm run check:objc3c:m263-e002-lane-e-readiness
```
