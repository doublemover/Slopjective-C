Set-StrictMode -Version Latest

function New-ExecutionReplayProofSummary {
  param(
    [Parameter(Mandatory = $true)][object[]]$ProofCases,
    [Parameter(Mandatory = $true)][object[]]$SelectedProofCases,
    [Parameter(Mandatory = $true)][object[]]$CaseSummaries,
    [Parameter(Mandatory = $true)][object[]]$CaseTimings,
    [Parameter(Mandatory = $true)][AllowEmptyString()][string]$CaseId,
    [Parameter(Mandatory = $true)][int]$ShardIndex,
    [Parameter(Mandatory = $true)][int]$ShardCount,
    [Parameter(Mandatory = $true)][int]$Limit,
    [Parameter(Mandatory = $true)][double]$ElapsedSeconds
  )

  return [ordered]@{
    proof_run_id = $script:proofRunId
    native_exe = if (Test-Path -LiteralPath $script:nativeExe -PathType Leaf) { Get-RepoRelativePath -Path $script:nativeExe -Root $script:repoRoot } else { $script:nativeExe }
    claim_boundary = [ordered]@{
      contract_id = "objc3c.runtime.execution.claim.boundary.v1"
      authoritative_claim_class = "compile-coupled-replay-proof"
      proof_corpus_model = "canonical-native-truth-corpus"
      canonical_case_ids = @($ProofCases | ForEach-Object { [string]$_.case_id })
      execution_smoke_rerun_removed = $true
      compile_output_truthfulness_contract_id = "objc3c.native.compile.output.truthfulness.v1"
      registration_manifest_truth_required = $true
      authoritative_evidence = @(
        "emitted object coupled to compile provenance",
        "runtime registration manifest bound to the same artifact digest",
        "deterministic replay digest equality across two real compile runs",
        "runtime section inspection derived from the emitted object when required"
      )
      non_authoritative_inputs = @(
        "hand-authored llvm ir without the emitted object",
        "sidecar-only reports or manifests without coupled compile output",
        "non-authoritative test surfaces without the emitted object and runtime-backed probe path",
        "replay text alone without compile provenance and registration-manifest coupling"
      )
    }
    selection = [ordered]@{
      case_id = $CaseId
      shard_index = $ShardIndex
      shard_count = $ShardCount
      limit = $Limit
      selected_cases = $SelectedProofCases.Count
    }
    cases = $CaseSummaries
    timing = [ordered]@{
      elapsed_seconds = [math]::Round($ElapsedSeconds, 6)
      stage_totals = $script:stageTimings
      slowest_cases = @($CaseTimings | Sort-Object -Property duration_seconds -Descending | Select-Object -First 10)
      case_timings = @($CaseTimings)
    }
    status = "PASS"
  }
}

function Write-ExecutionReplayProofSummary {
  param([Parameter(Mandatory = $true)][object]$Summary)

  $Summary.timing.stage_totals = $script:stageTimings
  $Summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $script:summaryPath -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $script:summaryPath -Root $script:repoRoot)"
  Write-Output "status: PASS"
}
