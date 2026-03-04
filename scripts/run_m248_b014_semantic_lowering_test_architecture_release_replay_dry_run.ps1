param(
  [string]$SourcePath = "tests/tooling/fixtures/native/hello.objc3",
  [string]$ReportRoot = "tmp/reports/m248/M248-B014"
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

function Get-FileSha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)
  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Assert-FileExists {
  param([Parameter(Mandatory = $true)][string]$Path)
  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "missing required artifact: $Path"
  }
}

function Read-JsonFile {
  param([Parameter(Mandatory = $true)][string]$Path)
  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$source = Join-Path $repoRoot $SourcePath
$reportDir = Join-Path $repoRoot $ReportRoot
$run1 = Join-Path $reportDir "run1"
$run2 = Join-Path $reportDir "run2"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$compileWrapper = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"

Assert-FileExists -Path $source
Assert-FileExists -Path $buildScript
Assert-FileExists -Path $compileWrapper

New-Item -ItemType Directory -Force -Path $run1 | Out-Null
New-Item -ItemType Directory -Force -Path $run2 | Out-Null

& $buildScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $compileWrapper $source --out-dir $run1 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $compileWrapper $source --out-dir $run2 --emit-prefix module
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$deterministicFiles = @(
  "module.manifest.json",
  "module.diagnostics.json",
  "module.ll",
  "module.object-backend.txt"
)

$comparisons = New-Object System.Collections.Generic.List[object]
foreach ($fileName in $deterministicFiles) {
  $path1 = Join-Path $run1 $fileName
  $path2 = Join-Path $run2 $fileName
  Assert-FileExists -Path $path1
  Assert-FileExists -Path $path2
  $text1 = Get-Content -LiteralPath $path1 -Raw
  $text2 = Get-Content -LiteralPath $path2 -Raw
  if ($text1 -ne $text2) {
    throw "deterministic replay drift detected in $fileName"
  }
  $comparisons.Add([ordered]@{
      file = $fileName
      sha256 = Get-FileSha256Hex -Path $path1
      identical = $true
    })
}

$manifest = Read-JsonFile -Path (Join-Path $run1 "module.manifest.json")
$readiness = $manifest.frontend.pipeline.parse_lowering_readiness

if (-not $readiness.ready_for_lowering) {
  throw "manifest parse_lowering_readiness.ready_for_lowering is false"
}
if (-not $readiness.parse_artifact_replay_key_deterministic) {
  throw "manifest parse_lowering_readiness.parse_artifact_replay_key_deterministic is false"
}
if (-not $readiness.parse_recovery_determinism_hardening_consistent) {
  throw "manifest parse_lowering_readiness.parse_recovery_determinism_hardening_consistent is false"
}
if (-not $readiness.parse_lowering_conformance_matrix_consistent) {
  throw "manifest parse_lowering_readiness.parse_lowering_conformance_matrix_consistent is false"
}
if (-not $readiness.parse_lowering_conformance_corpus_consistent) {
  throw "manifest parse_lowering_readiness.parse_lowering_conformance_corpus_consistent is false"
}
if (-not $readiness.parse_lowering_performance_quality_guardrails_consistent) {
  throw "manifest parse_lowering_readiness.parse_lowering_performance_quality_guardrails_consistent is false"
}
if (-not $readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent) {
  throw "manifest parse_lowering_readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent is false"
}
if (-not $readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready) {
  throw "manifest parse_lowering_readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready is false"
}
if (-not $readiness.long_tail_grammar_integration_closeout_consistent) {
  throw "manifest parse_lowering_readiness.long_tail_grammar_integration_closeout_consistent is false"
}
if (-not $readiness.long_tail_grammar_gate_signoff_ready) {
  throw "manifest parse_lowering_readiness.long_tail_grammar_gate_signoff_ready is false"
}
if (($readiness.toolchain_runtime_ga_operations_docs_runbook_sync_key -as [string]).IndexOf("long_tail_grammar_gate_signoff_ready=true", [System.StringComparison]::Ordinal) -lt 0) {
  throw "manifest toolchain_runtime_ga_operations_docs_runbook_sync_key missing long-tail gate signoff anchor"
}
if (($readiness.long_tail_grammar_integration_closeout_key -as [string]).IndexOf("toolchain_runtime_ga_operations_docs_runbook_sync_key=", [System.StringComparison]::Ordinal) -lt 0) {
  throw "manifest long_tail_grammar_integration_closeout_key missing docs/runbook sync dependency continuity"
}

$summary = [ordered]@{
  contract_id = "objc3c-semantic-lowering-test-architecture-release-candidate-replay-dry-run/m248-b014-v1"
  source = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $source
  run1 = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $run1
  run2 = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $run2
  parse_lowering_readiness = [ordered]@{
    ready_for_lowering = [bool]$readiness.ready_for_lowering
    parse_artifact_replay_key_deterministic = [bool]$readiness.parse_artifact_replay_key_deterministic
    parse_recovery_determinism_hardening_consistent = [bool]$readiness.parse_recovery_determinism_hardening_consistent
    parse_lowering_conformance_matrix_consistent = [bool]$readiness.parse_lowering_conformance_matrix_consistent
    parse_lowering_conformance_corpus_consistent = [bool]$readiness.parse_lowering_conformance_corpus_consistent
    parse_lowering_performance_quality_guardrails_consistent = [bool]$readiness.parse_lowering_performance_quality_guardrails_consistent
    toolchain_runtime_ga_operations_docs_runbook_sync_consistent = [bool]$readiness.toolchain_runtime_ga_operations_docs_runbook_sync_consistent
    toolchain_runtime_ga_operations_docs_runbook_sync_ready = [bool]$readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready
    long_tail_grammar_integration_closeout_consistent = [bool]$readiness.long_tail_grammar_integration_closeout_consistent
    long_tail_grammar_gate_signoff_ready = [bool]$readiness.long_tail_grammar_gate_signoff_ready
    toolchain_runtime_ga_operations_docs_runbook_sync_key = [string]$readiness.toolchain_runtime_ga_operations_docs_runbook_sync_key
    long_tail_grammar_integration_closeout_key = [string]$readiness.long_tail_grammar_integration_closeout_key
  }
  deterministic_files = $comparisons
}

$summaryPath = Join-Path $reportDir "replay_dry_run_summary.json"
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("summary: " + (Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $summaryPath))
