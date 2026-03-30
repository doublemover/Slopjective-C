#!/usr/bin/env python3
"""Validate the build-emitted repo superclean source-of-truth artifact."""

from __future__ import annotations

import json
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SURFACE_PATH = ROOT / "tmp" / "artifacts" / "objc3c-native" / "repo_superclean_source_of_truth.json"


def expect(condition: bool, message: str, errors: list[str]) -> None:
    if not condition:
        errors.append(message)


def main() -> int:
    errors: list[str] = []
    expect(SURFACE_PATH.is_file(), f"missing source-of-truth artifact: {SURFACE_PATH}", errors)
    if errors:
        print("repo-superclean-surface: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    payload = json.loads(SURFACE_PATH.read_text(encoding="utf-8"))
    expect(
        payload.get("contract_id")
        == "objc3c-repo-superclean-source-of-truth/build-wrapper-generated-surface-v1",
        "unexpected contract_id",
        errors,
    )
    expect(payload.get("schema_version") == 1, "unexpected schema_version", errors)
    expect(
        payload.get("surface_kind") == "native-build-wrapper-generated-source-of-truth",
        "unexpected surface_kind",
        errors,
    )
    expect(
        payload.get("implementation_roots") == ["native/objc3c", "scripts", "tests"],
        "implementation_roots drifted",
        errors,
    )
    expect(
        payload.get("checked_in_doc_sources")
        == ["README.md", "CONTRIBUTING.md", "showcase", "stdlib", "site/src", "docs/objc3c-native/src", "docs/runbooks", "package.json"],
        "checked_in_doc_sources drifted",
        errors,
    )
    expect(
        payload.get("generated_checked_in_outputs")
        == ["site/index.md", "docs/objc3c-native.md", "docs/runbooks/objc3c_public_command_surface.md"],
        "generated_checked_in_outputs drifted",
        errors,
    )
    expect(
        payload.get("maintainer_runbooks")
        == [
            "docs/runbooks/objc3c_maintainer_workflows.md",
            "docs/runbooks/objc3c_developer_tooling.md",
            "docs/runbooks/objc3c_bonus_experiences.md",
            "docs/runbooks/objc3c_performance.md",
            "docs/runbooks/objc3c_stdlib_foundation.md",
            "docs/runbooks/objc3c_stdlib_core.md",
        ],
        "maintainer_runbooks drifted",
        errors,
    )
    expect(
        payload.get("machine_owned_output_roots") == ["tmp", "artifacts"],
        "machine_owned_output_roots drifted",
        errors,
    )

    package_entrypoints = payload.get("package_entrypoints", {})
    expect(
        package_entrypoints == {
            "build_native": "build:objc3c-native",
            "build_contracts": "build:objc3c-native:contracts",
            "build_full": "build:objc3c-native:full",
            "build_reconfigure": "build:objc3c-native:reconfigure",
            "build_site": "build:site",
            "check_site": "check:site",
            "build_native_docs": "build:docs:native",
            "build_public_command_surface": "build:docs:commands",
            "build_stdlib": "build:objc3c:stdlib",
            "check_stdlib_surface": "check:stdlib:surface",
            "test_stdlib": "test:stdlib",
            "test_stdlib_e2e": "test:stdlib:e2e",
            "test_fast": "test:fast",
            "test_ci": "test:ci",
            "test_docs": "test:docs",
        },
        "package_entrypoints drifted",
        errors,
    )

    native_build_outputs = payload.get("native_build_outputs", {})
    expect(
        native_build_outputs == {
            "native_executable": "artifacts/bin/objc3c-native.exe",
            "frontend_c_api_runner": "artifacts/bin/objc3c-frontend-c-api-runner.exe",
            "runtime_library": "artifacts/lib/objc3_runtime.lib",
            "compile_commands": "tmp/build-objc3c-native/compile_commands.json",
        },
        "native_build_outputs drifted",
        errors,
    )

    bonus_experience_surfaces = payload.get("bonus_experience_surfaces", {})
    expect(
        bonus_experience_surfaces
        == {
            "playground": {
                "source_roots": [
                    "showcase/auroraBoard/main.objc3",
                    "showcase/signalMesh/main.objc3",
                    "showcase/patchKit/main.objc3",
                    "tests/tooling/fixtures/native/hello.objc3",
                ],
                "artifact_roots": [
                    "tmp/artifacts/playground",
                    "tmp/reports/playground",
                    "tmp/artifacts/showcase",
                ],
                "public_actions": [
                    "materialize-playground-workspace",
                    "compile-objc3c",
                    "inspect-playground-repro",
                    "inspect-compile-observability",
                    "trace-compile-stages",
                ],
            },
            "runtime_inspector": {
                "source_roots": [
                    "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
                    "scripts/probe_objc3c_llvm_capabilities.py",
                    "native/objc3c/src/runtime/objc3_runtime.cpp",
                    "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
                    "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
                    "tests/tooling/runtime/task_runtime_hardening_probe.cpp",
                ],
                "report_roots": [
                    "tmp/reports/objc3c-public-workflow",
                    "tmp/reports/developer-tooling",
                ],
                "public_actions": [
                    "inspect-runtime-inspector",
                    "inspect-capability-explorer",
                    "benchmark-runtime-inspector",
                    "trace-compile-stages",
                    "validate-developer-tooling",
                ],
            },
            "template_and_demo_harness": {
                "source_roots": [
                    "scripts/materialize_objc3c_project_template.py",
                    "showcase/README.md",
                    "showcase/portfolio.json",
                    "showcase/tutorial_walkthrough.json",
                    "docs/tutorials/getting_started.md",
                    "docs/tutorials/build_run_verify.md",
                    "docs/tutorials/guided_walkthrough.md",
                ],
                "report_roots": [
                    "tmp/artifacts/project-template",
                    "tmp/reports/project-template",
                    "tmp/reports/showcase",
                    "tmp/reports/tutorials",
                ],
                "public_actions": [
                    "materialize-project-template",
                    "validate-showcase",
                    "validate-runnable-showcase",
                    "validate-getting-started",
                ],
            },
        },
        "bonus_experience_surfaces drifted",
        errors,
    )
    expect(
        payload.get("bonus_tool_integration_surface")
        == {
            "source_of_truth_artifact": "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json",
            "report_root": "tmp/reports/objc3c-public-workflow",
            "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
            "portfolio_contract": "showcase/portfolio.json",
            "guided_walkthrough_manifest": "showcase/tutorial_walkthrough.json",
            "public_actions": [
                "inspect-bonus-tool-integration",
                "materialize-project-template",
                "materialize-playground-workspace",
                "benchmark-runtime-inspector",
                "validate-showcase",
                "validate-runnable-showcase",
                "validate-getting-started",
                "package-runnable-toolchain",
            ],
        },
        "bonus_tool_integration_surface drifted",
        errors,
    )
    expect(
        payload.get("performance_benchmark_surface")
        == {
            "benchmark_portfolio": "tests/tooling/fixtures/performance/benchmark_portfolio.json",
            "measurement_policy": "tests/tooling/fixtures/performance/measurement_policy.json",
            "benchmark_parameters": "tests/tooling/fixtures/performance/benchmark_parameters.json",
            "comparative_baseline_manifest": "tests/tooling/fixtures/performance/comparative_baseline_manifest.json",
            "telemetry_schema": "schemas/objc3c-performance-telemetry-v1.schema.json",
            "source_roots": [
                "showcase/auroraBoard/main.objc3",
                "showcase/signalMesh/main.objc3",
                "showcase/patchKit/main.objc3",
                "tests/tooling/fixtures/performance/baselines/objc2_reference_workload.m",
                "tests/tooling/fixtures/performance/baselines/swift_reference_workload.swift",
                "tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp",
            ],
            "report_roots": [
                "tmp/artifacts/performance",
                "tmp/reports/performance",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "public_actions": [
                "benchmark-performance",
                "benchmark-comparative-baselines",
                "validate-runnable-performance",
                "package-runnable-toolchain",
            ],
        },
        "performance_benchmark_surface drifted",
        errors,
    )
    expect(
        payload.get("stdlib_foundation_surface")
        == {
            "workspace_contract": "stdlib/workspace.json",
            "module_inventory": "stdlib/module_inventory.json",
            "stability_policy": "stdlib/stability_policy.json",
            "package_surface": "stdlib/package_surface.json",
            "core_architecture": "stdlib/core_architecture.json",
            "semantic_policy": "stdlib/semantic_policy.json",
            "lowering_import_surface": "stdlib/lowering_import_surface.json",
            "source_roots": [
                "stdlib/README.md",
                "stdlib/modules/objc3.core/module.objc3",
                "stdlib/modules/objc3.errors/module.objc3",
                "stdlib/modules/objc3.concurrency/module.objc3",
                "stdlib/modules/objc3.keypath/module.objc3",
                "stdlib/modules/objc3.system/module.objc3",
            ],
            "report_roots": [
                "tmp/artifacts/stdlib",
                "tmp/reports/stdlib",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "public_actions": [
                "check-stdlib-surface",
                "materialize-stdlib-workspace",
                "validate-stdlib-foundation",
                "validate-runnable-stdlib-foundation",
                "package-runnable-toolchain",
            ],
        },
        "stdlib_foundation_surface drifted",
        errors,
    )

    frontend_contract_artifacts = payload.get("frontend_contract_artifacts", [])
    expect(isinstance(frontend_contract_artifacts, list) and frontend_contract_artifacts, "frontend_contract_artifacts missing", errors)
    artifact_names = [entry.get("name") for entry in frontend_contract_artifacts if isinstance(entry, dict)]
    expect(
        artifact_names
        == [
            "frontend_source_graph",
            "frontend_invocation_lock",
            "frontend_core_feature_expansion",
            "frontend_edge_compat",
            "frontend_edge_robustness",
            "frontend_diagnostics_hardening",
            "frontend_recovery_determinism_hardening",
            "frontend_conformance_matrix",
            "frontend_conformance_corpus",
            "frontend_integration_closeout",
        ],
        "frontend contract artifact inventory drifted",
        errors,
    )

    explicit_non_goals = payload.get("explicit_non_goals")
    expect(
        explicit_non_goals
        == [
            "no milestone-coded command aliases",
            "no secondary source-of-truth directories",
            "no generated-output hand edits",
        ],
        "explicit_non_goals drifted",
        errors,
    )

    if errors:
        print("repo-superclean-surface: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    print("repo-superclean-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
