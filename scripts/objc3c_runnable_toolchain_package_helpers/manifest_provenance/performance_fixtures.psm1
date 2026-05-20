Set-StrictMode -Version Latest

function Get-ManifestProvenancePerformanceFixtureFiles {
  return @(
    "tests/tooling/fixtures/performance/benchmark_portfolio.json",
    "tests/tooling/fixtures/performance/measurement_policy.json",
    "tests/tooling/fixtures/performance/benchmark_parameters.json",
    "tests/tooling/fixtures/performance/comparative_baseline_manifest.json",
    "tests/tooling/fixtures/performance_governance/budget_model.json",
    "tests/tooling/fixtures/performance/baselines/objc2_reference_workload.m",
    "tests/tooling/fixtures/performance/baselines/swift_reference_workload.swift",
    "tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp",
    "schemas/objc3c-performance-telemetry-v1.schema.json",
    "tests/tooling/fixtures/compiler_throughput/source_surface.json",
    "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
    "tests/tooling/fixtures/compiler_throughput/validation_tier_map.json",
    "tests/tooling/fixtures/compiler_throughput/optimization_policy.json",
    "tests/tooling/fixtures/compiler_throughput/artifact_surface.json",
    "schemas/objc3c-compiler-throughput-summary-v1.schema.json",
    "tests/tooling/fixtures/runtime_performance/source_surface.json",
    "tests/tooling/fixtures/runtime_performance/workload_manifest.json",
    "tests/tooling/fixtures/runtime_performance/executable_fixture_manifest.json",
    "tests/tooling/fixtures/runtime_performance/artifact_surface.json",
    "tests/tooling/fixtures/runtime_performance/optimization_policy.json",
    "tests/tooling/fixtures/runtime_performance/README.md",
    "schemas/objc3c-runtime-performance-telemetry-v1.schema.json"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenancePerformanceFixtureFiles")
