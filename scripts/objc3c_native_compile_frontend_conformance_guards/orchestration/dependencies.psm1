$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Import-FrontendConformanceGuardDependencies {
  param(
    [Parameter(Mandatory = $true)]
    [string]$ConformanceGuardRoot
  )

  $scriptRoot = Split-Path $ConformanceGuardRoot -Parent
  $modules = @(
    (Join-Path $scriptRoot "objc3c_native_compile_toolchain.psm1"),
    (Join-Path $ConformanceGuardRoot "config.psm1"),
    (Join-Path $ConformanceGuardRoot "evaluation.psm1"),
    (Join-Path $ConformanceGuardRoot "normalization.psm1")
  )

  foreach ($modulePath in $modules) {
    if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
      Write-Error "native compile frontend conformance guard dependency module missing at $modulePath"
      exit 2
    }
    Import-Module $modulePath -Force -DisableNameChecking
  }
}
