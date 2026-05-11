$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileWrapperRoot = Join-Path $PSScriptRoot "objc3c_native_compile_wrapper"
$compileWrapperModules = @(
  "success_provenance.psm1",
  "cache_flow.psm1",
  "orchestration.psm1"
)

foreach ($compileWrapperModule in $compileWrapperModules) {
  $compileWrapperModulePath = Join-Path $compileWrapperRoot $compileWrapperModule
  if (!(Test-Path -LiteralPath $compileWrapperModulePath -PathType Leaf)) {
    Write-Error "native compile wrapper support module missing at $compileWrapperModulePath"
    exit 2
  }
  . $compileWrapperModulePath
}
