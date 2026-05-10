$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_driver_shell_split_contract_helpers.psm1") -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "source_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "smoke_compile.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_renderer.psm1") -Force -DisableNameChecking

function Invoke-Objc3cDriverShellSplitContract {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $config = New-Objc3cDriverShellSplitContractConfig -ScriptRoot $ScriptRoot
  $checks = New-Object 'System.Collections.Generic.List[object]'
  $hadFatalError = $false
  $fatalErrorMessage = ""
  $smokeCompile = $null

  Set-Objc3cDriverShellSplitContractContext -Checks $checks -RepoRoot $config.RepoRoot

  New-Item -ItemType Directory -Force -Path $config.RunDir | Out-Null

  Push-Location $config.RepoRoot
  try {
    Invoke-Objc3cDriverShellSplitContractSourceAssertions -Config $config
    $smokeCompile = Invoke-Objc3cDriverShellSplitContractSmokeCompile -Config $config
  }
  catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  }
  finally {
    Pop-Location
  }

  Write-Objc3cDriverShellSplitContractSummary `
    -Config $config `
    -Checks $checks `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage `
    -SmokeCompile $smokeCompile
}

Export-ModuleMember -Function "Invoke-Objc3cDriverShellSplitContract"
