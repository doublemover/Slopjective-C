param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$loweringScript = Join-Path $repoRoot "scripts/run_objc3c_lowering_regression_suite.ps1"
$loweringRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-regression"
$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-replay-proof"
$configuredProofRunId = $env:OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID
$defaultProofRunId = "m143-lane-c-lowering-replay-proof-default"

function Resolve-ValidatedRunId {
  param(
    [Parameter()][string]$ConfiguredRunId,
    [Parameter(Mandatory = $true)][string]$DefaultRunId,
    [Parameter(Mandatory = $true)][string]$FailurePrefix
  )

  if ([string]::IsNullOrWhiteSpace($ConfiguredRunId)) {
    return $DefaultRunId
  }

  $candidate = $ConfiguredRunId.Trim()
  if ($candidate.Length -gt 80) {
    throw "$FailurePrefix FAIL: configured run id exceeds 80 characters"
  }
  if ($candidate -notmatch '^[A-Za-z0-9_-]+$') {
    throw "$FailurePrefix FAIL: configured run id must match ^[A-Za-z0-9_-]+$"
  }
  return $candidate
}

$proofRunId = Resolve-ValidatedRunId `
  -ConfiguredRunId $configuredProofRunId `
  -DefaultRunId $defaultProofRunId `
  -FailurePrefix "lowering replay proof"
$proofDir = Join-Path $proofRoot $proofRunId
$proofSummaryPath = Join-Path $proofDir "summary.json"

New-Item -ItemType Directory -Force -Path $proofDir | Out-Null

function Get-Sha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "lowering replay proof FAIL: missing file for hash at $Path"
  }

  $bytes = [System.IO.File]::ReadAllBytes($Path)
  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Invoke-LoweringReplayRun {
  param(
    [Parameter(Mandatory = $true)][string]$RunId,
    [Parameter(Mandatory = $true)][string]$RunLabel,
    [Parameter(Mandatory = $true)][string]$ProofDirectory
  )

  $runLogPath = Join-Path $ProofDirectory ("$RunLabel.log")
  $deterministicCopyPath = Join-Path $ProofDirectory ("$RunLabel.deterministic-summary.json")
  $summaryCopyPath = Join-Path $ProofDirectory ("$RunLabel.summary.json")
  $runSummaryPath = Join-Path (Join-Path $loweringRoot $RunId) "summary.json"
  $deterministicSummaryPath = Join-Path $loweringRoot "latest-summary.json"

  $previousRunId = $env:OBJC3C_NATIVE_LOWERING_RUN_ID
  try {
    $env:OBJC3C_NATIVE_LOWERING_RUN_ID = $RunId
    & powershell -NoProfile -ExecutionPolicy Bypass -File $loweringScript *> $runLogPath
    $exitCode = [int]$LASTEXITCODE
  } finally {
    if ($null -eq $previousRunId) {
      Remove-Item Env:OBJC3C_NATIVE_LOWERING_RUN_ID -ErrorAction SilentlyContinue
    } else {
      $env:OBJC3C_NATIVE_LOWERING_RUN_ID = $previousRunId
    }
  }

  if ($exitCode -ne 0) {
    throw "lowering replay proof FAIL: lowering suite $RunLabel failed with exit code $exitCode"
  }
  if (!(Test-Path -LiteralPath $runSummaryPath -PathType Leaf)) {
    throw "lowering replay proof FAIL: missing run summary for $RunLabel at $runSummaryPath"
  }
  if (!(Test-Path -LiteralPath $deterministicSummaryPath -PathType Leaf)) {
    throw "lowering replay proof FAIL: missing deterministic summary after $RunLabel at $deterministicSummaryPath"
  }

  Copy-Item -LiteralPath $runSummaryPath -Destination $summaryCopyPath -Force
  Copy-Item -LiteralPath $deterministicSummaryPath -Destination $deterministicCopyPath -Force

  $deterministicHash = Get-Sha256Hex -Path $deterministicCopyPath
  return [pscustomobject]@{
    label = $RunLabel
    run_id = $RunId
    log = $runLogPath
    summary = $summaryCopyPath
    deterministic_summary = $deterministicCopyPath
    deterministic_sha256 = $deterministicHash
  }
}

try {
  $runIdA = "$proofRunId`_a"
  $runIdB = "$proofRunId`_b"

  $runA = Invoke-LoweringReplayRun -RunId $runIdA -RunLabel "run1" -ProofDirectory $proofDir
  $runB = Invoke-LoweringReplayRun -RunId $runIdB -RunLabel "run2" -ProofDirectory $proofDir

  $hashesMatch = $runA.deterministic_sha256 -eq $runB.deterministic_sha256
  $status = if ($hashesMatch) { "PASS" } else { "FAIL" }

  $summary = [ordered]@{
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    proof_run_id = $proofRunId
    status = $status
    deterministic_hashes_match = $hashesMatch
    run1 = $runA
    run2 = $runB
  }
  $summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $proofSummaryPath -Encoding utf8

  Write-Output ("run1_sha256: {0}" -f $runA.deterministic_sha256)
  Write-Output ("run2_sha256: {0}" -f $runB.deterministic_sha256)
  Write-Output ("summary_path: {0}" -f $proofSummaryPath.Replace($repoRoot, "").TrimStart('\', '/').Replace('\', '/'))
  Write-Output ("status: {0}" -f $status)

  if (-not $hashesMatch) {
    throw "lowering replay proof FAIL: deterministic summary hash mismatch"
  }
}
catch {
  Write-Output ("error: {0}" -f $_.Exception.Message)
  if (!(Test-Path -LiteralPath $proofSummaryPath -PathType Leaf)) {
    $fallback = [ordered]@{
      generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
      proof_run_id = $proofRunId
      status = "FAIL"
      fatal_error = $_.Exception.Message
    }
    $fallback | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $proofSummaryPath -Encoding utf8
  }
  Write-Output ("summary_path: {0}" -f $proofSummaryPath.Replace($repoRoot, "").TrimStart('\', '/').Replace('\', '/'))
  exit 1
}
