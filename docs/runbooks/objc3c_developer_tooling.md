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
- CLI reporting contract scaffolds:
  - `native/objc3c/src/io/objc3_cli_reporting_output_contract_scaffold.h`
  - `native/objc3c/src/io/objc3_cli_reporting_output_contract_core_feature_surface.h`
  - `native/objc3c/src/io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h`
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

## Explainability And Introspection Surface

The current live explainability surface is intentionally narrow and already
implemented:

- compiler-side summary and artifact emission comes from
  `objc3c-frontend-c-api-runner`
- runtime-side inspection comes from exported testing/debug ABI snapshots that
  are emitted from the real runtime implementation
- build-owned source-of-truth metadata comes from
  `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json`

Downstream issues must extend these exact surfaces before inventing new ones.

## Exact Live Artifact And Output Paths

- native binaries:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- runtime library:
  - `artifacts/lib/objc3_runtime.lib`
- build-emitted source-of-truth artifact:
  - `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json`
- default compile/explain output root:
  - `tmp/artifacts/compilation/objc3c-native/`
- runtime/debug report roots:
  - `tmp/reports/runtime/`
  - `tmp/reports/objc3c-public-workflow/`
- developer-tooling dump artifacts:
  - `tmp/reports/objc3c-public-workflow/inspect-compile-observability-summary.json`
  - `tmp/reports/objc3c-public-workflow/compile-observability.json`
  - `tmp/reports/objc3c-public-workflow/inspect-runtime-inspector-summary.json`
  - `tmp/reports/objc3c-public-workflow/runtime-inspector.json`

## Exact Live Commands

- build the live binaries and contracts:
  - `npm run build:objc3c-native`
  - `npm run build:objc3c-native:contracts`
- compile one source through the public compiler path:
  - `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3`
- inspect the direct compiler/summary boundary:
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3 --summary-out tmp/reports/objc3c-public-workflow/frontend-c-api-runner-summary.json`
- dump the structured developer observability object through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py inspect-compile-observability`
  - `npm run inspect:objc3c:observability`
- dump the structured runtime inspector object through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py inspect-runtime-inspector`
  - `npm run inspect:objc3c:runtime`
- validate compiler/library parity:
  - `python scripts/check_objc3c_library_cli_parity.py`
- validate runtime/debug ABI and emitted source surfaces:
  - `python scripts/check_objc3c_runtime_acceptance.py`

## Runtime Introspection Primitives

- exported ARC/debug snapshot ABI:
  - `objc3_runtime_copy_arc_debug_state_for_testing`
- emitted metadata that already records inspection/debug facts:
  - `arc_debug_state_snapshot_symbol`
  - `runtime_metadata_object_inspection_uses_llvm_objdump`
- downstream work must treat those runtime-emitted facts as authoritative over
  ad hoc report text

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
