$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendConformanceGuardDependencyModules {
  param(
    [Parameter(Mandatory = $true)]
    [string]$ConformanceGuardRoot
  )

  $scriptRoot = Split-Path $ConformanceGuardRoot -Parent
  return @(
    (Join-Path $scriptRoot "objc3c_native_compile_toolchain.psm1"),
    (Join-Path $ConformanceGuardRoot "config.psm1"),
    (Join-Path $ConformanceGuardRoot "evaluation.psm1"),
    (Join-Path $ConformanceGuardRoot "normalization.psm1")
  )
}
