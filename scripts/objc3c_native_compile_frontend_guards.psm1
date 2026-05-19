$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Export-ModuleMember {
  param(
    [string[]]$Function,
    [string[]]$Cmdlet,
    [string[]]$Variable,
    [string[]]$Alias
  )
}

$frontendGuardRoot = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_guards"
$frontendGuardModules = @(
  "dependencies.psm1",
  "orchestration.psm1"
)

foreach ($frontendGuardModule in $frontendGuardModules) {
  $frontendGuardModulePath = Join-Path $frontendGuardRoot $frontendGuardModule
  if (!(Test-Path -LiteralPath $frontendGuardModulePath -PathType Leaf)) {
    Write-Error "native compile frontend guard helper missing at $frontendGuardModulePath"
    exit 2
  }
  $frontendGuardModuleRootLiteral = (Split-Path -Parent $frontendGuardModulePath).Replace("'", "''")
  $frontendGuardScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$frontendGuardModuleRootLiteral'`n" +
    (Get-Content -LiteralPath $frontendGuardModulePath -Raw)
  )
  . $frontendGuardScript
}

$guardModules = Get-Objc3cNativeCompileFrontendGuardDependencyModules

foreach ($guardModule in $guardModules) {
  $guardModulePath = Join-Path $PSScriptRoot $guardModule
  if (!(Test-Path -LiteralPath $guardModulePath -PathType Leaf)) {
    Write-Error "native compile frontend guard module missing at $guardModulePath"
    exit 2
  }
  $guardModuleRootLiteral = (Split-Path -Parent $guardModulePath).Replace("'", "''")
  $guardScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$guardModuleRootLiteral'`n" +
    (Get-Content -LiteralPath $guardModulePath -Raw)
  )
  . $guardScript
}

Microsoft.PowerShell.Core\Export-ModuleMember -Function "Invoke-Objc3cNativeCompileFrontendGuards"
