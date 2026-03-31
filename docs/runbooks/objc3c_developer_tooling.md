# objc3c Developer Tooling Boundary

## Working Boundary

This runbook defines the live developer-tooling, inspection, and debugging
surface for objc3c.

Use it when changing developer ergonomics, explainability, runtime inspection,
or debug-reporting behavior.

Downstream developer-tooling work must stay on the existing implementation paths below
instead of introducing sidecar drivers, milestone-local wrappers, or proof-only
inspection flows.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/developer_tooling/boundary_inventory.json`

Replayable generators and validators:

- `python scripts/build_developer_tooling_boundary_inventory_summary.py`
- `python scripts/check_objc3c_developer_tooling_integration.py`
- `python scripts/check_m325_developer_tooling_closeout_gate.py`

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
  - `tmp/artifacts/playground/`
  - `tmp/reports/playground/`
- developer-tooling dump artifacts:
  - `tmp/reports/objc3c-public-workflow/inspect-compile-observability-summary.json`
  - `tmp/reports/objc3c-public-workflow/compile-observability.json`
  - `tmp/reports/objc3c-public-workflow/inspect-runtime-inspector-summary.json`
  - `tmp/reports/objc3c-public-workflow/runtime-inspector.json`
  - `tmp/reports/objc3c-public-workflow/capability-explorer.json`
  - `tmp/reports/objc3c-public-workflow/runtime-inspector-benchmark.json`
  - `tmp/reports/objc3c-public-workflow/trace-compile-stages-summary.json`
  - `tmp/reports/objc3c-public-workflow/compile-stage-trace.json`

## Exact Live Commands

- build the live binaries and contracts:
  - `npm run build:objc3c-native`
  - `npm run build:objc3c-native:contracts`
- compile one source through the public compiler path:
  - `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3`
- materialize a runnable workspace with editor/debug drill references:
  - `python scripts/objc3c_public_workflow_runner.py materialize-playground-workspace tests/tooling/fixtures/native/hello.objc3`
  - `npm run build:objc3c:playground -- tests/tooling/fixtures/native/hello.objc3`
- inspect the direct compiler/summary boundary:
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3 --summary-out tmp/reports/objc3c-public-workflow/frontend-c-api-runner-summary.json`
- dump the structured developer observability object through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py inspect-compile-observability`
  - `npm run inspect:objc3c:observability`
- dump the structured runtime inspector object through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py inspect-runtime-inspector`
  - `npm run inspect:objc3c:runtime`
- dump the live capability-explorer object through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py inspect-capability-explorer`
  - `npm run inspect:objc3c:capabilities`
- benchmark the runtime-inspector and capability-explorer workflow through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py benchmark-runtime-inspector`
  - `npm run inspect:objc3c:benchmark`
- dump the structured compile-stage trace through the public command surface:
  - `python scripts/objc3c_public_workflow_runner.py trace-compile-stages`
  - `npm run trace:objc3c:stages`
- inspect the combined editor tooling surface:
  - `python scripts/objc3c_public_workflow_runner.py inspect-editor-tooling`
  - `npm run inspect:objc3c:editor`
- format one supported objc3c source through the preview formatter subset:
  - `python scripts/objc3c_public_workflow_runner.py format-objc3c -- tests/tooling/fixtures/developer_tooling/messy_hello.objc3`
  - `npm run format:objc3c -- tests/tooling/fixtures/developer_tooling/messy_hello.objc3`
- run the integrated developer-tooling validation flow:
  - `python scripts/objc3c_public_workflow_runner.py validate-developer-tooling`
  - `npm run test:objc3c:developer-tooling`
- run the packaged developer-tooling validation flow against the staged runnable bundle:
  - `python scripts/objc3c_public_workflow_runner.py validate-runnable-developer-tooling`
  - `npm run test:objc3c:runnable-developer-tooling`
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
- live capability-explorer probe contract:
  - `scripts/probe_objc3c_llvm_capabilities.py`
  - `tmp/reports/objc3c-public-workflow/capability-explorer.json`
  - `capability_demo_compatibility`
  - `stdlib/program_surface.json`
  - `showcase/portfolio.json`
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

## Current Capability Map

The current checked-in developer-tooling surface is intentionally narrower than a
full editor product:

- supported today:
  - compile observability
  - runtime inspection
  - capability exploration
  - runtime inspector benchmarking
  - compile-stage tracing
  - manifest-backed language-server capabilities and navigation
  - preview formatter output on the supported canonical subset
  - declaration-breakpoint and object-symbol inspection debug anchors
  - runnable workspace drill materialization with editor/debug references
  - integrated developer-tooling validation
  - packaged CLI-to-editor, formatter, debug, and workspace handoff over the staged runnable toolchain bundle
- explicit remaining gaps after the current implementation slice:
  - references, rename, semantic tokens, and code actions remain fail-closed
  - statement-level stepping and full source-map publication remain fail-closed

Downstream work must extend the real frontend runner, runtime artifacts, and
public workflow commands instead of creating an editor-only shadow parser or
debug-only sidecar data model.

## Explicit Gap Inventory

Current remaining gaps after the current developer-tooling slice:

- no checked-in references/rename/semantic-token/code-action contract
- no checked-in statement-level stepping or full source-map publication contract

These remaining gaps stay fail-closed; they are not implied by the preview
formatter or declaration-breakpoint debug surface.

## Diagnostics, Formatting, And Symbol Resolution Policy

Diagnostics, formatting, and symbol resolution must stay coupled to the live
frontend runner output model.

- diagnostics source of truth:
  - the frontend runner summary JSON
  - the emitted diagnostics JSON with real line, column, severity, code, and
    message entries
- symbol resolution source of truth:
  - the emitted manifest declaration records for globals, functions,
    interfaces, implementations, protocols, and categories
  - declaration coordinates published by the real compile output
- formatting source of truth:
  - machine-owned formatter output must be generated from the canonical
    formatter helper and reflected through the combined developer-tooling
    surface
  - formatter claims must fail closed when the source is outside the supported
    canonical subset

Downstream editor and navigation work must use compile-owned declaration
coordinates instead of building a shadow symbol index from ad hoc text scans.

## Language-Server Capability And Fallback Policy

Language-server claims must stay narrower than the real shipped capability set.

- supported capability claims may only be published when they can be backed by:
  - compile-owned diagnostics
  - compile-owned declaration coordinates
  - emitted artifact presence and runtime inspection facts
- unsupported capability classes must fail closed with explicit fallback
  metadata instead of pretending partial support:
  - references
  - rename
  - semantic tokens
  - code actions
  - statement-level debugger stepping

The public developer-tooling surface must publish one canonical capability map
with capability status, fallback status, and evidence roots instead of
duplicating per-editor interpretations.

## Debugger, Source-Map, And Stepping Semantics

Debugger and stepping behavior must stay tied to emitted artifacts and truthful
availability rules.

- breakpoint and navigation anchors come from compile-owned declaration
  coordinates
- object-backed symbol visibility comes from the emitted object artifact and the
  runtime inspector symbol inventory path
- statement-level stepping and full source-map claims remain fail-closed until
  the emitted toolchain artifacts prove them directly

Until native line-table and full debugger metadata are emitted on the canonical
toolchain path, the public debug surface must describe itself as
declaration-breakpoint and artifact-inspection driven rather than as a full
statement debugger.

## Editor Protocol And Debug Artifact Contract

`M325` must publish one machine-owned editor tooling surface instead of
scattering separate editor-only payloads across ad hoc scripts.

The canonical generated surface must group:

- diagnostics summary and per-diagnostic entries
- language-server capability publication and fallback metadata
- navigation and declaration coordinates rooted in compile-owned manifest data
- formatter execution results and formatted output references
- debug artifact inspection, breakpoint anchors, and stepping availability

The authoritative report family lives under `tmp/reports/developer-tooling/`
and must be produced by the public runner plus replayable checked-in scripts.

The current generator for the combined surface is:

- `python scripts/build_objc3c_editor_tooling_surface.py`

The current and follow-on public entrypoints for the surface converge on:

- `python scripts/objc3c_public_workflow_runner.py inspect-editor-tooling`
- `python scripts/objc3c_public_workflow_runner.py format-objc3c`
- `python scripts/objc3c_public_workflow_runner.py validate-developer-tooling`

Exact implementation anchors for the current formatter/debug/workspace slice:

- `scripts/format_objc3c_source.py`
- `scripts/build_objc3c_editor_tooling_surface.py`
- `scripts/check_developer_tooling_formatter_debug_surface.py`
- `scripts/check_developer_tooling_workspace_integration.py`
- `scripts/check_objc3c_runnable_developer_tooling_end_to_end.py`
- `tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json`
- `tests/tooling/fixtures/developer_tooling/packaged_cli_to_editor_contract.json`

The npm entrypoints must route to the same action family once implemented:

- `npm run inspect:objc3c:editor`
- `npm run format:objc3c -- <source>`
- `npm run test:objc3c:developer-tooling`
- `npm run test:objc3c:runnable-developer-tooling`

## Explicit Non-Goals

- no milestone-local debug launcher
- no ad hoc LLVM-only inspection path treated as source of truth
- no duplicate command surface outside `package.json` and
  `scripts/objc3c_public_workflow_runner.py`
- no hand-authored report snapshots under checked-in doc roots
- no new parallel source-of-truth copy for runtime inspection semantics
