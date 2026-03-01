param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Add-Failure {
  param([string]$Message)
  $script:Failures.Add($Message) | Out-Null
}

function Has-Property {
  param(
    [object]$Object,
    [string]$Name
  )
  if ($null -eq $Object) {
    return $false
  }
  return $Object.PSObject.Properties.Name -contains $Name
}

function Is-ExecutableFixture {
  param([object]$Payload)
  return (Has-Property -Object $Payload -Name "source") -and (Has-Property -Object $Payload -Name "expect")
}

function Is-MetadataFixtureRecord {
  param([object]$Payload)
  if ($null -eq $Payload) {
    return $false
  }
  if (-not (Has-Property -Object $Payload -Name "fixture_id")) {
    return $false
  }
  if ($Payload.fixture_id -isnot [string]) {
    return $false
  }
  return $Payload.fixture_id.Trim().Length -gt 0
}

function Resolve-FixtureId {
  param(
    [object]$Payload,
    [string]$FallbackId
  )
  if ((Has-Property -Object $Payload -Name "fixture_id") -and ($Payload.fixture_id -is [string]) -and ($Payload.fixture_id.Trim().Length -gt 0)) {
    return $Payload.fixture_id.Trim()
  }
  return $FallbackId
}

function Require-Id {
  param([string]$Id)
  if (-not $script:IdSet.Contains($Id)) {
    Add-Failure "Missing required conformance ID: $Id"
  }
}

function Require-Range {
  param(
    [string]$Prefix,
    [int]$Start,
    [int]$End,
    [int]$Width = 2
  )
  for ($n = $Start; $n -le $End; $n++) {
    $suffix = $n.ToString("D$Width")
    Require-Id "$Prefix$suffix"
  }
}

$Failures = [System.Collections.Generic.List[string]]::new()

$BucketMinima = [ordered]@{
  parser = 15
  semantic = 25
  lowering_abi = 10
  module_roundtrip = 12
  diagnostics = 20
}

$AllFixtureIds = [System.Collections.Generic.List[string]]::new()
$MetadataOnlyFixtureCount = 0

Write-Output "Conformance bucket minima check:"
foreach ($bucket in $BucketMinima.Keys) {
  $bucketPath = Join-Path $RepoRoot "tests/conformance/$bucket"
  if (-not (Test-Path -LiteralPath $bucketPath -PathType Container)) {
    Add-Failure "Missing required bucket directory: tests/conformance/$bucket"
    continue
  }
  $bucketFiles = Get-ChildItem -LiteralPath $bucketPath -File -Filter "*.json" |
    Where-Object { $_.Name -ne "manifest.json" }
  $executableCount = 0
  $metadataCount = 0
  foreach ($file in $bucketFiles) {
    $raw = $null
    try {
      $raw = Get-Content -LiteralPath $file.FullName -Raw -Encoding utf8
      $payload = $raw | ConvertFrom-Json
    } catch {
      Add-Failure ("Bucket '{0}' fixture parse failure: {1} ({2})" -f $bucket, $file.FullName, $_.Exception.Message)
      continue
    }

    if ($null -eq $payload -or ($payload -isnot [pscustomobject])) {
      Add-Failure ("Bucket '{0}' fixture shape failure: {1} must be a JSON object" -f $bucket, $file.FullName)
      continue
    }

    $fixtureId = Resolve-FixtureId -Payload $payload -FallbackId $file.BaseName
    $AllFixtureIds.Add($fixtureId) | Out-Null

    if (Is-ExecutableFixture -Payload $payload) {
      $executableCount += 1
      continue
    }

    if (Is-MetadataFixtureRecord -Payload $payload) {
      $metadataCount += 1
      $MetadataOnlyFixtureCount += 1
      continue
    }

    Add-Failure ("Bucket '{0}' fixture shape failure: {1} must include ('source' and 'expect') or include a non-empty string 'fixture_id'" -f $bucket, $file.FullName)
  }

  Write-Output ("- {0}: executable={1} metadata_only={2} (minimum executable {3})" -f $bucket, $executableCount, $metadataCount, $BucketMinima[$bucket])
  if ($executableCount -lt $BucketMinima[$bucket]) {
    Add-Failure ("Bucket '{0}' has {1} executable fixtures, below minimum {2}" -f $bucket, $executableCount, $BucketMinima[$bucket])
  }
}

$IdSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
foreach ($id in $AllFixtureIds) {
  $IdSet.Add($id) | Out-Null
}

Write-Output "Required family coverage check:"

# Core conformance families from Part 12 / workpack #106.
Require-Range "TUV-" 1 5
Require-Range "CRPT-" 1 6
Require-Range "SCM-" 1 6
Require-Range "EXE-" 1 5
Require-Range "CAN-" 1 7
Require-Range "ACT-" 1 9
Require-Range "SND-" 1 8
Require-Range "SND-XM-" 1 2
Require-Range "BRW-NEG-" 1 5
Require-Range "BRW-POS-" 1 4
Require-Range "PERF-DIRMEM-" 1 4
Require-Range "PERF-DYN-" 1 4

# Remaining E.3.11 / E.3.12 families.
Require-Range "INT-CXX-" 1 8
Require-Range "INT-SWIFT-" 1 8
Require-Range "DIAG-GRP-" 1 10
Require-Range "MIG-" 1 8

# Strict-system gate families for issue #107.
Require-Id "AGR-NEG-01"
Require-Range "AGR-RT-" 1 3
Require-Range "RES-" 1 6
Require-Range "SYS-DIAG-" 1 8

if ($Failures.Count -gt 0) {
  Write-Error ("Conformance suite check failed with {0} issue(s):" -f $Failures.Count)
  foreach ($failure in $Failures) {
    Write-Error ("- " + $failure)
  }
  exit 1
}

Write-Output ("metadata_only_fixtures_excluded_from_minima: {0}" -f $MetadataOnlyFixtureCount)
Write-Output "Conformance suite check passed."
