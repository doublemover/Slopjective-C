$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-replay-proof"
$proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$proofDir = Join-Path $proofRoot $proofRunId
$summaryPath = Join-Path $proofDir "summary.json"
$smokeScript = Join-Path $repoRoot "scripts/check_objc3c_native_execution_smoke.ps1"
$executionRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
$run1Id = "${proofRunId}_run1"
$run2Id = "${proofRunId}_run2"

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-Sha256HexFromText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Text)
    $hashBytes = $sha256.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $sha256.Dispose()
  }
}

function Invoke-SmokeRun {
  param(
    [Parameter(Mandatory = $true)][string]$RunId,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousRunId = $env:OBJC3C_NATIVE_EXECUTION_RUN_ID
  try {
    $env:OBJC3C_NATIVE_EXECUTION_RUN_ID = $RunId
    & powershell -NoProfile -ExecutionPolicy Bypass -File $smokeScript *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    if ($null -eq $previousRunId) {
      Remove-Item Env:OBJC3C_NATIVE_EXECUTION_RUN_ID -ErrorAction SilentlyContinue
    }
    else {
      $env:OBJC3C_NATIVE_EXECUTION_RUN_ID = $previousRunId
    }
  }
}

function Get-CanonicalSummaryJson {
  param([Parameter(Mandatory = $true)][string]$SummaryPath)

  if (!(Test-Path -LiteralPath $SummaryPath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing summary $SummaryPath"
  }

  $summary = Get-Content -LiteralPath $SummaryPath -Raw | ConvertFrom-Json
  $results = @(
    $summary.results |
      Sort-Object -Property kind, fixture |
      ForEach-Object {
        [ordered]@{
          kind = $_.kind
          fixture = $_.fixture
          expectation = $_.expectation
          stage = if ($_.PSObject.Properties.Name -contains "stage") { $_.stage } else { "" }
          requires_runtime_shim = if ($_.PSObject.Properties.Name -contains "requires_runtime_shim") { [bool]$_.requires_runtime_shim } else { $false }
          runtime_dispatch_symbol = if ($_.PSObject.Properties.Name -contains "runtime_dispatch_symbol") { "$($_.runtime_dispatch_symbol)" } else { "" }
          compile_exit = [int]$_.compile_exit
          link_exit = [int]$_.link_exit
          run_exit = if ($_.PSObject.Properties.Name -contains "run_exit") { [int]$_.run_exit } else { -1 }
          expected_exit = if ($_.PSObject.Properties.Name -contains "expected_exit") { [int]$_.expected_exit } else { -1 }
          required_link_tokens = if ($_.PSObject.Properties.Name -contains "required_link_tokens") { @($_.required_link_tokens) } else { @() }
          missing_link_tokens = if ($_.PSObject.Properties.Name -contains "missing_link_tokens") { @($_.missing_link_tokens) } else { @() }
          passed = [bool]$_.passed
        }
      }
  )

  $canonical = [ordered]@{
    runtime_shim = "$($summary.runtime_shim)"
    clang = "$($summary.clang)"
    total = [int]$summary.total
    passed = [int]$summary.passed
    failed = [int]$summary.failed
    status = "$($summary.status)"
    results = $results
  }
  return ($canonical | ConvertTo-Json -Depth 8 -Compress)
}

New-Item -ItemType Directory -Force -Path $proofDir | Out-Null
Push-Location $repoRoot
try {
  $run1Log = Join-Path $proofDir "run1.log"
  $run2Log = Join-Path $proofDir "run2.log"
  $run1Exit = Invoke-SmokeRun -RunId $run1Id -LogPath $run1Log
  if ($run1Exit -ne 0) {
    throw "execution replay proof FAIL: run1 failed with exit $run1Exit"
  }
  $run2Exit = Invoke-SmokeRun -RunId $run2Id -LogPath $run2Log
  if ($run2Exit -ne 0) {
    throw "execution replay proof FAIL: run2 failed with exit $run2Exit"
  }

  $run1Dir = Join-Path $executionRoot $run1Id
  $run2Dir = Join-Path $executionRoot $run2Id
  $run1SummaryPath = Join-Path $run1Dir "summary.json"
  $run2SummaryPath = Join-Path $run2Dir "summary.json"
  $run1Canonical = Get-CanonicalSummaryJson -SummaryPath $run1SummaryPath
  $run2Canonical = Get-CanonicalSummaryJson -SummaryPath $run2SummaryPath
  $run1Hash = Get-Sha256HexFromText -Text $run1Canonical
  $run2Hash = Get-Sha256HexFromText -Text $run2Canonical
  if ($run1Hash -ne $run2Hash) {
    throw "execution replay proof FAIL: canonical summary drift across replay (run1=$run1Hash run2=$run2Hash)"
  }

  $summary = [ordered]@{
    proof_run_id = $proofRunId
    run1_id = $run1Id
    run2_id = $run2Id
    run1_summary = Get-RepoRelativePath -Path $run1SummaryPath -Root $repoRoot
    run2_summary = Get-RepoRelativePath -Path $run2SummaryPath -Root $repoRoot
    run1_sha256 = $run1Hash
    run2_sha256 = $run2Hash
    status = "PASS"
  }
  $summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $summaryPath -Encoding utf8
  Write-Output "run1_sha256: $run1Hash"
  Write-Output "run2_sha256: $run2Hash"
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
  Write-Output "status: PASS"
}
finally {
  Pop-Location
}
