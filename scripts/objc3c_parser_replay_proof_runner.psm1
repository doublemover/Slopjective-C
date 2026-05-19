Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_invocation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_summary.psm1") -Force -DisableNameChecking

function Invoke-Objc3cParserReplayProof {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $ErrorActionPreference = "Stop"
  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $nativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
  $fixturePatterns = @(Get-Objc3cParserReplayFixturePatterns)
  $proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/parser-replay-proof"
  $proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
  $proofDir = Join-Path $proofRoot $proofRunId
  $summaryPath = Join-Path $proofDir "summary.json"
  $buildLogPath = Join-Path $proofDir "build.log"

  New-Item -ItemType Directory -Force -Path $proofDir | Out-Null

  Assert-Objc3cParserReplayPreconditions -BuildScript $buildScript -FixtureDir $fixtureDir
  Ensure-Objc3cParserReplayNativeCompiler -BuildScript $buildScript -NativeExe $nativeExe -BuildLogPath $buildLogPath -RepoRoot $repoRoot

  $fixtures = @(Get-Objc3cParserReplayFixtures -FixtureDir $fixtureDir -FixturePatterns $fixturePatterns)
  $results = New-Object System.Collections.Generic.List[object]
  $failedCount = 0

  foreach ($fixture in $fixtures) {
    $result = Invoke-Objc3cParserReplayFixtureCase -Fixture $fixture -ProofDir $proofDir -NativeExe $nativeExe -RepoRoot $repoRoot
    $fixtureName = [string]$result["fixture"]
    if ([bool]$result["passed"]) {
      Write-Output "PASS: $fixtureName"
    } else {
      $failedCount++
      Write-Output "FAIL: $fixtureName"
      foreach ($message in @($result["errors"])) {
        Write-Output ("  - {0}" -f $message)
      }
    }
    $results.Add($result)
  }

  $total = $fixtures.Count
  $passedCount = $total - $failedCount
  $status = if ($failedCount -eq 0) { "PASS" } else { "FAIL" }

  Write-Objc3cParserReplaySummary `
    -SummaryPath $summaryPath `
    -ProofRunId $proofRunId `
    -FixturePatterns $fixturePatterns `
    -Total $total `
    -PassedCount $passedCount `
    -FailedCount $failedCount `
    -Status $status `
    -Results ($results.ToArray())

  if ($failedCount -eq 0) {
    Write-Output ("PASS SUMMARY: {0}/{1} fixtures passed parser replay proof." -f $passedCount, $total)
  } else {
    Write-Output ("FAIL SUMMARY: {0}/{1} fixtures passed parser replay proof; {2} failed." -f $passedCount, $total, $failedCount)
  }
  Write-Output ("summary_path: {0}" -f (Get-Objc3cParserReplayRepoRelativePath -Path $summaryPath -RepoRoot $repoRoot))
  Write-Output ("status: {0}" -f $status)

  if ($failedCount -ne 0) {
    exit 1
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cParserReplayProof"
)
