# objc3c Developer Tooling Boundary

## Working Boundary

This runbook defines the live developer-tooling, inspection, and debugging
surface for objc3c.

Use it when changing developer ergonomics, explainability, runtime inspection,
or debug-reporting behavior.

Downstream developer-tooling work must stay on the existing implementation paths below
instead of introducing sidecar drivers, release-scope wrappers, or evidence-only
inspection flows.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/developer_tooling/boundary_inventory.json`

Replayable public workflow actions:

- `npm run objc3c -- validate-developer-tooling`
- `npm run objc3c -- validate-runnable-developer-tooling`
- `npm run objc3c -- inspect-compile-observability`
- `npm run objc3c -- inspect-runtime-inspector`
- `npm run objc3c -- inspect-capability-explorer`
- `npm run objc3c -- inspect-editor-tooling`
- `npm run objc3c -- format-objc3c`
- `npm run objc3c -- rewrite-objc3c-source`
- `npm run objc3c -- check-developer-diagnostic-quality`
- `npm run objc3c -- trace-runtime-debug`
- `npm run objc3c -- trace-compile-stages`
- `npm run objc3c -- test-capability-routed-source-parity`

Helper implementations and native tool binaries are registry
anchors for those actions, not separate current-facing commands.

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
  - `npm run objc3c -- build-native-binaries`
  - published binary: `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- public command and workflow surface:
  - package bridge: `npm run objc3c -- <action>`
  - `scripts.objc3c_workflow`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- runtime inspection and debug-state implementation:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/{classes,dispatch,images,selectors,state,public}/`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/artifacts/`
- runtime/debug ABI and contract emitters:
  - `native/objc3c/src/lower/contracts/`
  - `native/objc3c/src/ir/`
- live validation and parity paths:
  - `scripts/check_objc3c_library_cli_parity.py`
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `npm run objc3c -- test-capability-routed-source-parity`
  - `npm run objc3c -- test-runtime-acceptance-fast`
  - `npm run objc3c -- check-repo-superclean-surface`
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
- build-owned owner metadata comes from the generated repo-superclean owner
  artifact selected by the checked-in build contract

Downstream issues must extend these exact surfaces before inventing new ones.

## Exact Live Artifact And Output Paths

- native binaries:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- runtime library:
  - `artifacts/lib/objc3_runtime.lib`
- build-emitted owner artifact:
  - generated repo-superclean owner artifact selected by the checked-in build contract
  - `tmp/build-objc3c-native/repo_superclean_source_of_truth.json`
- default compile/explain output root:
  - generated native compilation artifact root
  - `tmp/artifacts/compilation/objc3c-native/`
- runtime/debug report roots:
  - runtime reports
  - public-workflow reports
  - playground artifacts
  - playground reports
- developer-tooling dump artifacts:
  - generated editor-tooling reports
  - `tmp/reports/developer-tooling/`
  - compile observability summary
  - `tmp/reports/objc3c-public-workflow/compile-observability.json`
  - compile observability payload
  - runtime inspector summary
  - `tmp/reports/objc3c-public-workflow/runtime-inspector.json`
  - runtime inspector payload
  - capability explorer payload
  - `tmp/reports/objc3c-public-workflow/capability-explorer.json`
  - `capability_demo_compatibility`
  - runtime inspector benchmark payload
  - deterministic runtime debug trace payload
  - `tmp/reports/objc3c-public-workflow/runtime-debug-trace.json`
  - compile-stage trace summary
  - `tmp/reports/objc3c-public-workflow/compile-stage-trace.json`
  - compile-stage trace payload

## Exact Live Commands

- build the live binaries and contracts:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- build-native-contracts`
- compile one source through the public compiler path:
  - `npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/hello.objc3`
- inspect one source through the raw runner binary when debugging runner-only issues:
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3`
- materialize a runnable workspace with editor/debug drill references:
  - `npm run objc3c -- materialize-playground-workspace tests/tooling/fixtures/native/hello.objc3`
- inspect the direct compiler/summary boundary:
  - `npm run objc3c -- inspect-compile-observability tests/tooling/fixtures/native/hello.objc3`
- dump the structured developer observability object through the public command surface:
  - `npm run objc3c -- inspect-compile-observability`
- dump the structured runtime inspector object through the public command surface:
  - `npm run objc3c -- inspect-runtime-inspector`
- dump the live capability-explorer object through the public command surface:
  - `npm run objc3c -- inspect-capability-explorer`
- benchmark the runtime-inspector and capability-explorer workflow through the public command surface:
  - `npm run objc3c -- benchmark-runtime-inspector`
- dump the structured compile-stage trace through the public command surface:
  - `npm run objc3c -- trace-compile-stages`
- compose the deterministic runtime debug trace through the public command surface:
  - `npm run objc3c -- trace-runtime-debug`
- inspect the combined editor tooling surface:
  - `npm run objc3c -- inspect-editor-tooling`
- format one supported objc3c source through the canonical Objective-C 3 formatter subset:
  - `npm run objc3c -- format-objc3c -- tests/tooling/fixtures/developer_tooling/messy_hello.objc3`
- rewrite one supported objc3c source through the safe token-boundary rewrite engine:
  - `npm run objc3c -- rewrite-objc3c-source tests/tooling/fixtures/developer_tooling/rewrite_legacy_aliases.objc3 -- --rule legacy-literal-aliases --rename-symbol legacyValue=canonicalValue`
- validate diagnostic taxonomy and fix-it quality over the checked-in diagnostic manifest:
  - `npm run objc3c -- check-developer-diagnostic-quality`
- run the integrated developer-tooling validation flow:
  - `npm run objc3c -- validate-developer-tooling`
- run the packaged developer-tooling validation flow against the staged runnable bundle:
  - `npm run objc3c -- validate-runnable-developer-tooling`
- validate compiler/library parity:
  - `npm run objc3c -- test-capability-routed-source-parity`
- validate runtime/debug ABI and emitted source surfaces:
  - `npm run objc3c -- test-runtime-acceptance-fast`

## Runtime Introspection Primitives

- exported ARC/debug snapshot ABI:
  - `objc3_runtime_copy_arc_debug_state_for_testing`
- emitted metadata that already records inspection/debug facts:
  - `arc_debug_state_snapshot_symbol`
  - `runtime_metadata_object_inspection_uses_llvm_objdump`
- live capability-explorer probe contract:
  - `npm run objc3c -- inspect-capability-explorer`
  - generated capability-explorer public-workflow payload
  - `capability_demo_consistency`
  - `stdlib/program_surface.json`
  - `showcase/portfolio.json`
- downstream work must treat those runtime-emitted facts as authoritative over
  ad hoc report text

## Working Rules For Downstream Issues

- extend the existing native tool or runtime ABI before adding any new script
  wrapper
- treat the `package.json` bridge, `npm run objc3c -- <action>`, as the
  only public command routing surface
- keep emitted reports and dumps under `tmp/`
- keep checked-in developer guidance under `docs/runbooks/`
- prove inspection/debug behavior through the existing runtime acceptance and
  parity paths before adding broader integration coverage

## Current Capability Map

The current checked-in developer-tooling surface is intentionally narrower than a
full editor product:

- supported today:
  - first-run setup through the public `npm run objc3c -- <action>` bridge
  - copy/paste quickstart commands checked by the getting-started surface
  - ready-to-compile project template materialization with a published compile
    command contract
  - migration analyzer and rewrite examples with checked-in positive output and
    fail-closed diagnostics
  - compile observability
  - runtime inspection
  - capability exploration
  - runtime inspector benchmarking
  - compile-stage tracing
  - deterministic runtime debug tracing from runtime-inspector, compile-stage,
    and editor debug artifacts
  - manifest-backed language-server capabilities and navigation
  - deterministic workspace semantic indexing across the primary source,
    checked-in stdlib modules, and showcase package workspaces
  - diagnostic-fix-it-backed `codeAction` payloads for checked deterministic
    fix-it ranges
  - canonical Objective-C 3 source formatter output on the supported checked-in subset
  - safe source rewrite output for token-boundary literal canonicalization and identifier rewrites
  - diagnostic taxonomy and fix-it quality gate over checked-in diagnostic fixtures
  - declaration-breakpoint and object-symbol inspection debug anchors
  - runnable workspace drill materialization with editor/debug references
  - integrated developer-tooling validation
  - packaged CLI-to-editor, formatter, debug, and workspace handoff over the staged runnable toolchain bundle
- explicit remaining gaps after the current implementation slice:
  - references, rename, semantic tokens, and non-diagnostic code actions remain fail-closed
  - statement-level stepping and full source-map publication remain fail-closed
  - async task inspection, error/unwind tracing, and LLDB plugin support remain
    reserved until emitted runtime trace snapshots prove those lanes directly

Downstream work must extend the real frontend runner, runtime artifacts, and
public workflow commands instead of creating an editor-only shadow parser or
debug-only sidecar data model.

The support matrix row
`tooling.developer-experience.first-run-product-path` is the current public
truth boundary for the normal-developer path. Its support claim is limited to
checked-in first-run, template, migration, and diagnostic contracts. It does not
publish a full IDE, full language-server, retired Objective-C 2 compiler input,
or debugger-stepping claim.

## Explicit Gap Inventory

Current remaining gaps after the current developer-tooling slice:

- no checked-in references/rename/semantic-token/general-code-action contract
  beyond deterministic diagnostic fix-its
- no checked-in statement-level stepping or full source-map publication contract

These remaining gaps stay fail-closed; they are not implied by the formatter,
safe rewrite, diagnostic quality, or declaration-breakpoint debug surface.

## Diagnostics, Formatting, And Symbol Resolution Policy

Diagnostics, formatting, and symbol resolution must stay coupled to the live
frontend runner output model.

- diagnostics owner outputs:
  - diagnostics source of truth: the frontend runner summary JSON for compile
    status, blocking stage, and aggregate severity
  - diagnostics source of truth: the emitted diagnostics JSON with real line, column, severity, code, and message fields stays authoritative
- symbol resolution owner outputs:
  - symbol resolution source of truth: the emitted manifest declaration records
    for globals, functions, interfaces, implementations, protocols, and
    categories
  - declaration coordinates published by the real compile output
  - workspace semantic index entries derived from checked-in stdlib module
    inventory, showcase package workspaces, and package lock/mirror guardrail
    contracts
- formatting owner output:
  - machine-owned formatter output must be generated from the canonical
    formatter helper and reflected through the combined developer-tooling
    surface
  - formatter claims must fail closed when the source is malformed or outside the supported
    canonical subset
- safe source rewrite owner output:
  - safe source rewrites publish token-boundary edits, replacement text, source
    ranges, and deterministic source hashes
  - rewrite rules skip strings and comments and fail closed on malformed source
- diagnostic taxonomy and fix-it quality gate:
  - checked-in diagnostic fixtures must publish stable diagnostic codes,
    severities, messages, source ranges, and explicit fix-it arrays
  - machine-applicable fix-its must carry structured source ranges and
    replacement text before they count as fix-it coverage

Downstream editor and navigation work must use compile-owned declaration
coordinates instead of building a shadow symbol index from ad hoc text scans.

## Language-Server Capability Publication Policy

The canonical policy contract for this surface is:

- `tests/tooling/fixtures/developer_tooling/language_server_capability_publication_policy.json`

Language-server claims must stay narrower than the real shipped capability set.

- supported capability claims may only be published when they can be backed by:
  - compile-owned diagnostics
  - compile-owned declaration coordinates
  - emitted artifact presence and runtime inspection facts
- unsupported capability classes must fail closed with explicit unpublished-status
  metadata instead of pretending unpublished capabilities are supported:
  - references
  - rename
  - semantic tokens
  - non-diagnostic code actions
  - statement-level debugger stepping

The public developer-tooling surface must publish one canonical capability map
with capability status, publication status, and evidence roots instead of
duplicating per-editor interpretations.

Workspace-symbol claims require both manifest-backed declaration coordinates
and the workspace semantic index guardrails. The index may include checked-in
package entries and cross-package stdlib follow-up edges, but it must not treat
generated `tmp/` locks, mirrors, or reports as source authority.

## Hosted LLVM Capability Truth Payloads

Hosted LLVM and capability-routed parity claims are owned by typed workflow
contracts, not by local executable discovery text.

- local `check-llvm-capabilities` probe output is diagnostic-only, even when it
  finds clang and llc on the current machine
- hosted execution support requires a hosted LLVM summary with canonical mode,
  `ok=true`, clang availability, llc availability, and llc `--filetype=obj`
  support
- capability-routed source parity is publishable only when the same hosted
  object-emission truth is available
- fail-closed payload fields must include source kind, local-diagnostic status,
  hosted execution support, hosted source parity support, and failure reasons

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

The checked-in developer-tooling surface must publish one machine-owned editor tooling surface instead of
scattering separate editor-only payloads across ad hoc scripts.

The generated developer-tooling surface must group:

- diagnostics summary and per-diagnostic entries
- language-server capability publication and unpublished-status metadata
- navigation and declaration coordinates rooted in compile-owned manifest data
- workspace semantic package index, package-symbol entries, and cross-package
  navigation edges rooted in checked-in workspace/package contracts
- formatter execution results and formatted output references
- debug artifact inspection, breakpoint anchors, and stepping availability
- deterministic runtime debug trace commands and report paths

The developer-tooling report family is transient output produced by the public
runner plus replayable checked-in scripts. It does not own capability claims.

The current generator for the combined surface is reached through:

- `npm run objc3c -- inspect-editor-tooling`

The current and follow-on public entrypoints for the surface converge on:

- `npm run objc3c -- inspect-editor-tooling`
- `npm run objc3c -- format-objc3c`
- `npm run objc3c -- rewrite-objc3c-source`
- `npm run objc3c -- check-developer-diagnostic-quality`
- `npm run objc3c -- trace-runtime-debug`
- `npm run objc3c -- validate-developer-tooling`

The current formatter/debug/workspace slice is action-catalog-owned. Its script
paths are implementation details, not direct public commands.

Checked-in contracts for the current slice:

- `tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json`
- `schemas/objc3c-developer-tooling-editor-surface-v1.schema.json`
- `schemas/objc3c-runtime-debug-trace-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`
- `tests/tooling/fixtures/developer_tooling/packaged_cli_to_editor_contract.json`

The editor tooling schema is a registry-backed owner surface. This runbook must
not copy its JSON shape or promote generated editor tooling reports into public
support claims outside the capability matrix and evidence map.

The npm entrypoints route to the same action family:

- `npm run objc3c -- inspect-editor-tooling`
- `npm run objc3c -- format-objc3c <source>`
- `npm run objc3c -- rewrite-objc3c-source <source> -- --rule legacy-literal-aliases --rename-symbol oldName=newName`
- `npm run objc3c -- check-developer-diagnostic-quality`
- `npm run objc3c -- trace-runtime-debug`
- `npm run objc3c -- validate-developer-tooling`
- `npm run objc3c -- validate-runnable-developer-tooling`

## Explicit Non-Goals

- no release-scope debug launcher
- no ad hoc LLVM-only inspection path treated as owner evidence
- no duplicate command surface outside the `package.json` bridge:
  `npm run objc3c -- <action>`
- no hand-authored report snapshots under checked-in doc roots
- no new parallel owner copy for runtime inspection semantics
