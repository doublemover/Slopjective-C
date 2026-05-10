# objc3c Stress Validation Boundary

## Working Boundary

This runbook defines the live `objc3c.validation.stresssurface.v1` boundary for generative, fuzz, replay,
and stress validation work in the checked-in objc3c repo.

Use it when changing:

- malformed-input parser and semantic fuzz coverage
- lowering and runtime stress sweeps over checked-in fixtures
- mixed-module and import/export stress validation
- reducer, minimization, crash triage, and replay artifacts
- public workflow and runnable-package stress validation commands

Downstream stress-validation work must stay on the existing compiler, conformance,
execution-smoke, package, and public workflow surfaces listed here. Do not add
a sidecar fuzz harness app, hidden fixture root, or release-scope reducer
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

The checked-in stress safety policy lives at:

- `tests/tooling/fixtures/stress/safety_policy.json`

The checked-in machine-owned artifact and minimization surface lives at:

- `tests/tooling/fixtures/stress/artifact_surface.json`

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
  - package bridge: `npm run objc3c -- <action>`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- native build and compile roots:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- compile-objc3c`
  - `npm run objc3c -- package-runnable-toolchain`
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`
- existing live validation roots that downstream work must extend instead of
  duplicating:
  - `npm run objc3c -- test-fuzz-safety`
  - `npm run objc3c -- test-lowering-runtime-stress`
  - `npm run objc3c -- test-mixed-module-differential`
  - `npm run objc3c -- test-stress-minimization`
  - `npm run objc3c -- test-stress-crash-triage`
  - `npm run objc3c -- test-execution-smoke`
  - `npm run objc3c -- test-execution-replay`
  - `npm run objc3c -- test-runtime-acceptance-fast`
  - `npm run objc3c -- validate-interop-conformance`
  - `npm run objc3c -- validate-runnable-conformance-corpus`
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

## Live Artifact And Output Families

- build-owned owner artifact:
  - generated repo-superclean owner artifact selected by the checked-in build contract
- stress source summary:
  - generated stress source-surface output selected by the checked-in stress contract
- checked-in stress artifact/minimization contract:
  - `tests/tooling/fixtures/stress/artifact_surface.json`
- current machine-owned fuzz root:
  - generated fuzz-safety output root
- existing machine-owned validation roots that downstream work must reuse:
  - execution-smoke artifacts
  - conformance reports
  - runtime reports
  - runnable-toolchain package staging
- new machine-owned stress roots for downstream stress-validation work:
  - stress artifacts
  - stress reports
- machine-owned reducer and triage roots that downstream work must populate:
  - failure captures
  - minimized reproductions
  - replay captures
  - triage captures

## Exact Live Commands

- build the native toolchain before stress validation:
  - `npm run objc3c -- build-native-binaries`
- run the current parser/sema malformed-input gate:
  - `npm run objc3c -- test-fuzz-safety`
- run the bounded lowering/runtime stress harness:
  - `npm run objc3c -- test-lowering-runtime-stress`
- run the mixed-module and import/export differential stress harness:
  - `npm run objc3c -- test-mixed-module-differential`
- run the deterministic reducer/minimization pass over failing checked-in seeds:
  - `npm run objc3c -- test-stress-minimization`
- build crash-signature indexes and replay requests from the current minimized corpus:
  - `npm run objc3c -- test-stress-crash-triage`
- validate the checked-in stress source contract:
  - `npm run objc3c -- check-stress-surface`
- run the current executable smoke and replay evidence paths:
  - `npm run objc3c -- test-execution-smoke`
  - `npm run objc3c -- test-execution-replay`
- run the current conformance-corpus integration paths:
  - `npm run objc3c -- validate-conformance-corpus`
  - `npm run objc3c -- validate-runnable-conformance-corpus`
- stage the runnable package before packaged stress validation:
  - `npm run objc3c -- package-runnable-toolchain`

Helper implementations listed in live path sections are action-registry anchors.
Current-facing commands route through `npm run objc3c -- <action>`.

## Exact Live Paths For Downstream Work

- boundary and operator guidance:
  - `docs/runbooks/objc3c_stress_validation.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `docs/runbooks/objc3c_maintainer_workflows.md`
- fixture and corpus ownership:
  - `tests/tooling/fixtures/stress/source_surface.json`
  - `tests/tooling/fixtures/stress/safety_policy.json`
  - `tests/tooling/fixtures/stress/artifact_surface.json`
  - `tests/tooling/fixtures/native/`
  - `tests/tooling/fixtures/objc3c/`
  - `tests/conformance/corpus_surface.json`
  - `tests/conformance/README.md`
- compile and runtime execution roots:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- compile-objc3c`
  - `npm run objc3c -- test-execution-smoke`
  - `npm run objc3c -- test-execution-replay`
  - package bridge: `npm run objc3c -- <action>`
  - `npm run objc3c -- package-runnable-toolchain`

## Explicit Non-Goals

- no second fuzz corpus outside checked-in fixture and conformance roots
- no hidden random-input generator without checked-in seed or reducer rules
- no release-scope wrapper that bypasses the public compile or runtime path
- no publishable safety or coverage claim backed only by prose
- no GUI or separate service for crash triage, replay, or reducer execution
