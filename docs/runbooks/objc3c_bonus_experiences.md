# objc3c Bonus Experiences Boundary

## Working Boundary

This runbook defines the live boundary for bonus experiences that sit on top of
the checked-in compiler, runtime, showcase, tutorial, and developer-tooling
surfaces.

Use it when changing:

- interactive playground flows
- repro-runner and demo-harness flows
- visual runtime inspection and capability-explorer flows
- starter-template and project-generator flows

Downstream bonus-experience work must stay on the existing implementation paths below
instead of inventing a sidecar app, release-scope launcher, or evidence-only
demo surface.

## Current Truthful Portfolio

The bonus-experience portfolio is currently constrained to three real surfaces:

- playground and repro flows built on the public compiler runner and the
  checked-in showcase/tutorial sources
- runtime inspection and capability exploration built on the live frontend C
  API runner dumps plus the existing runtime/debug ABI snapshots
- template and demo-harness flows built on checked-in showcase examples,
  tutorial assets, and the public packaging/build workflow

There is no separate GUI app, hosted playground service, or standalone product
shell today. Downstream work must extend the current repo-owned surfaces until a
real implementation exists.

## Exact Live Implementation Paths

- public command and package surface:
  - package bridge: `npm run objc3c -- <action>`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- native compiler/runtime tooling entrypoints:
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner_*.{h,cpp}`
  - `native/objc3c/src/runtime/{classes,dispatch,images,selectors,state}/`
  - `native/objc3c/src/artifacts/`
- native build and package wiring:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- package-runnable-toolchain`
- showcase and tutorial roots:
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
  - `docs/tutorials/getting_started.md`
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/guided_walkthrough.md`
- current developer-tooling boundary:
  - `docs/runbooks/objc3c_developer_tooling.md`
  - `npm run objc3c -- validate-developer-tooling`
  - `npm run objc3c -- validate-bonus-experiences`
- current showcase/tutorial validation paths:
  - `npm run objc3c -- validate-showcase`
  - `npm run objc3c -- validate-runnable-showcase`
  - `npm run objc3c -- validate-getting-started`
  - `npm run objc3c -- validate-runnable-bonus-experiences`

## Exact Playground Inspector And Template Paths

- playground compile/repro source roots:
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
  - `tests/tooling/fixtures/native/hello.objc3`
- runtime-inspector and capability-explorer source roots:
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
  - `tests/tooling/runtime/arc_debug_instrumentation_probe.cpp`
  - `tests/tooling/runtime/block_arc_runtime_abi_probe.cpp`
  - `tests/tooling/runtime/task_runtime_hardening_probe.cpp`
- template and demo-harness source roots:
  - `npm run objc3c -- materialize-project-template`
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/tutorial_walkthrough.json`
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/guided_walkthrough.md`

## Exact Live Artifact And Output Paths

- native binaries and runtime library:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`
- build-emitted owner artifact:
  - `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json`
- playground/repro artifact roots:
  - `tmp/artifacts/playground/`
  - `tmp/reports/playground/`
  - `tmp/artifacts/showcase/`
- runtime inspector and stage-trace report roots:
  - `tmp/reports/objc3c-public-workflow/compile-observability.json`
  - `tmp/reports/objc3c-public-workflow/runtime-inspector.json`
  - `tmp/reports/objc3c-public-workflow/compile-stage-trace.json`
  - `tmp/reports/developer-tooling/integration-summary.json`
- showcase/tutorial validation report roots:
  - `tmp/artifacts/project-template/`
  - `tmp/reports/project-template/`
  - `tmp/reports/showcase/`
  - `tmp/reports/tutorials/`
- staged package manifest within a runnable package root:
  - `artifacts/package/objc3c-runnable-toolchain-package.json`

## Exact Live Commands

- compile one checked-in source through the public compiler path:
  - `npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3`
- materialize a machine-owned playground workspace for one source:
  - `npm run objc3c -- materialize-playground-workspace showcase/auroraBoard/main.objc3`
- materialize a machine-owned project template and demo harness from one showcase example:
  - `npm run objc3c -- materialize-project-template --example auroraBoard`
- stage the runnable toolchain and package manifest surface:
  - `npm run objc3c -- package-runnable-toolchain`
- inspect the live integrated bonus-tool surface:
  - `npm run objc3c -- inspect-bonus-tool-integration`
- dump the live playground and repro payload:
  - `npm run objc3c -- inspect-playground-repro`
- dump the current playground/repro observability payload:
  - `npm run objc3c -- inspect-compile-observability`
- dump the current runtime-inspector payload:
  - `npm run objc3c -- inspect-runtime-inspector`
- dump the current capability-explorer payload:
  - `npm run objc3c -- inspect-capability-explorer`
- benchmark the runtime-inspector and capability-explorer workflow:
  - `npm run objc3c -- benchmark-runtime-inspector`
- dump the current stage-trace payload:
  - `npm run objc3c -- trace-compile-stages`
- validate the current developer-tooling integration surface:
  - `npm run objc3c -- validate-developer-tooling`
- validate showcase/tutorial-backed bonus experience flows:
  - `npm run objc3c -- validate-bonus-experiences`
  - `npm run objc3c -- validate-showcase`
  - `npm run objc3c -- validate-runnable-showcase`
  - `npm run objc3c -- validate-getting-started`
  - `npm run objc3c -- validate-runnable-bonus-experiences`

Helper implementations are action-registry anchors, not separate
current-facing commands.

## Feasibility And Working Model

The current live implementation is sufficient to support bonus experiences
without inventing a second product stack because it already exposes:

- compile, observability, runtime-inspector, and stage-trace dumps from the
  real frontend runner
- runnable checked-in examples and tutorial flows that already compile and run
  through the real toolchain
- package/build flows that can stage examples and validation artifacts under
  `tmp/` and `artifacts/`

Downstream work must compose those surfaces. It must not treat screenshots,
mock JSON, or hand-written walkthrough output as authoritative.

## Working Rules For Downstream Issues

- keep public command routing through the `package.json` bridge:
  `npm run objc3c -- <action>`
- keep checked-in bonus-experience guidance in `docs/runbooks/`
- keep runnable example sources under `showcase/` or existing tutorial/example
  roots
- keep generated reports, traces, and captures under `tmp/`
- prove bonus-experience behavior through the live showcase, tutorial, and
  developer-tooling validation paths before widening the surface

## Explicit Non-Goals

- no release-scope playground shell
- no sidecar web service or hidden local daemon
- no synthetic runtime-inspector payloads treated as owner evidence
- no duplicate example/template inventory outside checked-in showcase/tutorial
  roots
- no hand-authored report snapshots under checked-in doc roots
