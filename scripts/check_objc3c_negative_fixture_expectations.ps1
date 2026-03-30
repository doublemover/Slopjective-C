param(
  [string]$FixtureList = "",
  [string]$FixtureGlob = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
$diagnosticHeaderPattern = '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$'
$diagnosticCodePattern = 'O3[A-Z]\d{3}'

# Suite ownership: this script owns static negative-fixture expectation
# metadata/header enforcement only. It intentionally does not recompile the
# negative recovery corpus because deterministic compile diagnostics belong to
# the recovery contract suite.

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

function Get-RequestedRelativePaths {
  param([string]$FixtureListPath)

  $resolvedFixtureList = if ([System.IO.Path]::IsPathRooted($FixtureListPath)) { $FixtureListPath } else { Join-Path $repoRoot $FixtureListPath }
  if (!(Test-Path -LiteralPath $resolvedFixtureList -PathType Leaf)) {
    Write-Output "FAIL: missing fixture list at $resolvedFixtureList"
    exit 1
  }

  $requested = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($rawLine in @(Get-Content -LiteralPath $resolvedFixtureList)) {
    $candidate = "$rawLine".Trim()
    if ([string]::IsNullOrWhiteSpace($candidate) -or $candidate.StartsWith("#")) {
      continue
    }
    $normalized = $candidate.Replace('\', '/')
    if ($normalized.StartsWith("./")) {
      $normalized = $normalized.Substring(2)
    }
    $null = $requested.Add($normalized)
  }
  return $requested
}

function Select-Fixtures {
  param([object[]]$Fixtures)

  if ($Limit -lt 0) {
    Write-Output "FAIL: limit must be non-negative"
    exit 1
  }
  if ($ShardCount -lt 0) {
    Write-Output "FAIL: shard-count must be non-negative"
    exit 1
  }
  if (($ShardIndex -ge 0) -and ($ShardCount -le 0)) {
    Write-Output "FAIL: shard-index requires shard-count > 0"
    exit 1
  }
  if (($ShardCount -gt 0) -and (($ShardIndex -lt 0) -or ($ShardIndex -ge $ShardCount))) {
    Write-Output "FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
    exit 1
  }

  $selected = @($Fixtures)

  if (-not [string]::IsNullOrWhiteSpace($FixtureList)) {
    $requested = Get-RequestedRelativePaths -FixtureListPath $FixtureList
    $selected = @($selected | Where-Object { $requested.Contains([System.IO.Path]::GetRelativePath($repoRoot, $_.FullName).Replace("\", "/")) })
    $matched = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
    foreach ($fixture in $selected) {
      $null = $matched.Add([System.IO.Path]::GetRelativePath($repoRoot, $fixture.FullName).Replace("\", "/"))
    }
    $missing = @()
    foreach ($requestedPath in $requested) {
      if (-not $matched.Contains($requestedPath)) {
        $missing += $requestedPath
      }
    }
    if ($missing.Count -gt 0) {
      Write-Output "FAIL: fixture-list entries did not match negative fixtures ($($missing -join ', '))"
      exit 1
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($FixtureGlob)) {
    $pattern = [System.Management.Automation.WildcardPattern]::new(
      $FixtureGlob.Replace('\', '/'),
      [System.Management.Automation.WildcardOptions]::IgnoreCase
    )
    $selected = @($selected | Where-Object { $pattern.IsMatch([System.IO.Path]::GetRelativePath($repoRoot, $_.FullName).Replace("\", "/")) })
  }

  if ($ShardCount -gt 0) {
    $sharded = New-Object System.Collections.Generic.List[object]
    for ($index = 0; $index -lt $selected.Count; $index++) {
      if (($index % $ShardCount) -eq $ShardIndex) {
        $sharded.Add($selected[$index]) | Out-Null
      }
    }
    $selected = @($sharded)
  }

  if (($Limit -gt 0) -and ($selected.Count -gt $Limit)) {
    $selected = @($selected | Select-Object -First $Limit)
  }

  if ($selected.Count -eq 0) {
    Write-Output "FAIL: no negative fixtures matched the requested selection"
    exit 1
  }

  return $selected
}

if (-not (Test-Path -LiteralPath $fixtureDir -PathType Container)) {
  Write-Output "FAIL: missing negative fixture directory at $fixtureDir"
  exit 1
}

$fixtures = @(Get-ChildItem -LiteralPath $fixtureDir -File | Where-Object {
    $_.Extension -in @(".objc3", ".m")
  } | Sort-Object FullName)
$fixtures = @(Select-Fixtures -Fixtures $fixtures)
Write-Output ("selection: negative={0}" -f $fixtures.Count)

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
