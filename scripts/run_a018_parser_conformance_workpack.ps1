param(
  [string]$FixtureRoot = "tests/tooling/fixtures/native",
  [string]$ReportRoot = "tmp/reports/parser_build/M226-A018"
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

function Read-JsonFile {
  param([Parameter(Mandatory = $true)][string]$Path)
  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
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
  "typed_i32_bool.objc3"
)

$requiredParserKeys = @(
  "diagnostic_code_count",
  "diagnostic_code_fingerprint",
  "diagnostic_code_surface_deterministic",
  "interface_categories",
  "implementation_categories",
  "function_prototypes",
  "function_pure"
)

New-Item -ItemType Directory -Force -Path $reportDir | Out-Null

& $buildScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$results = New-Object System.Collections.Generic.List[object]
foreach ($fixtureName in $fixtureNames) {
  $fixturePath = Join-Path $fixturesDir $fixtureName
  Assert-FileExists -Path $fixturePath
  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($fixtureName)
  $outDir = Join-Path $reportDir ("conformance/" + $fixtureStem)
  New-Item -ItemType Directory -Force -Path $outDir | Out-Null

  & $compileWrapper $fixturePath --out-dir $outDir --emit-prefix module
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

  $manifestPath = Join-Path $outDir "module.manifest.json"
  Assert-FileExists -Path $manifestPath
  $manifest = Read-JsonFile -Path $manifestPath

  $parserStage = $manifest.frontend.pipeline.stages.parser
  foreach ($key in $requiredParserKeys) {
    if ($null -eq $parserStage.$key) {
      throw "parser stage key '$key' missing for fixture '$fixtureName'"
    }
  }
  if (-not [bool]$parserStage.deterministic_handoff) {
    throw "parser deterministic_handoff is false for fixture '$fixtureName'"
  }
  if (-not [bool]$parserStage.recovery_replay_ready) {
    throw "parser recovery_replay_ready is false for fixture '$fixtureName'"
  }

  $results.Add([ordered]@{
      fixture = $fixtureName
      manifest = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $manifestPath
      parser_stage = [ordered]@{
        diagnostic_code_count = [int]$parserStage.diagnostic_code_count
        diagnostic_code_fingerprint = [uint64]$parserStage.diagnostic_code_fingerprint
        diagnostic_code_surface_deterministic = [bool]$parserStage.diagnostic_code_surface_deterministic
        interface_categories = [int]$parserStage.interface_categories
        implementation_categories = [int]$parserStage.implementation_categories
        function_prototypes = [int]$parserStage.function_prototypes
        function_pure = [int]$parserStage.function_pure
        deterministic_handoff = [bool]$parserStage.deterministic_handoff
        recovery_replay_ready = [bool]$parserStage.recovery_replay_ready
      }
    })
}

$summary = [ordered]@{
  contract_id = "objc3c-parser-advanced-conformance-workpack-contract/parser_build-a018-v1"
  fixture_root = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $fixturesDir
  fixtures = $results
}
$summaryPath = Join-Path $reportDir "parser_conformance_summary.json"
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("summary: " + (Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $summaryPath))
