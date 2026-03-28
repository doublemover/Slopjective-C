param(
  [string]$ReportRoot = "tmp/reports/parser_build/M226-A032"
)

$ErrorActionPreference = "Stop"

function Get-RepoRelativePathCompat {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  if ($resolvedRoot.EndsWith('\\') -or $resolvedRoot.EndsWith('/')) {
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
  return $relativePath.Replace('\\', '/')
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
  @{ packet = "A025"; path = "tmp/reports/parser_build/M226-A025/parser_integration_shard2_summary.json" },
  @{ packet = "A031"; path = "tmp/reports/parser_build/M226-A031/parser_integration_shard3_summary.json" }
)

$upstreamResults = New-Object System.Collections.Generic.List[object]
foreach ($entry in $upstream) {
  $absolutePath = Join-Path $repoRoot $entry.path
  Assert-FileExists -Path $absolutePath
  $payload = Read-JsonFile -Path $absolutePath
  if ($null -eq $payload.integrated_ok -or -not [bool]$payload.integrated_ok) {
    throw "upstream parser packet '$($entry.packet)' integrated_ok is false: $($entry.path)"
  }

  $upstreamResults.Add([ordered]@{
      packet = $entry.packet
      path = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $absolutePath
      integrated_ok = [bool]$payload.integrated_ok
    })
}

$summary = [ordered]@{
  contract_id = "objc3c-parser-integration-closeout-signoff-contract/parser_build-integration-closeout-signoff-v1"
  upstream = $upstreamResults
  gate_signoff_ready = $true
  closeout_ready = $true
}
$summaryPath = Join-Path $reportDir "parser_integration_closeout_signoff_summary.json"
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("summary: " + (Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $summaryPath))
