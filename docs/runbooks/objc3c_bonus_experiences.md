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

Downstream `M292` issues must stay on the existing implementation paths below
instead of inventing a sidecar app, milestone-local launcher, or proof-only
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
  - `scripts/objc3c_public_workflow_runner.py`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- native compiler/runtime tooling entrypoints:
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- native build and package wiring:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/package_objc3c_runnable_toolchain.ps1`
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
  - `scripts/check_objc3c_developer_tooling_integration.py`
- current showcase/tutorial validation paths:
  - `scripts/check_showcase_integration.py`
  - `scripts/check_objc3c_runnable_showcase_end_to_end.py`
  - `scripts/check_getting_started_integration.py`

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
- build-emitted source-of-truth artifact:
  - `tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json`
- playground/repro artifact roots:
  - `tmp/artifacts/compilation/objc3c-native/`
  - `tmp/artifacts/showcase/`
- runtime inspector and stage-trace report roots:
  - `tmp/reports/objc3c-public-workflow/compile-observability.json`
  - `tmp/reports/objc3c-public-workflow/runtime-inspector.json`
  - `tmp/reports/objc3c-public-workflow/compile-stage-trace.json`
  - `tmp/reports/developer-tooling/integration-summary.json`
- showcase/tutorial validation report roots:
  - `tmp/reports/showcase/`
  - `tmp/reports/tutorials/`

## Exact Live Commands

- compile one checked-in source through the public compiler path:
  - `python scripts/objc3c_public_workflow_runner.py compile-objc3c showcase/auroraBoard/main.objc3`
  - `npm run compile:objc3c -- showcase/auroraBoard/main.objc3`
- dump the live playground and repro payload:
  - `python scripts/objc3c_public_workflow_runner.py inspect-playground-repro`
  - `npm run inspect:objc3c:playground`
- dump the current playground/repro observability payload:
  - `python scripts/objc3c_public_workflow_runner.py inspect-compile-observability`
  - `npm run inspect:objc3c:observability`
- dump the current runtime-inspector payload:
  - `python scripts/objc3c_public_workflow_runner.py inspect-runtime-inspector`
  - `npm run inspect:objc3c:runtime`
- dump the current capability-explorer payload:
  - `python scripts/objc3c_public_workflow_runner.py inspect-capability-explorer`
  - `npm run inspect:objc3c:capabilities`
- dump the current stage-trace payload:
  - `python scripts/objc3c_public_workflow_runner.py trace-compile-stages`
  - `npm run trace:objc3c:stages`
- validate the current developer-tooling integration surface:
  - `python scripts/objc3c_public_workflow_runner.py validate-developer-tooling`
  - `npm run test:objc3c:developer-tooling`
- validate showcase/tutorial-backed bonus experience flows:
  - `python scripts/objc3c_public_workflow_runner.py validate-showcase`
  - `python scripts/objc3c_public_workflow_runner.py validate-runnable-showcase`
  - `python scripts/objc3c_public_workflow_runner.py validate-getting-started`

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

- keep public command routing in `scripts/objc3c_public_workflow_runner.py`
  and `package.json`
- keep checked-in bonus-experience guidance in `docs/runbooks/`
- keep runnable example sources under `showcase/` or existing tutorial/example
  roots
- keep generated reports, traces, and captures under `tmp/`
- prove bonus-experience behavior through the live showcase, tutorial, and
  developer-tooling validation paths before widening the surface

## Explicit Non-Goals

- no milestone-local playground shell
- no sidecar web service or hidden local daemon
- no synthetic runtime-inspector payloads treated as source of truth
- no duplicate example/template inventory outside checked-in showcase/tutorial
  roots
- no hand-authored report snapshots under checked-in doc roots
