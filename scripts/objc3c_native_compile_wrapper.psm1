$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileWrapperIoModule = Join-Path $PSScriptRoot "objc3c_native_compile_io.psm1"
if (!(Test-Path -LiteralPath $compileWrapperIoModule -PathType Leaf)) {
  Write-Error "native compile wrapper IO dependency missing at $compileWrapperIoModule"
  exit 2
}
Import-Module $compileWrapperIoModule -Force -DisableNameChecking

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
  $compileWrapperModuleRootLiteral = (Split-Path -Parent $compileWrapperModulePath).Replace("'", "''")
  $compileWrapperScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$compileWrapperModuleRootLiteral'`n" +
    (Get-Content -LiteralPath $compileWrapperModulePath -Raw)
  )
  . $compileWrapperScript
}

Export-ModuleMember -Function "Invoke-Objc3cNativeCompileWrapper"
