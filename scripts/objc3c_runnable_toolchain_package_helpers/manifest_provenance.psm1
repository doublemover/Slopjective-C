Set-StrictMode -Version Latest

$manifestProvenanceModules = @(
  "core_toolchain.psm1",
  "application_package.psm1",
  "developer_tooling.psm1",
  "native_execution.psm1",
  "adoption_governance.psm1",
  "docs_showcase.psm1",
  "platform_hardening.psm1",
  "runtime_artifacts.psm1",
  "release_surfaces.psm1",
  "conformance_runtime_tests.psm1",
  "fixture_contracts.psm1",
  "native_fixtures.psm1",
  "release_foundation_fixtures.psm1",
  "performance_fixtures.psm1"
)

foreach ($manifestProvenanceModule in $manifestProvenanceModules) {
  Import-Module (Join-Path $PSScriptRoot (Join-Path "manifest_provenance" $manifestProvenanceModule)) -Force -DisableNameChecking
}

function Get-RequiredRunnableToolchainPackageFiles {
  return @(
    Get-ManifestProvenanceCoreToolchainFiles
    Get-ManifestProvenanceApplicationPackageFiles
    Get-ManifestProvenanceDeveloperToolingFiles
    Get-ManifestProvenanceNativeExecutionFiles
    Get-ManifestProvenanceAdoptionGovernanceFiles
    Get-ManifestProvenanceDocsShowcaseFiles
    Get-ManifestProvenancePlatformHardeningFiles
    Get-ManifestProvenanceRuntimeArtifactFiles
    Get-ManifestProvenanceReleaseSurfaceFiles
    Get-ManifestProvenanceConformanceRuntimeTestFiles
    Get-ManifestProvenanceFixtureContractFiles
    Get-ManifestProvenanceNativeSmokeFixtureFiles
    Get-ManifestProvenanceReleaseFoundationFixtureFiles
    Get-ManifestProvenanceNativeRuntimeFixtureFiles
    Get-ManifestProvenancePerformanceFixtureFiles
  )
}

Export-ModuleMember -Function @("Get-RequiredRunnableToolchainPackageFiles")
