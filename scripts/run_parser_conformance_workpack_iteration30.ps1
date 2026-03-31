param(
  [string]$FixtureRoot = "tests/tooling/fixtures/native",
  [string]$ReportRoot = "tmp/reports/parser_build/parser_conformance_shard3"
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

function Convert-ToCanonicalJson {
  param([Parameter(Mandatory = $true)][object]$Value)
  return ($Value | ConvertTo-Json -Depth 12 -Compress)
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

$requiredParserKeys = @(
  "diagnostic_code_count",
  "diagnostic_code_fingerprint",
  "diagnostic_code_surface_deterministic",
  "interface_categories",
  "implementation_categories",
  "function_prototypes",
  "function_pure",
  "protocol_properties",
  "protocol_methods",
  "interface_properties",
  "interface_methods",
  "implementation_properties",
  "implementation_methods"
)

$requiredReadinessKeys = @(
  "parse_artifact_replay_key_deterministic",
  "parse_artifact_diagnostics_hardening_consistent",
  "parse_artifact_edge_case_robustness_consistent",
  "parse_recovery_determinism_hardening_consistent",
  "parse_recovery_determinism_hardening_key",
  "ready_for_lowering"
)

New-Item -ItemType Directory -Force -Path $reportDir | Out-Null

& $buildScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$results = New-Object System.Collections.Generic.List[object]
foreach ($fixtureName in $fixtureNames) {
  $fixturePath = Join-Path $fixturesDir $fixtureName
  Assert-FileExists -Path $fixturePath
  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($fixtureName)
  $fixtureDir = Join-Path $reportDir ("conformance/" + $fixtureStem)
  New-Item -ItemType Directory -Force -Path $fixtureDir | Out-Null

  $passRecords = New-Object System.Collections.Generic.List[object]
  $baselineParserCanonical = $null
  $baselineReadinessCanonical = $null
  for ($pass = 1; $pass -le 2; $pass++) {
    $passDir = Join-Path $fixtureDir ("pass" + $pass)
    New-Item -ItemType Directory -Force -Path $passDir | Out-Null

    & $compileWrapper $fixturePath --out-dir $passDir --emit-prefix module
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    $manifestPath = Join-Path $passDir "module.manifest.json"
    Assert-FileExists -Path $manifestPath
    $manifest = Read-JsonFile -Path $manifestPath

    $parserStage = $manifest.frontend.pipeline.stages.parser
    foreach ($key in $requiredParserKeys) {
      if ($null -eq $parserStage.$key) {
        throw "parser stage key '$key' missing for fixture '$fixtureName' pass '$pass'"
      }
    }
    if (-not [bool]$parserStage.deterministic_handoff) {
      throw "parser deterministic_handoff is false for fixture '$fixtureName' pass '$pass'"
    }
    if (-not [bool]$parserStage.recovery_replay_ready) {
      throw "parser recovery_replay_ready is false for fixture '$fixtureName' pass '$pass'"
    }

    $readiness = $manifest.frontend.pipeline.parse_lowering_readiness
    foreach ($key in $requiredReadinessKeys) {
      if ($null -eq $readiness.$key) {
        throw "parse_lowering_readiness key '$key' missing for fixture '$fixtureName' pass '$pass'"
      }
    }

    $parserProjection = [ordered]@{
      diagnostic_code_count = [int]$parserStage.diagnostic_code_count
      diagnostic_code_fingerprint = [uint64]$parserStage.diagnostic_code_fingerprint
      diagnostic_code_surface_deterministic = [bool]$parserStage.diagnostic_code_surface_deterministic
      interface_categories = [int]$parserStage.interface_categories
      implementation_categories = [int]$parserStage.implementation_categories
      function_prototypes = [int]$parserStage.function_prototypes
      function_pure = [int]$parserStage.function_pure
      protocol_properties = [int]$parserStage.protocol_properties
      protocol_methods = [int]$parserStage.protocol_methods
      interface_properties = [int]$parserStage.interface_properties
      interface_methods = [int]$parserStage.interface_methods
      implementation_properties = [int]$parserStage.implementation_properties
      implementation_methods = [int]$parserStage.implementation_methods
      deterministic_handoff = [bool]$parserStage.deterministic_handoff
      recovery_replay_ready = [bool]$parserStage.recovery_replay_ready
    }
    $readinessProjection = [ordered]@{
      parse_artifact_replay_key_deterministic = [bool]$readiness.parse_artifact_replay_key_deterministic
      parse_artifact_diagnostics_hardening_consistent = [bool]$readiness.parse_artifact_diagnostics_hardening_consistent
      parse_artifact_edge_case_robustness_consistent = [bool]$readiness.parse_artifact_edge_case_robustness_consistent
      parse_recovery_determinism_hardening_consistent = [bool]$readiness.parse_recovery_determinism_hardening_consistent
      parse_recovery_determinism_hardening_key = [string]$readiness.parse_recovery_determinism_hardening_key
      ready_for_lowering = [bool]$readiness.ready_for_lowering
    }
    if (-not [bool]$readiness.parse_recovery_determinism_hardening_consistent) {
      throw "parse_recovery_determinism_hardening_consistent is false for fixture '$fixtureName' pass '$pass'"
    }
    if ([string]::IsNullOrWhiteSpace([string]$readiness.parse_recovery_determinism_hardening_key)) {
      throw "parse_recovery_determinism_hardening_key is empty for fixture '$fixtureName' pass '$pass'"
    }

    $parserCanonical = Convert-ToCanonicalJson -Value $parserProjection
    $readinessCanonical = Convert-ToCanonicalJson -Value $readinessProjection
    if ($pass -eq 1) {
      $baselineParserCanonical = $parserCanonical
      $baselineReadinessCanonical = $readinessCanonical
    } else {
      if ($baselineParserCanonical -ne $parserCanonical) {
        throw "parser stage projection drifted between pass1/pass2 for fixture '$fixtureName'"
      }
      if ($baselineReadinessCanonical -ne $readinessCanonical) {
        throw "parse_lowering_readiness projection drifted between pass1/pass2 for fixture '$fixtureName'"
      }
    }

    $passRecords.Add([ordered]@{
        pass = $pass
        manifest = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $manifestPath
        parser_stage = $parserProjection
        parse_lowering_readiness = $readinessProjection
      })
  }

  $results.Add([ordered]@{
      fixture = $fixtureName
      deterministic_match = $true
      passes = $passRecords
    })
}

$summary = [ordered]@{
  contract_id = "objc3c-parser-advanced-conformance-workpack-contract/parser_build-conformance-workpack-iteration30-v1"
  fixture_root = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $fixturesDir
  fixtures = $results
}
$summaryPath = Join-Path $reportDir "parser_conformance_shard3_summary.json"
$summary | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("summary: " + (Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $summaryPath))

