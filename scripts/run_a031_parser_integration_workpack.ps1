param(
  [string]$ReportRoot = "tmp/reports/parser_build/M226-A031"
)

$ErrorActionPreference = "Stop"

function Get-RepoRelativePathCompat {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  if ($resolvedRoot.EndsWith('\') -or $resolvedRoot.EndsWith('/')) {
    $rootWithSeparator = $resolvedRoot
  } else {
    $rootWithSeparator = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  }

  $relativePath = $null
  $getRelativeMethod = [System.IO.Path].GetMethod("GetRelativePath", [Type[]]@([string], [string]))
  if ($null -ne $getRelativeMethod) {
    $relativePath = [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)
  } else {
    $rootUri = New-Object System.Uri($rootWithSeparator)
    $targetUri = New-Object System.Uri($resolvedTarget)
    $relativeUri = $rootUri.MakeRelativeUri($targetUri)
    $relativePath = [System.Uri]::UnescapeDataString($relativeUri.ToString())
  }
  return $relativePath.Replace('\', '/')
}

function Assert-FileExists {
  param([Parameter(Mandatory = $true)][string]$Path)
  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "missing required summary artifact: $Path"
  }
}

function Read-JsonFile {
  param([Parameter(Mandatory = $true)][string]$Path)
  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$reportDir = Join-Path $repoRoot $ReportRoot
New-Item -ItemType Directory -Force -Path $reportDir | Out-Null

$upstream = @(
  @{ packet = "A027"; path = "tmp/reports/parser_build/M226-A027/parser_advanced_core_workpack_summary.json" },
  @{ packet = "A028"; path = "tmp/reports/parser_build/M226-A028/parser_advanced_edge_compat_workpack_summary.json" },
  @{ packet = "A029"; path = "tmp/reports/parser_build/M226-A029/parser_advanced_diagnostics_workpack_summary.json" },
  @{ packet = "A030"; path = "tmp/reports/parser_build/M226-A030/parser_conformance_shard3_summary.json" }
)

$upstreamResults = New-Object System.Collections.Generic.List[object]
foreach ($entry in $upstream) {
  $absolutePath = Join-Path $repoRoot $entry.path
  Assert-FileExists -Path $absolutePath
  $payload = Read-JsonFile -Path $absolutePath
  if ($entry.packet -ne "A030") {
    if ($null -eq $payload.ok -or -not [bool]$payload.ok) {
      throw "upstream parser packet '$($entry.packet)' summary is not ok: $($entry.path)"
    }
  } else {
    if ($null -eq $payload.fixtures -or @($payload.fixtures).Count -eq 0) {
      throw "upstream parser packet 'A030' summary has no fixtures: $($entry.path)"
    }
    foreach ($fixture in @($payload.fixtures)) {
      if ($null -eq $fixture.deterministic_match -or -not [bool]$fixture.deterministic_match) {
        throw "upstream parser packet 'A030' fixture is not deterministic: $($entry.path)"
      }
    }
  }

  $entryOk = $true
  if ($entry.packet -ne "A030") {
    $entryOk = [bool]$payload.ok
  }

  $upstreamResults.Add([ordered]@{
      packet = $entry.packet
      path = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $absolutePath
      ok = $entryOk
    })
}

$summary = [ordered]@{
  contract_id = "objc3c-parser-advanced-integration-workpack-contract/parser_build-a031-v1"
  upstream = $upstreamResults
  integrated_ok = $true
}
$summaryPath = Join-Path $reportDir "parser_integration_shard3_summary.json"
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("summary: " + (Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $summaryPath))


