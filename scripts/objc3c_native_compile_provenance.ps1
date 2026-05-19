$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileProvenanceModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_provenance"
$compileProvenanceModules = @(
  "hash_io.psm1",
  "path_config.psm1",
  "text_analysis.psm1",
  "truthfulness.psm1",
  "provenance_capture.psm1"
)

foreach ($compileProvenanceModuleName in $compileProvenanceModules) {
  $compileProvenanceModulePath = Join-Path $compileProvenanceModuleRoot $compileProvenanceModuleName
  if (!(Test-Path -LiteralPath $compileProvenanceModulePath -PathType Leaf)) {
    Write-Error "native compile provenance helper missing at $compileProvenanceModulePath"
    exit 2
  }
  Import-Module $compileProvenanceModulePath -Force -DisableNameChecking
}
