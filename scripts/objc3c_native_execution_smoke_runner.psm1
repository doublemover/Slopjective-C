Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_execution_smoke_runner/orchestration.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativeExecutionSmoke {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  Invoke-Objc3cNativeExecutionSmokeCore `
    -ScriptRoot $ScriptRoot `
    -FixtureList $FixtureList `
    -FixtureGlob $FixtureGlob `
    -ShardIndex $ShardIndex `
    -ShardCount $ShardCount `
    -Limit $Limit
}

Export-ModuleMember -Function "Invoke-Objc3cNativeExecutionSmoke"
