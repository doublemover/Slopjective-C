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
