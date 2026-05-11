$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force

$closeoutEdgeArtifactRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_closeout_edge_artifacts"
$closeoutEdgeArtifactModules = @(
  "constants.psm1",
  "loading.psm1",
  "payloads.psm1",
  "orchestration.psm1"
)

foreach ($closeoutEdgeArtifactModule in $closeoutEdgeArtifactModules) {
  $closeoutEdgeArtifactModulePath = Join-Path $closeoutEdgeArtifactRoot $closeoutEdgeArtifactModule
  if (!(Test-Path -LiteralPath $closeoutEdgeArtifactModulePath -PathType Leaf)) {
    throw "frontend closeout edge artifact support module missing: $closeoutEdgeArtifactModulePath"
  }

  . $closeoutEdgeArtifactModulePath
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativeFrontendEdgeCompatibilityArtifact",
  "Write-Objc3cNativeFrontendEdgeRobustnessArtifact",
  "Write-Objc3cNativeFrontendDiagnosticsHardeningArtifact",
  "Write-Objc3cNativeFrontendRecoveryDeterminismHardeningArtifact"
)
