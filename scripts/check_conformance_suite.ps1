param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Add-Failure {
  param([string]$Message)
  $script:Failures.Add($Message) | Out-Null
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

Write-Output "Conformance bucket minima check:"
foreach ($bucket in $BucketMinima.Keys) {
  $bucketPath = Join-Path $RepoRoot "tests/conformance/$bucket"
  if (-not (Test-Path $bucketPath)) {
    Add-Failure "Missing required bucket directory: tests/conformance/$bucket"
    continue
  }
  $count = (Get-ChildItem $bucketPath -File -Filter "*.json" |
    Where-Object { $_.Name -ne "manifest.json" } |
    Measure-Object).Count
  Write-Output ("- {0}: {1} fixtures (minimum {2})" -f $bucket, $count, $BucketMinima[$bucket])
  if ($count -lt $BucketMinima[$bucket]) {
    Add-Failure ("Bucket '{0}' has {1} fixtures, below minimum {2}" -f $bucket, $count, $BucketMinima[$bucket])
  }
}

$allIds = Get-ChildItem (Join-Path $RepoRoot "tests/conformance") -Recurse -File -Filter "*.json" |
  Where-Object { $_.Name -ne "manifest.json" } |
  ForEach-Object { $_.BaseName }

$IdSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
foreach ($id in $allIds) {
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

Write-Output "Conformance suite check passed."
