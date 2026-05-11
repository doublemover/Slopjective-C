$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$configRoot = Join-Path $PSScriptRoot "config"
$configModules = @(
  "diagnostics.psm1",
  "invocation.psm1",
  "contracts.psm1"
)

foreach ($configModule in $configModules) {
  $configModulePath = Join-Path $configRoot $configModule
  if (!(Test-Path -LiteralPath $configModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance config helper missing at $configModulePath"
    exit 2
  }
  . $configModulePath
}

Export-ModuleMember -Function @(
  "Get-FrontendConformanceCorpusGuardConfig",
  "Get-FrontendConformanceInvocationConfig",
  "Get-FrontendConformanceMatrixGuardConfig",
  "Get-FrontendConformanceRejectDiagnostics",
  "Get-FrontendIntegrationCloseoutGuardConfig"
)
