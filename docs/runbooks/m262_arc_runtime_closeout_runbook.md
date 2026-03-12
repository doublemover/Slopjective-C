# M262 ARC Runtime Closeout Runbook

## Scope

This runbook closes the currently supported runnable ARC slice for `M262`.
It covers:

- ARC mode-enabled source fixtures that compile, link, and run through the native toolchain
- the private property/runtime proof row consumed from `M262-D003`
- the frozen lane-E gate from `M262-E001`

It does not claim public object-allocation syntax, generalized ARC closeout, or
cross-module ARC behavior.

## Required local tools

- `pwsh`
- `python` with `pytest`
- `node` and `npm`
- `llc` reachable either on `PATH` or via `OBJC3C_NATIVE_EXECUTION_LLC_PATH`
- `clang` reachable either on `PATH` or via `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`

## Canonical commands

Build the native compiler and runtime archive:

```powershell
python scripts/ensure_objc3c_native_build.py --mode full --reason m262-e002-runbook
```

Re-run the frozen ARC gate:

```powershell
npm run check:objc3c:m262-e001-lane-e-readiness
```

Run the canonical execution smoke with the ARC closeout run id:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_RUN_ID='m262_e002_arc_closeout'; pwsh -NoProfile -ExecutionPolicy Bypass -File .\scripts\check_objc3c_native_execution_smoke.ps1
```

Run the ARC closeout checker:

```powershell
npm run check:objc3c:m262-e002-runnable-arc-closeout
```

Run the full lane-E readiness chain:

```powershell
npm run check:objc3c:m262-e002-lane-e-readiness
```

## Required predecessor evidence

- `tmp/reports/m262/M262-A002/arc_mode_handling_summary.json`
- `tmp/reports/m262/M262-B003/arc_interaction_semantics_summary.json`
- `tmp/reports/m262/M262-C004/arc_block_autorelease_return_lowering_summary.json`
- `tmp/reports/m262/M262-D003/arc_debug_instrumentation_summary.json`
- `tmp/reports/m262/M262-E001/arc_runtime_gate_summary.json`

## Required smoke evidence

- `tmp/artifacts/objc3c-native/execution-smoke/m262_e002_arc_closeout/summary.json`
- ARC-positive rows:
  - `tests/tooling/fixtures/native/execution/positive/arc_cleanup_scope_positive.objc3`
  - `tests/tooling/fixtures/native/execution/positive/arc_implicit_cleanup_void_positive.objc3`
  - `tests/tooling/fixtures/native/execution/positive/arc_block_autorelease_return_positive.objc3`

## Sign-off statement

- `M262` is closed only when every predecessor summary above is green, the ARC-positive smoke rows pass, and the private `M262-D003` property/runtime probe row remains live.
- The next implementation issue is `M263-A001`.
