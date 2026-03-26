$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
$diagnosticHeaderPattern = '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$'
$diagnosticCodePattern = 'O3[A-Z]\d{3}'

function Get-ExpectedDiagnosticCodes {
  param(
    [string]$FixturePath
  )

  $content = Get-Content -LiteralPath $FixturePath -Raw
  $headerMatch = [regex]::Match($content, $diagnosticHeaderPattern)
  if (-not $headerMatch.Success) {
    return @{
      HeaderFound = $false
      Codes = @()
      DuplicateCodes = @()
    }
  }

  $rawCodes = [regex]::Matches($headerMatch.Groups[1].Value, $diagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  $codes = @($rawCodes | Select-Object -Unique)
  $duplicates = @($rawCodes | Group-Object | Where-Object { $_.Count -gt 1 } | ForEach-Object { $_.Name })

  return @{
    HeaderFound = $true
    Codes = $codes
    DuplicateCodes = $duplicates
  }
}

if (-not (Test-Path -LiteralPath $fixtureDir -PathType Container)) {
  Write-Output "FAIL: missing negative fixture directory at $fixtureDir"
  exit 1
}

$fixtures = @(Get-ChildItem -LiteralPath $fixtureDir -File | Where-Object {
    $_.Extension -in @(".objc3", ".m")
  } | Sort-Object FullName)

if ($fixtures.Count -eq 0) {
  Write-Output "FAIL: no negative fixtures found at $fixtureDir"
  exit 1
}

$failedFixtures = New-Object 'System.Collections.Generic.List[object]'

foreach ($fixture in $fixtures) {
  $fixtureName = $fixture.Name
  $fixtureErrors = New-Object 'System.Collections.Generic.List[string]'
  $headerResult = Get-ExpectedDiagnosticCodes -FixturePath $fixture.FullName

  if (-not $headerResult.HeaderFound) {
    $fixtureErrors.Add("missing expected diagnostic header")
  } elseif ($headerResult.Codes.Count -eq 0) {
    $fixtureErrors.Add("expected diagnostic header has no parseable diagnostic codes")
  }

  if ($headerResult.DuplicateCodes.Count -gt 0) {
    $fixtureErrors.Add("duplicate expected diagnostic code(s): $($headerResult.DuplicateCodes -join ', ')")
  }

  if ($fixtureErrors.Count -eq 0) {
    Write-Output "PASS: $fixtureName"
  } else {
    Write-Output "FAIL: $fixtureName"
    foreach ($errorMessage in $fixtureErrors) {
      Write-Output "  - $errorMessage"
    }
    $failedFixtures.Add([PSCustomObject]@{
        Fixture = $fixtureName
        Errors = @($fixtureErrors)
      })
  }
}

$total = $fixtures.Count
$failed = $failedFixtures.Count
$passed = $total - $failed

if ($failed -eq 0) {
  Write-Output "PASS SUMMARY: $passed/$total fixtures passed static expectation enforcement."
  exit 0
}

Write-Output "FAIL SUMMARY: $passed/$total fixtures passed; $failed failed static expectation enforcement."
exit 1
