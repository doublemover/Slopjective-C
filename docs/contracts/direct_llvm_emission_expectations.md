# Direct LLVM Emission Expectations (M135)

## Contract Metadata

- Contract ID: `objc3c-direct-llvm-emission/m135-v1`
- Owner issue: [#4268](https://github.com/doublemover/Slopjective-C/issues/4268)
- Related implementation issue: [#4264](https://github.com/doublemover/Slopjective-C/issues/4264)
- Scope: `.objc3` native compilation path in `objc3c-native`

## Expectations

| ID | Expectation |
| --- | --- |
| `M135-LLVM-01` | `.objc3` compilation emits deterministic `module.ll`, `module.obj`, `module.manifest.json`, and `module.diagnostics.json` artifacts from the native pipeline. |
| `M135-LLVM-02` | `.objc3` object emission uses direct LLVM-native object generation for production output. |
| `M135-LLVM-03` | Fallback path `RunIRCompile(...clang -x ir...)` is forbidden for `.objc3` production emission. |
| `M135-LLVM-04` | `--clang` remains scoped to non-`.objc3` compatibility (`.m`) and downstream link/test harness usage; it is not a `.objc3` object-emission dependency. |
| `M135-LLVM-05` | Emission failures are fail-closed with deterministic diagnostics and a non-zero exit code. |
| `M145-LLVM-06` | Lane-C matrix hardening covers process (`native/objc3c/src/io/objc3_process.cpp`), driver/routing (`native/objc3c/src/driver/objc3_objc3_path.cpp`, `native/objc3c/src/driver/objc3_llvm_capability_routing.cpp`), runtime ABI emit path (`native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`), and C API runner forwarding (`native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`) with explicit fail-closed backend semantics and no hidden clang fallback. |
| `M145-B001` | Lane-B sema/type-system matrix hardening in `scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1` covers deterministic `clang` + `llvm-direct` replay, forced missing-`llc` fail-closed object-emission behavior, and backend-invariant semantic-negative diagnostics. |
| `M145-LLVM-07` | Lane-D validation/conformance/perf extension binds `tests/conformance/lowering_abi/M145-D001.json` into the conformance coverage/manifest graph and enforces matrix closeout command wiring via `test:objc3c:m145-direct-llvm-matrix:lane-d`. |

## CI And Tooling Enforcement

The following checks are required to stay wired and fail-closed:

- `python scripts/check_m135_direct_llvm_contract.py`
- `npm run check:compiler-closeout:m135`
- `npm run check:task-hygiene`

The checker validates packet/docs presence and CI wiring so contract drift fails before milestone closeout.

## M145 Matrix Extension (Lane B + Lane C)

M145 extends direct LLVM object-emission fail-closed matrix coverage to sema/type-system lane-B and lowering/runtime-ABI lane-C surfaces:

- `python scripts/check_m145_direct_llvm_matrix_contract.py`
- `npm run test:objc3c:m145-direct-llvm-matrix`
- `npm run check:compiler-closeout:m145`

The matrix checker enforces:

- explicit lane-B sema matrix coverage with deterministic backend replay checks and forced missing-`llc` fail-closed assertions,
- explicit `clang|llvm-direct` backend branching in CLI and runtime ABI entrypoints,
- deterministic fail-closed diagnostics for invalid/missing backend prerequisites (`O3E001`) and emit failures (`O3E002`),
- no hidden fallback object-emission paths in driver, capability-routing, or runtime ABI backend dispatch blocks.

Lane-C fail-closed anchor check IDs (stable):

- `process-m145-01` through `process-m145-05`
- `driver-m145-01` through `driver-m145-04`
- `routing-m145-01` through `routing-m145-05`
- `frontend-m145-01` through `frontend-m145-10`
- `runner-m145-01` through `runner-m145-06`
- `lanec-m145-01`

## M145 Matrix Extension (Lane D)

Lane-D extends the same fail-closed matrix into validation/conformance/perf controls:

- Conformance fixture: `tests/conformance/lowering_abi/M145-D001.json`
- Coverage-map traceability: `tests/conformance/COVERAGE_MAP.md`
- Manifest registration: `tests/conformance/lowering_abi/manifest.json`
- Validation command: `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`

The lane-D extension is required to ensure matrix expectations are not only
source-level checks but also traceable through conformance and perf execution
gates used by closeout.

Lane-D fixture anchor payload used by the M145 checker when validating fail-closed matrix markers:

```json
{
  "id": "M145-D001",
  "lane": "D",
  "issue": 4317,
  "cmd": "objc3c-native --objc3-ir-object-backend llvm-direct",
  "expected": ["O3E001", "O3E002"]
}
```

## Validation Workflow

1. `npm run lint:md:all`
2. `npm run check:compiler-closeout:m135`
3. `npm run check:task-hygiene`
4. Execute the M135 native command set from `spec/planning/compiler/m135/m135_parallel_dispatch_plan_20260228.md`.
