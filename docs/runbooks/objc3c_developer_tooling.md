# objc3c Developer Tooling Boundary

## Working Boundary

This runbook defines the live developer-tooling, inspection, and debugging
surface for objc3c.

Use it when changing developer ergonomics, explainability, runtime inspection,
or debug-reporting behavior.

Downstream work in `M291` must stay on the existing implementation paths below
instead of introducing sidecar drivers, milestone-local wrappers, or proof-only
inspection flows.

## Exact Live Implementation Paths

- native tooling entrypoint:
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- native tooling target wiring:
  - `native/objc3c/CMakeLists.txt`
- native build/publish surface:
  - `scripts/build_objc3c_native.ps1`
  - published binary: `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- public command and workflow surface:
  - `scripts/objc3c_public_workflow_runner.py`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- runtime inspection and debug-state implementation:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime/debug ABI and contract emitters:
  - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- live validation and parity paths:
  - `scripts/check_objc3c_library_cli_parity.py`
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_repo_superclean_surface.py`
- live runtime probes:
  - `tests/tooling/runtime/arc_debug_instrumentation_probe.cpp`
  - `tests/tooling/runtime/block_arc_runtime_abi_probe.cpp`
  - `tests/tooling/runtime/live_cleanup_retainable_integration_probe.cpp`
  - `tests/tooling/runtime/task_runtime_hardening_probe.cpp`
  - `tests/tooling/runtime/system_helper_runtime_contract_probe.cpp`

## Working Rules For Downstream Issues

- extend the existing native tool or runtime ABI before adding any new script
  wrapper
- treat `scripts/objc3c_public_workflow_runner.py` as the only public command
  routing surface
- keep emitted reports and dumps under `tmp/`
- keep checked-in developer guidance under `docs/runbooks/`
- prove inspection/debug behavior through the existing runtime acceptance and
  parity paths before adding broader integration coverage

## Explicit Non-Goals

- no milestone-local debug launcher
- no ad hoc LLVM-only inspection path treated as source of truth
- no duplicate command surface outside `package.json` and
  `scripts/objc3c_public_workflow_runner.py`
- no hand-authored report snapshots under checked-in doc roots
- no new parallel source-of-truth copy for runtime inspection semantics
