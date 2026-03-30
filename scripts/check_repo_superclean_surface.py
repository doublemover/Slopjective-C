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
            "docs/runbooks/objc3c_performance_governance.md",
            "docs/runbooks/objc3c_packaging_channels.md",
            "docs/runbooks/objc3c_release_foundation.md",
            "docs/runbooks/objc3c_runtime_performance.md",
            "docs/runbooks/objc3c_stress_validation.md",
            "docs/runbooks/objc3c_stdlib_foundation.md",
            "docs/runbooks/objc3c_stdlib_core.md",
            "docs/runbooks/objc3c_stdlib_advanced.md",
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
            "inspect_performance_dashboard": "inspect:objc3c:performance-dashboard",
            "publish_performance_report": "publish:objc3c:performance-report",
            "test_performance_governance": "test:objc3c:performance-governance",
            "inspect_release_manifest": "inspect:objc3c:release-manifest",
            "publish_release_provenance": "publish:objc3c:release-provenance",
            "test_release_foundation": "test:objc3c:release-foundation",
            "package_channels": "package:objc3c:channels",
            "check_packaging_channels_surface": "check:objc3c:packaging-channels:surface",
            "check_packaging_channels_schemas": "check:objc3c:packaging-channels:schemas",
            "test_packaging_channels": "test:objc3c:packaging-channels",
            "test_packaging_channels_e2e": "test:objc3c:packaging-channels:e2e",
            "inspect_runtime_performance": "inspect:objc3c:runtime-performance",
            "test_runtime_performance": "test:objc3c:runtime-performance",
            "test_runtime_performance_e2e": "test:objc3c:runnable-runtime-performance",
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
        payload.get("runtime_performance_surface")
        == {
            "runbook": "docs/runbooks/objc3c_runtime_performance.md",
            "source_surface_contract": "tests/tooling/fixtures/runtime_performance/source_surface.json",
            "workload_manifest": "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
            "artifact_surface_contract": "tests/tooling/fixtures/runtime_performance/artifact_surface.json",
            "optimization_policy": "tests/tooling/fixtures/runtime_performance/optimization_policy.json",
            "telemetry_schema": "schemas/objc3c-runtime-performance-telemetry-v1.schema.json",
            "source_readme": "tests/tooling/fixtures/runtime_performance/README.md",
            "source_roots": [
                "native/objc3c/src/runtime/objc3_runtime.cpp",
                "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
                "scripts/benchmark_objc3c_runtime_performance.py",
                "scripts/check_objc3c_runtime_acceptance.py",
                "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
                "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
                "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
                "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
                "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
            ],
            "report_roots": [
                "tmp/artifacts/runtime-performance",
                "tmp/reports/runtime-performance",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "public_actions": [
                "inspect-runtime-inspector",
                "benchmark-runtime-performance",
                "validate-runtime-performance",
                "validate-runnable-runtime-performance",
                "package-runnable-toolchain",
            ],
        },
        "runtime_performance_surface drifted",
        errors,
    )
    expect(
        payload.get("compiler_throughput_surface")
        == {
            "runbook": "docs/runbooks/objc3c_compiler_throughput.md",
            "source_surface_contract": "tests/tooling/fixtures/compiler_throughput/source_surface.json",
            "workload_manifest": "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
            "validation_tier_map": "tests/tooling/fixtures/compiler_throughput/validation_tier_map.json",
            "optimization_policy": "tests/tooling/fixtures/compiler_throughput/optimization_policy.json",
            "artifact_surface_contract": "tests/tooling/fixtures/compiler_throughput/artifact_surface.json",
            "summary_schema": "schemas/objc3c-compiler-throughput-summary-v1.schema.json",
            "source_roots": [
                "scripts/check_objc3c_native_perf_budget.ps1",
                "scripts/check_objc3c_native_execution_smoke.ps1",
                "scripts/check_objc3c_native_recovery_contract.ps1",
                "scripts/check_objc3c_execution_replay_proof.ps1",
                "scripts/run_objc3c_native_fixture_matrix.ps1",
                "scripts/check_objc3c_negative_fixture_expectations.ps1",
                "scripts/build_objc3c_native_docs.py",
                "scripts/render_objc3c_public_command_surface.py",
            ],
            "report_roots": [
                "tmp/artifacts/objc3c-native/perf-budget",
                "tmp/reports/compiler-throughput",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "public_actions": [
                "benchmark-compiler-throughput",
                "validate-compiler-throughput",
                "validate-runnable-compiler-throughput",
                "package-runnable-toolchain",
            ],
        },
        "compiler_throughput_surface drifted",
        errors,
    )
    expect(
        payload.get("performance_governance_surface")
        == {
            "runbook": "docs/runbooks/objc3c_performance_governance.md",
            "source_surface_contract": "tests/tooling/fixtures/performance_governance/source_surface.json",
            "budget_model": "tests/tooling/fixtures/performance_governance/budget_model.json",
            "claim_policy": "tests/tooling/fixtures/performance_governance/claim_policy.json",
            "breach_triage_policy": "tests/tooling/fixtures/performance_governance/breach_triage_policy.json",
            "lab_policy": "tests/tooling/fixtures/performance_governance/lab_policy.json",
            "waiver_registry": "tests/tooling/fixtures/performance_governance/waivers.json",
            "workflow_surface": "tests/tooling/fixtures/performance_governance/workflow_surface.json",
            "schema_surface": "tests/tooling/fixtures/performance_governance/schema_surface.json",
            "dashboard_schema": "schemas/objc3c-performance-dashboard-summary-v1.schema.json",
            "public_report_schema": "schemas/objc3c-performance-public-report-v1.schema.json",
            "source_roots": [
                "scripts/check_performance_governance_source_surface.py",
                "scripts/check_performance_governance_schema_surface.py",
                "scripts/build_objc3c_performance_dashboard.py",
                "scripts/publish_objc3c_performance_report.py",
                "scripts/check_objc3c_performance_governance_integration.py",
                "scripts/check_objc3c_performance_governance_end_to_end.py",
            ],
            "report_roots": [
                "tmp/reports/performance-governance",
                "tmp/artifacts/performance-governance",
                "tmp/reports/objc3c-public-workflow",
            ],
            "public_actions": [
                "check-performance-governance-surface",
                "check-performance-governance-schema-surface",
                "build-performance-dashboard",
                "publish-performance-report",
                "validate-performance-governance",
                "validate-performance-governance-integration",
                "validate-performance-governance-end-to-end",
            ],
        },
        "performance_governance_surface drifted",
        errors,
    )
    expect(
        payload.get("release_foundation_surface")
        == {
            "runbook": "docs/runbooks/objc3c_release_foundation.md",
            "source_surface_contract": "tests/tooling/fixtures/release_foundation/source_surface.json",
            "artifact_taxonomy": "tests/tooling/fixtures/release_foundation/artifact_taxonomy.json",
            "distribution_trust_model": "tests/tooling/fixtures/release_foundation/distribution_trust_model.json",
            "distribution_audit": "tests/tooling/fixtures/release_foundation/distribution_audit.json",
            "reproducibility_policy": "tests/tooling/fixtures/release_foundation/reproducibility_policy.json",
            "release_payload_policy": "tests/tooling/fixtures/release_foundation/release_payload_policy.json",
            "provenance_policy": "tests/tooling/fixtures/release_foundation/provenance_policy.json",
            "workflow_surface": "tests/tooling/fixtures/release_foundation/workflow_surface.json",
            "schema_surface": "tests/tooling/fixtures/release_foundation/schema_surface.json",
            "release_manifest_schema": "schemas/objc3c-release-manifest-v1.schema.json",
            "release_sbom_schema": "schemas/objc3c-release-sbom-v1.schema.json",
            "release_attestation_schema": "schemas/objc3c-release-attestation-v1.schema.json",
            "source_roots": [
                "scripts/check_release_foundation_source_surface.py",
                "scripts/check_release_foundation_schema_surface.py",
                "scripts/build_objc3c_release_manifest.py",
                "scripts/publish_objc3c_release_provenance.py",
                "scripts/check_objc3c_release_foundation_integration.py",
                "scripts/package_objc3c_runnable_toolchain.ps1",
                "scripts/check_release_evidence.py",
            ],
            "report_roots": [
                "tmp/reports/release-foundation",
                "tmp/artifacts/release-foundation",
                "tmp/pkg/objc3c-release-foundation",
            ],
            "public_actions": [
                "check-release-foundation-surface",
                "check-release-foundation-schema-surface",
                "build-release-manifest",
                "publish-release-provenance",
                "validate-release-foundation",
            ],
        },
        "release_foundation_surface drifted",
        errors,
    )
    expect(
        payload.get("packaging_channels_surface")
        == {
            "runbook": "docs/runbooks/objc3c_packaging_channels.md",
            "source_surface_contract": "tests/tooling/fixtures/packaging_channels/source_surface.json",
            "supported_platforms": "tests/tooling/fixtures/packaging_channels/supported_platforms.json",
            "installer_policy": "tests/tooling/fixtures/packaging_channels/installer_policy.json",
            "metadata_surface": "tests/tooling/fixtures/packaging_channels/metadata_surface.json",
            "workflow_surface": "tests/tooling/fixtures/packaging_channels/workflow_surface.json",
            "schema_surface": "tests/tooling/fixtures/packaging_channels/schema_surface.json",
            "package_channels_manifest_schema": "schemas/objc3c-package-channels-manifest-v1.schema.json",
            "install_receipt_schema": "schemas/objc3c-package-install-receipt-v1.schema.json",
            "source_roots": [
                "scripts/check_packaging_channels_source_surface.py",
                "scripts/check_packaging_channels_schema_surface.py",
                "scripts/build_objc3c_package_channels.py",
                "scripts/check_objc3c_packaging_channels_integration.py",
                "scripts/check_objc3c_packaging_channels_end_to_end.py",
                "scripts/package_objc3c_runnable_toolchain.ps1",
                "scripts/build_objc3c_release_manifest.py",
                "scripts/publish_objc3c_release_provenance.py",
            ],
            "report_roots": [
                "tmp/reports/package-channels",
                "tmp/artifacts/package-channels",
                "tmp/pkg/objc3c-package-channels",
            ],
            "public_actions": [
                "check-packaging-channels-surface",
                "check-packaging-channels-schema-surface",
                "build-package-channels",
                "validate-packaging-channels",
                "validate-packaging-channels-end-to-end",
            ],
        },
        "packaging_channels_surface drifted",
        errors,
    )
    expect(
        payload.get("stress_validation_surface")
        == {
            "source_surface_contract": "tests/tooling/fixtures/stress/source_surface.json",
            "artifact_surface_contract": "tests/tooling/fixtures/stress/artifact_surface.json",
            "source_readme": "tests/tooling/fixtures/stress/README.md",
            "safety_policy": "tests/tooling/fixtures/stress/safety_policy.json",
            "runbook": "docs/runbooks/objc3c_stress_validation.md",
            "source_check_script": "scripts/check_stress_source_surface.py",
            "checked_in_roots": [
                "tests/tooling/fixtures/stress",
                "tests/tooling/fixtures/native",
                "tests/tooling/fixtures/objc3c",
                "tests/tooling/fixtures/parser_conformance_corpus",
                "tests/conformance",
            ],
            "source_family_ids": [
                "parser-sema-fuzz",
                "lowering-runtime-stress",
                "mixed-module-differential",
                "replay-backed-contracts",
            ],
        },
        "stress_validation_surface drifted",
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
            "advanced_architecture": "stdlib/advanced_architecture.json",
            "semantic_policy": "stdlib/semantic_policy.json",
            "lowering_import_surface": "stdlib/lowering_import_surface.json",
            "advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json",
            "source_roots": [
                "stdlib/README.md",
                "stdlib/advanced_helper_package_surface.json",
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
                "validate-stdlib-advanced",
                "validate-runnable-stdlib-advanced",
                "validate-runnable-stdlib-foundation",
                "package-runnable-toolchain",
            ],
        },
        "stdlib_foundation_surface drifted",
        errors,
    )
    expect(
        payload.get("stdlib_program_surface")
        == {
            "program_contract": "stdlib/program_surface.json",
            "stdlib_readme": "stdlib/README.md",
            "runbook": "docs/runbooks/objc3c_stdlib_program.md",
            "site_entry": "site/src/index.body.md",
            "publish_inputs": [
                "stdlib/README.md",
                "docs/runbooks/objc3c_stdlib_program.md",
                "docs/tutorials/README.md",
                "docs/tutorials/getting_started.md",
                "docs/tutorials/objc2_swift_cpp_comparison.md",
                "showcase/README.md",
                "showcase/portfolio.json",
                "showcase/tutorial_walkthrough.json",
                "site/src/index.body.md",
            ],
            "report_roots": [
                "tmp/artifacts/stdlib",
                "tmp/reports/stdlib",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "workflow_surface": {
                "report_root": "tmp/reports/stdlib",
                "showcase_report_root": "tmp/reports/showcase",
                "tutorial_report_root": "tmp/reports/tutorials",
                "integration_entrypoint": "validate-stdlib-program",
                "packaged_validation_entrypoint": "validate-runnable-stdlib-program",
                "integration_actions": [
                    "check-documentation-surface",
                    "validate-getting-started",
                    "validate-showcase",
                    "validate-stdlib-foundation",
                    "inspect-capability-explorer",
                ],
                "release_actions": [
                    "validate-runnable-showcase",
                    "validate-runnable-stdlib-foundation",
                    "package-runnable-toolchain",
                ],
            },
            "public_actions": [
                "check-documentation-surface",
                "check-showcase-surface",
                "validate-getting-started",
                "validate-showcase",
                "validate-runnable-showcase",
                "validate-stdlib-foundation",
                "validate-runnable-stdlib-foundation",
                "validate-stdlib-program",
                "validate-runnable-stdlib-program",
                "inspect-capability-explorer",
                "package-runnable-toolchain",
            ],
        },
        "stdlib_program_surface drifted",
        errors,
    )
    expect(
        payload.get("conformance_corpus_surface")
        == {
            "corpus_contract": "tests/conformance/corpus_surface.json",
            "suite_readme": "tests/conformance/README.md",
            "coverage_map": "tests/conformance/COVERAGE_MAP.md",
            "runbook": "docs/runbooks/objc3c_conformance_corpus.md",
            "longitudinal_manifest": "tests/conformance/longitudinal_suites.json",
            "report_roots": [
                "tmp/artifacts/conformance",
                "tmp/reports/conformance",
                "tmp/pkg/objc3c-native-runnable-toolchain",
            ],
            "workflow_surface": {
                "report_root": "tmp/reports/conformance",
                "artifact_root": "tmp/artifacts/conformance",
                "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
                "surface_check_script": "scripts/check_conformance_corpus_surface.py",
                "coverage_index_script": "scripts/generate_conformance_corpus_index.py",
                "legacy_suite_gate_script": "scripts/check_conformance_suite.ps1",
                "coverage_map": "tests/conformance/COVERAGE_MAP.md",
                "longitudinal_suite_manifest": "tests/conformance/longitudinal_suites.json",
            },
        },
        "conformance_corpus_surface drifted",
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
