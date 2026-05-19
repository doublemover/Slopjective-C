. (Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_positive_cases.psm1")
. (Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_negative_cases.psm1")

function Invoke-SemaPassManagerDiagnosticsBusRuntimeCases {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)][string[]]$NegativeFixturePaths,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  if (-not $nativeExeExplicit -and !(Test-Path -LiteralPath $nativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $repoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $repoRoot) }
  }

  Assert-FileExists -Path $nativeExePath -Id "runtime.native_executable.exists" -Description "native executable"

  Invoke-SemaPassManagerPositiveClangDefaultRuntimeCase `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -NativeExePath $nativeExePath `
    -PositiveFixturePath $positiveFixturePath `
    -CaseResults $caseResults

  Invoke-SemaPassManagerPositiveLlvmDirectDefaultRuntimeCase `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -NativeExePath $nativeExePath `
    -PositiveFixturePath $positiveFixturePath `
    -CaseResults $caseResults

  Invoke-SemaPassManagerPositiveLlvmDirectForcedMissingLlcRuntimeCase `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -NativeExePath $nativeExePath `
    -PositiveFixturePath $positiveFixturePath `
    -CaseResults $caseResults

  Invoke-SemaPassManagerNegativeBackendMatrixRuntimeCase `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -NativeExePath $nativeExePath `
    -NegativeFixturePath ($negativeFixturePaths[0]) `
    -CaseResults $caseResults

  Invoke-SemaPassManagerNegativeClangRuntimeCases `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -NativeExePath $nativeExePath `
    -NegativeFixturePaths $negativeFixturePaths `
    -CaseResults $caseResults
}

Export-ModuleMember -Function @(
  "Add-Check",
  "Assert-Contract",
  "Assert-FileExists",
  "Assert-TokensPresent",
  "Get-ExpectedSemaCodesFromFixture",
  "Get-FileSha256Hex",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Invoke-SemaPassManagerDiagnosticsBusRuntimeCases",
  "Read-NormalizedText",
  "Resolve-ValidatedRunId",
  "Set-SemaPassManagerDiagnosticsBusContractContext"
)
