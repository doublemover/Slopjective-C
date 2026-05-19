$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileArgumentsRoot = Join-Path $PSScriptRoot "objc3c_native_compile_arguments"
$compileArgumentModules = @(
  "usage.psm1",
  "boolean_values.psm1",
  "wrapper_flags.psm1",
  "parsing.psm1",
  "out_dir_filter.psm1"
)

foreach ($compileArgumentModule in $compileArgumentModules) {
  $compileArgumentModulePath = Join-Path $compileArgumentsRoot $compileArgumentModule
  if (!(Test-Path -LiteralPath $compileArgumentModulePath -PathType Leaf)) {
    Write-Error "native compile argument helper missing at $compileArgumentModulePath"
    exit 2
  }
  $compileArgumentModuleRootLiteral = (Split-Path -Parent $compileArgumentModulePath).Replace("'", "''")
  $compileArgumentScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$compileArgumentModuleRootLiteral'`n" +
    (Get-Content -LiteralPath $compileArgumentModulePath -Raw)
  )
  . $compileArgumentScript
}
