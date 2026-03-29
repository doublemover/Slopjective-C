# Objective-C 3 Public Command Surface

This runbook is generated from the live public workflow runner metadata.
It is an operator-facing appendix, not the primary onboarding or project-explanation surface.

- Current public script count: `67`
- Runner path: `scripts/objc3c_public_workflow_runner.py`
- Introspection command: `python scripts/objc3c_public_workflow_runner.py --list-json`
- Generator path: `scripts/render_objc3c_public_command_surface.py`

## Commands

| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |
| --- | --- | --- | --- | --- | --- |
| `build` | `build-default` | `-` | `-` | `fixed-shape` | `runner-internal` |
| `build:objc3c-native` | `build-native-binaries` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:contracts` | `build-native-contracts` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:full` | `build-native-full` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:objc3c-native:reconfigure` | `build-native-reconfigure` | `-` | `-` | `fixed-shape` | `pwsh:scripts/build_objc3c_native.ps1` |
| `build:site` | `build-site` | `-` | `-` | `fixed-shape` | `python:scripts/build_site_index.py + npx prettier` |
| `check:site` | `check-site` | `docs` | `published site index generation stays in sync with site/src inputs` | `fixed-shape` | `python:scripts/build_site_index.py --check` |
| `build:docs:native` | `build-native-docs` | `-` | `-` | `fixed-shape` | `python:scripts/build_objc3c_native_docs.py` |
| `check:docs:native` | `check-native-docs` | `docs` | `generated native implementation documentation stays in sync with docs/objc3c-native/src inputs` | `fixed-shape` | `python:scripts/build_objc3c_native_docs.py --check` |
| `build:docs:commands` | `build-public-command-surface` | `-` | `-` | `fixed-shape` | `python:scripts/render_objc3c_public_command_surface.py` |
| `check:docs:commands` | `check-public-command-surface` | `docs` | `operator-facing machine appendix stays in sync with the live workflow runner and package scripts` | `fixed-shape` | `python:scripts/render_objc3c_public_command_surface.py --check` |
| `check:docs:surface` | `check-documentation-surface` | `docs` | `reader-facing onboarding, site structure, and machine-appendix boundary stay accessible and explicit` | `fixed-shape` | `python:scripts/check_documentation_surface.py` |
| `check:showcase:surface` | `check-showcase-surface` | `repo` | `showcase examples stay compile-coupled, checked in, and tied to the public compiler path` | `pass-through` | `python:scripts/check_showcase_surface.py` |
| `test:showcase` | `validate-showcase` | `repo` | `showcase examples stay compiled, runnable, and wired into the normal repo validation path` | `fixed-shape` | `python:scripts/check_showcase_integration.py` |
| `test:showcase:e2e` | `validate-runnable-showcase` | `full` | `showcase examples stay publishable and runnable from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_showcase_end_to_end.py` |
| `test:getting-started` | `validate-getting-started` | `repo` | `getting-started tutorials stay compile-coupled, runnable, and wired into the normal repo validation path` | `fixed-shape` | `python:scripts/check_getting_started_integration.py` |
| `check:repo:surface` | `check-repo-superclean-surface` | `repo` | `native build emits the canonical repo-cleanup roots, outputs, and command names as one source-of-truth artifact` | `fixed-shape` | `python:scripts/check_repo_superclean_surface.py` |
| `test:docs` | `validate-documentation-surface` | `docs` | `site output, native docs, command appendix, and reader-facing onboarding remain buildable, in sync, and explicit` | `fixed-shape` | `runner-internal + generated documentation checks` |
| `test:repo` | `validate-repo-superclean` | `repo` | `repo roots, checked-in docs, generated outputs, and machine-owned boundaries remain canonical and enforced` | `fixed-shape` | `runner-internal + native build contracts + task hygiene gate` |
| `compile:objc3c` | `compile-objc3c` | `-` | `-` | `pass-through` | `pwsh:scripts/objc3c_native_compile.ps1` |
| `inspect:objc3c:capabilities` | `inspect-capability-explorer` | `repo` | `capability explorer payloads stay tied to the live LLVM probe and backend-routing contracts` | `pass-through` | `python:scripts/probe_objc3c_llvm_capabilities.py` |
| `inspect:objc3c:playground` | `inspect-playground-repro` | `repo` | `playground and repro payloads stay tied to the real frontend runner summary, emitted artifacts, and executable replay command` | `pass-through` | `runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe` |
| `inspect:objc3c:observability` | `inspect-compile-observability` | `repo` | `developer-facing compile observability stays tied to the real frontend runner summary and emitted artifacts` | `pass-through` | `runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe` |
| `inspect:objc3c:runtime` | `inspect-runtime-inspector` | `repo` | `developer-facing runtime inspection stays tied to the real emitted object artifact and runtime ABI boundary models` | `pass-through` | `runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe` |
| `trace:objc3c:stages` | `trace-compile-stages` | `repo` | `developer-facing compile stage traces stay tied to the real frontend runner stage summaries and process exit semantics` | `pass-through` | `runner-internal + artifacts/bin/objc3c-frontend-c-api-runner.exe` |
| `test:objc3c:developer-tooling` | `validate-developer-tooling` | `repo` | `developer-facing inspect and trace commands stay executable, artifact-backed, and tied to the live frontend runner` | `fixed-shape` | `python:scripts/check_objc3c_developer_tooling_integration.py` |
| `lint:spec` | `lint-spec` | `-` | `-` | `fixed-shape` | `python:scripts/spec_lint.py` |
| `test` | `test-default` | `-` | `-` | `fixed-shape` | `runner-internal` |
| `test:fast` | `test-fast` | `fast` | `runtime acceptance, canonical replay, and a bounded smoke slice` | `fixed-shape` | `runner-internal + targeted smoke slice` |
| `test:smoke` | `test-smoke` | `smoke` | `full execution smoke corpus` | `fixed-shape` | `runner-internal` |
| `test:ci` | `test-ci` | `ci` | `task hygiene, developer-tooling integration, showcase and tutorial onboarding validation, runtime acceptance, canonical replay, and full execution smoke validation` | `fixed-shape` | `runner-internal + direct task hygiene` |
| `test:objc3c` | `test-recovery` | `recovery` | `recovery compile success and deterministic recovery diagnostics` | `pass-through` | `pwsh:scripts/check_objc3c_native_recovery_contract.ps1` |
| `test:objc3c:execution-smoke` | `test-execution-smoke` | `smoke` | `compile/link/run execution behavior` | `pass-through` | `pwsh:scripts/check_objc3c_native_execution_smoke.ps1` |
| `test:objc3c:execution-replay-proof` | `test-execution-replay` | `full` | `replay and native-output truth` | `pass-through` | `pwsh:scripts/check_objc3c_execution_replay_proof.ps1` |
| `test:objc3c:runtime-acceptance` | `test-runtime-acceptance` | `fast` | `runtime acceptance and ABI/accessor proof` | `fixed-shape` | `python:scripts/check_objc3c_runtime_acceptance.py` |
| `proof:objc3c:runtime-architecture` | `proof-runtime-architecture` | `-` | `-` | `fixed-shape` | `python:scripts/check_objc3c_runtime_architecture_proof_packet.py` |
| `test:objc3c:runtime-architecture` | `validate-runtime-architecture` | `full` | `full public workflow and runtime architecture proof packet alignment` | `fixed-shape` | `python:scripts/check_objc3c_runtime_architecture_integration.py` |
| `test:objc3c:runnable-bootstrap` | `validate-runnable-bootstrap` | `full` | `packaged compile, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_bootstrap_end_to_end.py` |
| `test:objc3c:block-arc-conformance` | `validate-block-arc-conformance` | `full` | `integrated block/ARC conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_block_arc_conformance.py` |
| `test:objc3c:runnable-block-arc` | `validate-runnable-block-arc` | `full` | `packaged compile, block/ARC probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_block_arc_end_to_end.py` |
| `test:objc3c:concurrency-conformance` | `validate-concurrency-conformance` | `full` | `integrated async/task/executor/actor conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_concurrency_conformance.py` |
| `test:objc3c:runnable-concurrency` | `validate-runnable-concurrency` | `full` | `packaged compile, concurrency probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_concurrency_end_to_end.py` |
| `test:objc3c:object-model-conformance` | `validate-object-model-conformance` | `full` | `integrated object-model conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_object_model_conformance.py` |
| `test:objc3c:storage-reflection-conformance` | `validate-storage-reflection-conformance` | `full` | `integrated storage/accessor/reflection conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_storage_reflection_conformance.py` |
| `test:objc3c:runnable-object-model` | `validate-runnable-object-model` | `full` | `packaged compile, object-model probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_object_model_end_to_end.py` |
| `test:objc3c:runnable-storage-reflection` | `validate-runnable-storage-reflection` | `full` | `packaged compile, storage/reflection probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_storage_reflection_end_to_end.py` |
| `test:objc3c:error-conformance` | `validate-error-conformance` | `full` | `integrated error conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_error_conformance.py` |
| `test:objc3c:runnable-error` | `validate-runnable-error` | `full` | `packaged compile, error probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_error_end_to_end.py` |
| `test:objc3c:interop-conformance` | `validate-interop-conformance` | `full` | `integrated mixed-module runtime packaging and interop conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_interop_conformance.py` |
| `test:objc3c:runnable-interop` | `validate-runnable-interop` | `full` | `packaged compile, interop probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_interop_end_to_end.py` |
| `test:objc3c:metaprogramming-conformance` | `validate-metaprogramming-conformance` | `full` | `integrated metaprogramming conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_metaprogramming_conformance.py` |
| `test:objc3c:runnable-metaprogramming` | `validate-runnable-metaprogramming` | `full` | `packaged compile, metaprogramming probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_metaprogramming_end_to_end.py` |
| `test:objc3c:release-candidate-conformance` | `validate-release-candidate-conformance` | `full` | `integrated public-claims strict-profile and release-candidate conformance over the live runtime architecture workflow` | `fixed-shape` | `python:scripts/check_objc3c_runnable_release_candidate_conformance.py` |
| `test:objc3c:runnable-release-candidate` | `validate-runnable-release-candidate` | `full` | `packaged compile, release-candidate validation, runtime probe execution, smoke, and replay from the staged runnable toolchain bundle` | `fixed-shape` | `python:scripts/check_objc3c_runnable_release_candidate_end_to_end.py` |
| `test:objc3c:fixture-matrix` | `test-fixture-matrix` | `nightly` | `broad positive corpus artifact sanity` | `pass-through` | `pwsh:scripts/run_objc3c_native_fixture_matrix.ps1` |
| `test:objc3c:negative-expectations` | `test-negative-expectations` | `nightly` | `negative expectation header and token enforcement` | `pass-through` | `pwsh:scripts/check_objc3c_negative_fixture_expectations.ps1` |
| `test:objc3c:full` | `test-full` | `full` | `smoke, runtime acceptance, and replay without full recovery fan-out` | `fixed-shape` | `runner-internal + direct PowerShell suites` |
| `test:objc3c:nightly` | `test-nightly` | `nightly` | `full validation plus recovery and broad corpus sweeps` | `fixed-shape` | `runner-internal + direct PowerShell suites` |
| `package:objc3c-native:runnable-toolchain` | `package-runnable-toolchain` | `-` | `-` | `fixed-shape` | `pwsh:scripts/package_objc3c_runnable_toolchain.ps1` |
| `proof:objc3c` | `proof-objc3c` | `-` | `-` | `fixed-shape` | `pwsh:scripts/run_objc3c_native_compile_proof.ps1` |

## Operator Notes

- Use the package scripts above for normal operator workflows.
- Treat this file as a generated machine-facing appendix for exact command mapping, not as the reader-facing project introduction.
- Canonical user-facing command names come from `package.json` and map directly to `scripts/objc3c_public_workflow_runner.py` action names.
- Canonical checked-in doc outputs are `site/index.md`, `docs/objc3c-native.md`, and `docs/runbooks/objc3c_public_command_surface.md`; edit their source roots instead of the generated files.
- `native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots; `tmp/` and `artifacts/` are output roots, not naming roots.
- Composite validation entrypoints write an integrated runner summary to `tmp/reports/objc3c-public-workflow/<action>.json`.
- Those integrated summaries record the exact child-suite report paths emitted by smoke, replay, runtime-acceptance, and other live validation scripts.
- `compile:objc3c` and the fixture-backed suite commands accept pass-through arguments for bounded selectors.
- No additional package-script compatibility aliases remain supported.
- Maintainer-only package scripts are limited to repo hygiene and boundary checks.
