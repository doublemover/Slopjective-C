param(
  [string]$FixtureRoot = "tests/tooling/fixtures/native",
  [string]$ReportRoot = "tmp/reports/parser_build/M226-A026"
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
    throw "missing required file: $Path"
  }
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$fixturesDir = Join-Path $repoRoot $FixtureRoot
$reportDir = Join-Path $repoRoot $ReportRoot
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$compileWrapper = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"

Assert-FileExists -Path $buildScript
Assert-FileExists -Path $compileWrapper
if (!(Test-Path -LiteralPath $fixturesDir -PathType Container)) {
  throw "fixture root missing: $fixturesDir"
}

$fixtureNames = @(
  "hello.objc3",
  "return_paths_ok.objc3",
  "typed_i32_bool.objc3",
  "comparison_logic.objc3"
)

New-Item -ItemType Directory -Force -Path $reportDir | Out-Null

& $buildScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$results = New-Object System.Collections.Generic.List[object]
foreach ($fixtureName in $fixtureNames) {
  $fixturePath = Join-Path $fixturesDir $fixtureName
  Assert-FileExists -Path $fixturePath
  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($fixtureName)
  $fixtureDir = Join-Path $reportDir ("perf/" + $fixtureStem)
  New-Item -ItemType Directory -Force -Path $fixtureDir | Out-Null

  $passes = New-Object System.Collections.Generic.List[object]
  for ($pass = 1; $pass -le 2; $pass++) {
    $outDir = Join-Path $fixtureDir ("pass" + $pass)
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null

    $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    & $compileWrapper $fixturePath --out-dir $outDir --emit-prefix module
    $stopwatch.Stop()
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    $manifestPath = Join-Path $outDir "module.manifest.json"
    Assert-FileExists -Path $manifestPath

    $passes.Add([ordered]@{
        pass = $pass
        elapsed_ms = [math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
        manifest = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $manifestPath
      })
  }

  $pass1 = [double]$passes[0].elapsed_ms
  $pass2 = [double]$passes[1].elapsed_ms
  $avg = ($pass1 + $pass2) / 2.0
  $drift = [math]::Abs($pass1 - $pass2)

  $results.Add([ordered]@{
      fixture = $fixtureName
      passes = $passes
      average_elapsed_ms = [math]::Round($avg, 3)
      pass_drift_ms = [math]::Round($drift, 3)
    })
}

$totalMs = 0.0
$maxDrift = 0.0
foreach ($entry in $results) {
  $totalMs += [double]$entry.average_elapsed_ms
  if ([double]$entry.pass_drift_ms -gt $maxDrift) {
    $maxDrift = [double]$entry.pass_drift_ms
  }
}
$averageMs = 0.0
if ($results.Count -gt 0) {
  $averageMs = $totalMs / [double]$results.Count
}

$summary = [ordered]@{
  contract_id = "objc3c-parser-advanced-performance-workpack-contract/parser_build-performance-workpack-iteration26-v1"
  non_gating = $true
  fixtures = $results
  aggregate = [ordered]@{
    total_elapsed_ms = [math]::Round($totalMs, 3)
    average_elapsed_ms = [math]::Round($averageMs, 3)
    max_pass_drift_ms = [math]::Round($maxDrift, 3)
  }
}
$summaryPath = Join-Path $reportDir "parser_performance_shard2_summary.json"
$summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("summary: " + (Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $summaryPath))
