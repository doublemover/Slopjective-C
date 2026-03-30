# objc3c Stress Validation Boundary

## Working Boundary

This runbook defines the live `M302` boundary for generative, fuzz, replay,
and stress validation work in the checked-in objc3c repo.

Use it when changing:

- malformed-input parser and semantic fuzz coverage
- lowering and runtime stress sweeps over checked-in fixtures
- mixed-module and import/export stress validation
- reducer, minimization, crash triage, and replay artifacts
- public workflow and runnable-package stress validation commands

Downstream `M302` issues must stay on the existing compiler, conformance,
execution-smoke, package, and public workflow surfaces listed here. Do not add
a sidecar fuzz harness app, hidden fixture root, or milestone-local reducer
wrapper outside the checked-in repo paths named below.

## Current Truthful Architecture

The current truthful stress-validation shape is:

- `parser-sema-fuzz`
  - objective: drive deterministic malformed-input coverage through the live
    compiler entrypoint and preserve machine-readable failure summaries
- `lowering-runtime-stress`
  - objective: widen checked-in lowering/runtime fixtures through the same
    compile and runtime launch paths already used by runnable validation
- `mixed-module-differential`
  - objective: exercise provider/consumer and import/export stress cases
    without inventing a second module build pipeline
- `replay-minimization`
  - objective: preserve failing inputs, deterministic signatures, and reduced
    replay candidates in machine-owned report roots
- `crash-triage`
  - objective: collapse failing runs onto stable signatures and reducer inputs
    that can be replayed from checked-in scripts

## Differential And Fuzz Safety Policy

The checked-in safety policy for this milestone lives at:

- `tests/tooling/fixtures/stress/safety_policy.json`

Downstream work must preserve these rules:

- prefer checked-in fixtures, deterministic mutations, and replay-backed seeds
  over unbounded random generation
- keep execution bounded by explicit per-case timeouts and manifest-selected
  corpus size
- keep reducer, replay, and triage outputs in machine-owned roots only
- preserve original checked-in inputs unchanged
- record stable failure signatures before claiming a reducer result
- never publish safety or coverage claims that cannot be traced back to
  checked-in sources and machine-readable outputs

## Exact Live Implementation Paths

- public command and package surface:
  - `scripts/objc3c_public_workflow_runner.py`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- native build and compile roots:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_native_compile.ps1`
  - `scripts/package_objc3c_runnable_toolchain.ps1`
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`
- existing live validation roots that downstream work must extend instead of
  duplicating:
  - `scripts/run_objc3c_fuzz_safety.py`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_runnable_interop_conformance.py`
  - `scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py`
- checked-in fixture families that already hold the live stress corpus:
  - `tests/tooling/fixtures/stress/source_surface.json`
  - `tests/tooling/fixtures/stress/README.md`
  - `tests/tooling/fixtures/stress/safety_policy.json`
  - `tests/tooling/fixtures/native/recovery/negative/`
  - `tests/tooling/fixtures/native/recovery/positive/`
  - `tests/tooling/fixtures/native/execution/negative/`
  - `tests/tooling/fixtures/native/`
  - `tests/tooling/fixtures/objc3c/`
  - `tests/conformance/`

## Exact Live Artifact And Output Paths

- build-owned source-of-truth artifact:
  - `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json`
- checked-in stress source summary:
  - `tmp/reports/stress/source-surface-summary.json`
- current machine-owned fuzz root:
  - `tmp/artifacts/objc3c-native/fuzz-safety/`
- existing machine-owned validation roots that downstream work must reuse:
  - `tmp/artifacts/objc3c-native/execution-smoke/`
  - `tmp/reports/conformance/`
  - `tmp/reports/runtime/`
  - `tmp/pkg/objc3c-native-runnable-toolchain/`
- new machine-owned stress roots for downstream `M302` issues:
  - `tmp/artifacts/stress/`
  - `tmp/reports/stress/`

## Exact Live Commands

- build the native toolchain before stress validation:
  - `python scripts/objc3c_public_workflow_runner.py build-native-binaries`
  - `npm run build:objc3c-native`
- run the current parser/sema malformed-input gate:
  - `python scripts/run_objc3c_fuzz_safety.py`
- validate the checked-in stress source contract:
  - `python scripts/check_stress_source_surface.py`
- run the current executable smoke and replay proof paths:
  - `python scripts/objc3c_public_workflow_runner.py test-execution-smoke`
  - `python scripts/objc3c_public_workflow_runner.py test-execution-replay`
- run the current conformance-corpus integration paths:
  - `python scripts/objc3c_public_workflow_runner.py validate-conformance-corpus`
  - `python scripts/objc3c_public_workflow_runner.py validate-runnable-conformance-corpus`
- stage the runnable package before packaged stress validation:
  - `python scripts/objc3c_public_workflow_runner.py package-runnable-toolchain`
  - `npm run package:objc3c-native:runnable-toolchain`

## Exact Live Paths For Downstream Work

- boundary and operator guidance:
  - `docs/runbooks/objc3c_stress_validation.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `docs/runbooks/objc3c_maintainer_workflows.md`
- fixture and corpus ownership:
  - `tests/tooling/fixtures/stress/source_surface.json`
  - `tests/tooling/fixtures/stress/safety_policy.json`
  - `tests/tooling/fixtures/native/`
  - `tests/tooling/fixtures/objc3c/`
  - `tests/conformance/corpus_surface.json`
  - `tests/conformance/README.md`
- compile and runtime execution roots:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_native_compile.ps1`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/objc3c_public_workflow_runner.py`
  - `scripts/package_objc3c_runnable_toolchain.ps1`

## Explicit Non-Goals

- no second fuzz corpus outside checked-in fixture and conformance roots
- no hidden random-input generator without checked-in seed or reducer rules
- no milestone-local wrapper that bypasses the public compile or runtime path
- no publishable safety or coverage claim backed only by prose
- no GUI or separate service for crash triage, replay, or reducer execution
