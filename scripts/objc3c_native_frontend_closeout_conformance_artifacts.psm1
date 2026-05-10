$ErrorActionPreference = "Stop"

$closeoutConformanceArtifactRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_closeout_conformance_artifacts"
$closeoutConformanceArtifactModules = @(
  "constants.psm1",
  "loading.psm1",
  "assertions.psm1",
  "payloads.psm1",
  "orchestration.psm1"
)

foreach ($closeoutConformanceArtifactModule in $closeoutConformanceArtifactModules) {
  $closeoutConformanceArtifactModulePath = Join-Path $closeoutConformanceArtifactRoot $closeoutConformanceArtifactModule
  if (!(Test-Path -LiteralPath $closeoutConformanceArtifactModulePath -PathType Leaf)) {
    throw "frontend closeout conformance artifact support module missing: $closeoutConformanceArtifactModulePath"
  }

  . $closeoutConformanceArtifactModulePath
}

Export-ModuleMember -Function @(
  "Write-Objc3cNativeFrontendConformanceMatrixArtifact",
  "Write-Objc3cNativeFrontendConformanceCorpusArtifact",
  "Write-Objc3cNativeFrontendIntegrationCloseoutArtifact"
)
