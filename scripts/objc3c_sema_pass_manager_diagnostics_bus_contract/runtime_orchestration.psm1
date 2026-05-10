$script:SemaDiagnosticsBusScriptsRoot = Split-Path -Parent $PSScriptRoot
. (Join-Path $script:SemaDiagnosticsBusScriptsRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_positive_cases.psm1")
. (Join-Path $script:SemaDiagnosticsBusScriptsRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_negative_cases.psm1")

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

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

  if (-not $NativeExeExplicit -and !(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot) }
  }

  Assert-FileExists -Path $NativeExePath -Id "runtime.native_executable.exists" -Description "native executable"

  Invoke-SemaPassManagerPositiveClangDefaultRuntimeCase `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -PositiveFixturePath $PositiveFixturePath `
    -CaseResults $CaseResults

  Invoke-SemaPassManagerPositiveLlvmDirectDefaultRuntimeCase `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -PositiveFixturePath $PositiveFixturePath `
    -CaseResults $CaseResults

  Invoke-SemaPassManagerPositiveLlvmDirectForcedMissingLlcRuntimeCase `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -PositiveFixturePath $PositiveFixturePath `
    -CaseResults $CaseResults

  Invoke-SemaPassManagerNegativeBackendMatrixRuntimeCase `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -NegativeFixturePath ($NegativeFixturePaths[0]) `
    -CaseResults $CaseResults

  Invoke-SemaPassManagerNegativeClangRuntimeCases `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -NegativeFixturePaths $NegativeFixturePaths `
    -CaseResults $CaseResults
}
