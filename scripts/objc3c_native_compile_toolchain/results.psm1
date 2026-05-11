$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$resultsModuleRoot = Join-Path $PSScriptRoot "results"
$resultsModules = @(
  "artifact_prefixes.psm1",
  "conversion.psm1"
)

foreach ($resultsModule in $resultsModules) {
  $resultsModulePath = Join-Path $resultsModuleRoot $resultsModule
  if (!(Test-Path -LiteralPath $resultsModulePath -PathType Leaf)) {
    Write-Error "native compile toolchain result helper missing at $resultsModulePath"
    exit 2
  }
  . $resultsModulePath
}

Export-ModuleMember -Function "ConvertTo-NativeCompilerBuildResult"
