param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$diagnosticsScript = Join-Path $repoRoot "scripts/run_objc3c_diagnostics_regression_suite.ps1"
$diagnosticsRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/diagnostics-regression"
$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/diagnostics-replay-proof"
$proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$proofDir = Join-Path $proofRoot $proofRunId
$proofSummaryPath = Join-Path $proofDir "summary.json"

New-Item -ItemType Directory -Force -Path $proofDir | Out-Null

function Get-CanonicalDiagnosticsSummary {
  param([Parameter(Mandatory = $true)][string]$SummaryPath)

  if (!(Test-Path -LiteralPath $SummaryPath -PathType Leaf)) {
    throw "diagnostics replay proof FAIL: missing summary at $SummaryPath"
  }

  $summary = Get-Content -LiteralPath $SummaryPath -Raw | ConvertFrom-Json
  $results = @(
    $summary.results |
      Sort-Object -Property fixture |
      ForEach-Object {
        [ordered]@{
          fixture = $_.fixture
          expected_codes = @($_.expected_codes)
          passed = [bool]$_.passed
          run1_exit_code = [int]$_.run1_exit_code
          run2_exit_code = [int]$_.run2_exit_code
          run1_diagnostic_codes = @($_.run1_diagnostic_codes)
          run2_diagnostic_codes = @($_.run2_diagnostic_codes)
          run1_diagnostics_sha256 = $_.run1_diagnostics_sha256
          run2_diagnostics_sha256 = $_.run2_diagnostics_sha256
          run1_diagnostics_json_codes = @($_.run1_diagnostics_json_codes)
          run2_diagnostics_json_codes = @($_.run2_diagnostics_json_codes)
          run1_diagnostics_json_sha256 = $_.run1_diagnostics_json_sha256
          run2_diagnostics_json_sha256 = $_.run2_diagnostics_json_sha256
          checks = [ordered]@{
            exit_nonzero = [bool]$_.checks.exit_nonzero
            exit_deterministic = [bool]$_.checks.exit_deterministic
            diagnostics_present = [bool]$_.checks.diagnostics_present
            diagnostics_nonempty = [bool]$_.checks.diagnostics_nonempty
            diagnostics_deterministic = [bool]$_.checks.diagnostics_deterministic
            diagnostics_json_present = [bool]$_.checks.diagnostics_json_present
            diagnostics_json_nonempty = [bool]$_.checks.diagnostics_json_nonempty
            diagnostics_json_valid = [bool]$_.checks.diagnostics_json_valid
            diagnostics_json_deterministic = [bool]$_.checks.diagnostics_json_deterministic
            diagnostics_json_codes_match_text = [bool]$_.checks.diagnostics_json_codes_match_text
            codes_match_expected = [bool]$_.checks.codes_match_expected
            fail_closed_artifacts = [bool]$_.checks.fail_closed_artifacts
          }
          errors = @($_.errors)
        }
      }
  )

  return [ordered]@{
    total = [int]$summary.total
    passed = [int]$summary.passed
    failed = [int]$summary.failed
    status = $summary.status
    fatal_error = $summary.fatal_error
    results = $results
  }
}

function Get-Sha256ForText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $bytes = [System.Text.Encoding]::UTF8.GetBytes($Text)
  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Invoke-DiagnosticsReplayRun {
  param(
    [Parameter(Mandatory = $true)][string]$RunLabel,
    [Parameter(Mandatory = $true)][string]$ProofDirectory
  )

  $runLogPath = Join-Path $ProofDirectory ("$RunLabel.log")
  $summaryCopyPath = Join-Path $ProofDirectory ("$RunLabel.summary.json")
  $canonicalCopyPath = Join-Path $ProofDirectory ("$RunLabel.canonical-summary.json")
  $beforeDirs = @()
  if (Test-Path -LiteralPath $diagnosticsRoot -PathType Container) {
    $beforeDirs = @(Get-ChildItem -LiteralPath $diagnosticsRoot -Directory | ForEach-Object { $_.FullName })
  }
  $beforeSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($dir in $beforeDirs) {
    $null = $beforeSet.Add($dir)
  }

  & powershell -NoProfile -ExecutionPolicy Bypass -File $diagnosticsScript *> $runLogPath
  $exitCode = [int]$LASTEXITCODE

  if ($exitCode -ne 0) {
    throw "diagnostics replay proof FAIL: diagnostics suite $RunLabel failed with exit code $exitCode"
  }
  if (!(Test-Path -LiteralPath $diagnosticsRoot -PathType Container)) {
    throw "diagnostics replay proof FAIL: diagnostics root missing at $diagnosticsRoot"
  }

  $newRunDirs = @(
    Get-ChildItem -LiteralPath $diagnosticsRoot -Directory |
      Where-Object { -not $beforeSet.Contains($_.FullName) } |
      Sort-Object -Property LastWriteTimeUtc -Descending
  )
  if ($newRunDirs.Count -eq 0) {
    throw "diagnostics replay proof FAIL: could not detect new diagnostics run directory for $RunLabel"
  }
  $runDir = $newRunDirs[0].FullName
  $runId = Split-Path -Path $runDir -Leaf
  $runSummaryPath = Join-Path $runDir "summary.json"

  if (!(Test-Path -LiteralPath $runSummaryPath -PathType Leaf)) {
    throw "diagnostics replay proof FAIL: missing run summary for $RunLabel at $runSummaryPath"
  }

  Copy-Item -LiteralPath $runSummaryPath -Destination $summaryCopyPath -Force
  $canonical = Get-CanonicalDiagnosticsSummary -SummaryPath $runSummaryPath
  $canonicalJson = $canonical | ConvertTo-Json -Depth 8
  Set-Content -LiteralPath $canonicalCopyPath -Encoding utf8 -Value $canonicalJson
  $canonicalHash = Get-Sha256ForText -Text $canonicalJson

  return [pscustomobject]@{
    label = $RunLabel
    run_id = $RunId
    log = $runLogPath
    summary = $summaryCopyPath
    canonical_summary = $canonicalCopyPath
    canonical_sha256 = $canonicalHash
  }
}

try {
  $runA = Invoke-DiagnosticsReplayRun -RunLabel "run1" -ProofDirectory $proofDir
  $runB = Invoke-DiagnosticsReplayRun -RunLabel "run2" -ProofDirectory $proofDir

  $hashesMatch = $runA.canonical_sha256 -eq $runB.canonical_sha256
  $status = if ($hashesMatch) { "PASS" } else { "FAIL" }

  $summary = [ordered]@{
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    proof_run_id = $proofRunId
    status = $status
    canonical_hashes_match = $hashesMatch
    run1 = $runA
    run2 = $runB
  }
  $summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $proofSummaryPath -Encoding utf8

  Write-Output ("run1_sha256: {0}" -f $runA.canonical_sha256)
  Write-Output ("run2_sha256: {0}" -f $runB.canonical_sha256)
  Write-Output ("summary_path: {0}" -f $proofSummaryPath.Replace($repoRoot, "").TrimStart('\', '/').Replace('\', '/'))
  Write-Output ("status: {0}" -f $status)

  if (-not $hashesMatch) {
    throw "diagnostics replay proof FAIL: canonical summary hash mismatch"
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
