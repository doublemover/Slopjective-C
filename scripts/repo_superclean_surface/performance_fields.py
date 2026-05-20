"""Performance surface fields for the repo superclean checker."""

from __future__ import annotations

from .model import SurfaceField


PERFORMANCE_SURFACE_FIELDS = (
    SurfaceField(
        "performance_benchmark_surface",
        {
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
    ),
    SurfaceField(
        "runtime_performance_surface",
        {
            "runbook": "docs/runbooks/objc3c_runtime_performance.md",
            "source_surface_contract": "tests/tooling/fixtures/runtime_performance/source_surface.json",
            "workload_manifest": "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
            "executable_fixture_manifest": "tests/tooling/fixtures/runtime_performance/executable_fixture_manifest.json",
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
    ),
    SurfaceField(
        "compiler_throughput_surface",
        {
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
    ),
    SurfaceField(
        "performance_governance_surface",
        {
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
    ),
)
