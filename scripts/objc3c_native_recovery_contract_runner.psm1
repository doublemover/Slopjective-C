$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_runner/context.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_runner/canonical_cases.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_runner/fixture_contracts.psm1") -Force -DisableNameChecking

function Invoke-Objc3cNativeRecoveryContract {
  param(
    [string]$RepoRoot,
    [string]$OutDir,
    [string]$CompilerPath,
    [string]$CompileWrapperScript,
    [string]$PowerShellExecutable,
    [string]$FixtureList = "",
    [string]$FixtureGlob = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  Set-RecoveryContractContext `
    -RepoRoot $RepoRoot `
    -OutDir $OutDir `
    -CompilerPath $CompilerPath `
    -CompileWrapperScript $CompileWrapperScript `
    -PowerShellExecutable $PowerShellExecutable

  $recoveryFixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery"
  $helloObjc3Source = Join-Path $RepoRoot "tests/tooling/fixtures/native/hello.objc3"
  $positiveFixtureDir = Join-Path $recoveryFixtureRoot "positive"
  $negativeFixtureDir = Join-Path $recoveryFixtureRoot "negative"

  Invoke-CoreRecoveryContractCases
  Invoke-InvalidDispatchSymbolContract `
    -OutDir $OutDir `
    -HelloObjc3Source $helloObjc3Source
  Invoke-SelectedRecoveryFixtureContracts `
    -RepoRoot $RepoRoot `
    -OutDir $OutDir `
    -PositiveFixtureDir $positiveFixtureDir `
    -NegativeFixtureDir $negativeFixtureDir `
    -FixtureList $FixtureList `
    -FixtureGlob $FixtureGlob `
    -ShardIndex $ShardIndex `
    -ShardCount $ShardCount `
    -Limit $Limit

  Write-Output "status: PASS"
  Write-Output ("out_dir: " + [System.IO.Path]::GetRelativePath($RepoRoot, $OutDir).Replace("\", "/"))
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativeRecoveryContract"
)
