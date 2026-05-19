$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force

$closeoutArtifactModules = @(
  "objc3c_native_frontend_closeout_edge_artifacts.psm1",
  "objc3c_native_frontend_closeout_conformance_artifacts.psm1"
)

foreach ($closeoutArtifactModule in $closeoutArtifactModules) {
  $closeoutArtifactModulePath = Join-Path $PSScriptRoot $closeoutArtifactModule
  if (!(Test-Path -LiteralPath $closeoutArtifactModulePath -PathType Leaf)) {
    throw "frontend closeout artifact helper module missing: $closeoutArtifactModulePath"
  }

  . $closeoutArtifactModulePath
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativeFrontendEdgeCompatibilityArtifact",
  "Write-Objc3cNativeFrontendEdgeRobustnessArtifact",
  "Write-Objc3cNativeFrontendDiagnosticsHardeningArtifact",
  "Write-Objc3cNativeFrontendRecoveryDeterminismHardeningArtifact",
  "Write-Objc3cNativeFrontendConformanceMatrixArtifact",
  "Write-Objc3cNativeFrontendConformanceCorpusArtifact",
  "Write-Objc3cNativeFrontendIntegrationCloseoutArtifact"
)
