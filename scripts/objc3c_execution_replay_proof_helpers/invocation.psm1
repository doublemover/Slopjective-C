Set-StrictMode -Version Latest

function Invoke-Objc3cExecutionReplayProof {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$CaseId = "",
    [int]$ShardIndex = -1,
    [int]$ShardCount = 0,
    [int]$Limit = 0
  )

  $ErrorActionPreference = "Stop"
  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  Initialize-ExecutionReplayProofContext -ScriptRoot $ScriptRoot
  $proofCases = @(Get-ExecutionReplayProofCases)

  New-Item -ItemType Directory -Force -Path $script:proofDir | Out-Null
  Push-Location $script:repoRoot
  try {
    $suiteStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    Initialize-ExecutionReplayProofTimings
    Ensure-NativeCompilerExecutable -NativeExePath $script:nativeExe -NativeExeExplicit $script:nativeExeExplicit -BuildScriptPath $script:buildScript

    $selectedProofCases = @(Select-ProofCases -Cases $proofCases -CaseId $CaseId -ShardIndex $ShardIndex -ShardCount $ShardCount -Limit $Limit)
    Write-Output ("selection: cases={0}" -f $selectedProofCases.Count)
    $caseSummaries = @()
    $caseTimings = @()
    $caseIndex = 0
    $lastCompletedCase = "none"
    foreach ($case in $selectedProofCases) {
      $caseIndex += 1
      $caseStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
      Write-Output ("execution-replay-progress: [{0}/{1}] START case={2} elapsed={3:n3}s last={4}" -f $caseIndex, $selectedProofCases.Count, $case.case_id, $suiteStopwatch.Elapsed.TotalSeconds, $lastCompletedCase)
      $run1 = Invoke-ReplayCompile -Case $case -RunLabel "run1" -NativeExePath $script:nativeExe
      $run2 = Invoke-ReplayCompile -Case $case -RunLabel "run2" -NativeExePath $script:nativeExe

      $comparisonStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
      Compare-ExecutionReplayProofRuns -Case $case -Run1 $run1 -Run2 $run2
      $comparisonStopwatch.Stop()
      Add-StageDuration -StageKey "comparison_seconds" -DurationSeconds ([math]::Round($comparisonStopwatch.Elapsed.TotalSeconds, 6))
      $caseStopwatch.Stop()

      $caseSummaries += [ordered]@{
        case_id = [string]$case.case_id
        claim_class = "compile-coupled-replay-proof"
        run1 = $run1
        run2 = $run2
        timing = [ordered]@{
          duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
          run1_compile_seconds = [double]$run1.timing.compile_seconds
          run2_compile_seconds = [double]$run2.timing.compile_seconds
          run1_readobj_seconds = [double]$run1.timing.readobj_seconds
          run2_readobj_seconds = [double]$run2.timing.readobj_seconds
          comparison_seconds = [math]::Round($comparisonStopwatch.Elapsed.TotalSeconds, 6)
        }
        status = "PASS"
      }
      $caseTimings += [ordered]@{
        case_id = [string]$case.case_id
        duration_seconds = [math]::Round($caseStopwatch.Elapsed.TotalSeconds, 6)
        compile_seconds = [math]::Round(([double]$run1.timing.compile_seconds + [double]$run2.timing.compile_seconds), 6)
        readobj_seconds = [math]::Round(([double]$run1.timing.readobj_seconds + [double]$run2.timing.readobj_seconds), 6)
        comparison_seconds = [math]::Round($comparisonStopwatch.Elapsed.TotalSeconds, 6)
      }
      $lastCompletedCase = [string]$case.case_id
      Write-Output ("execution-replay-progress: [{0}/{1}] DONE case={2} duration={3:n3}s elapsed={4:n3}s" -f $caseIndex, $selectedProofCases.Count, $case.case_id, $caseStopwatch.Elapsed.TotalSeconds, $suiteStopwatch.Elapsed.TotalSeconds)
    }

    $reportStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    $summary = New-ExecutionReplayProofSummary `
      -ProofCases $proofCases `
      -SelectedProofCases $selectedProofCases `
      -CaseSummaries $caseSummaries `
      -CaseTimings $caseTimings `
      -CaseId $CaseId `
      -ShardIndex $ShardIndex `
      -ShardCount $ShardCount `
      -Limit $Limit `
      -ElapsedSeconds $suiteStopwatch.Elapsed.TotalSeconds
    $reportStopwatch.Stop()
    Add-StageDuration -StageKey "output_report_seconds" -DurationSeconds ([math]::Round($reportStopwatch.Elapsed.TotalSeconds, 6))
    $summary.timing.elapsed_seconds = [math]::Round($suiteStopwatch.Elapsed.TotalSeconds, 6)
    Write-ExecutionReplayProofSummary -Summary $summary
  }
  finally {
    Pop-Location
  }
}
