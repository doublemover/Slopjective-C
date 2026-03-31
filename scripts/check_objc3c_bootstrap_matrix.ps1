param(
  [string]$RunId = "m263_e002_bootstrap_matrix_closeout",
  [string]$LlcPath = $env:OBJC3C_BOOTSTRAP_MATRIX_LLC_PATH
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
$SummaryDir = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/bootstrap-matrix/$RunId"
$SummaryPath = Join-Path $SummaryDir "summary.json"
$BootstrapMatrixReportRoot = Join-Path $RepoRoot "tmp/reports/bootstrap_matrix"
$ArchiveReplaySummaryLeaf = "archive_static_link_bootstrap_replay_corpus_summary.json"
$RestartHardeningSummaryLeaf = "live_restart_hardening_summary.json"
$NativeExe = "artifacts/bin/objc3c-native.exe"
$RuntimeLibrary = "artifacts/lib/objc3_runtime.lib"

function Invoke-Step {
  param(
    [string]$Label,
    [string[]]$Command
  )

  Write-Host "[run] $($Command -join ' ')"
  if ($Command.Length -eq 0) {
    throw "$Label did not provide a command"
  }
  $Executable = $Command[0]
  [string[]]$Arguments = if ($Command.Length -gt 1) {
    $Command | Select-Object -Skip 1
  } else {
    @()
  }
  & $Executable @Arguments
  if ($LASTEXITCODE -ne 0) {
    throw "$Label failed with exit code $LASTEXITCODE"
  }
}

function Resolve-LlcPath {
  param([string]$ExplicitPath)

  if ($ExplicitPath) {
    if (-not (Test-Path -LiteralPath $ExplicitPath)) {
      throw "llc path does not exist: $ExplicitPath"
    }
    return (Resolve-Path -LiteralPath $ExplicitPath).ProviderPath
  }

  $Command = Get-Command llc.exe -ErrorAction SilentlyContinue
  if ($null -ne $Command) {
    return $Command.Source
  }

  return $null
}

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

function Get-D003CasePayload {
  param(
    [object]$Summary,
    [string]$CaseId
  )

  foreach ($Case in $Summary.dynamic_cases) {
    if ($Case.case_id -eq $CaseId) {
      return $Case.probe_payload
    }
  }

  throw "missing D003 case payload for $CaseId"
}

function Resolve-SummaryArtifactPath {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$LeafName
  )

  $matches = @(Get-ChildItem -LiteralPath $RootPath -File -Recurse -Filter $LeafName)
  if ($matches.Count -eq 0) {
    throw "missing required summary artifact named $LeafName under $RootPath"
  }
  if ($matches.Count -gt 1) {
    $rendered = ($matches | ForEach-Object { $_.FullName }) -join "; "
    throw "ambiguous summary artifact named $LeafName under ${RootPath}: $rendered"
  }
  return $matches[0].FullName
}

$ResolvedLlcPath = Resolve-LlcPath -ExplicitPath $LlcPath
if ($ResolvedLlcPath) {
  $env:PATH = "$(Split-Path -Parent $ResolvedLlcPath);$env:PATH"
}

New-Item -ItemType Directory -Force -Path $SummaryDir | Out-Null

Invoke-Step -Label "build:objc3c-native" -Command @("npm.cmd", "run", "build:objc3c-native")
Invoke-Step -Label "bootstrap_matrix-archive-static-link-replay" -Command @("python", "scripts/check_m263_archive_static_link_bootstrap_replay_corpus.py")
Invoke-Step -Label "bootstrap_matrix-live-restart-hardening" -Command @("python", "scripts/check_m263_live_restart_hardening.py")

$C003SummaryPath = Resolve-SummaryArtifactPath -RootPath $BootstrapMatrixReportRoot -LeafName $ArchiveReplaySummaryLeaf
$D003SummaryPath = Resolve-SummaryArtifactPath -RootPath $BootstrapMatrixReportRoot -LeafName $RestartHardeningSummaryLeaf
$C003SummaryRel = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $C003SummaryPath
$D003SummaryRel = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $D003SummaryPath

$C003 = Get-Content -LiteralPath $C003SummaryPath -Raw | ConvertFrom-Json
$D003 = Get-Content -LiteralPath $D003SummaryPath -Raw | ConvertFrom-Json
$DefaultD003 = Get-D003CasePayload -Summary $D003 -CaseId "default"
$ExplicitD003 = Get-D003CasePayload -Summary $D003 -CaseId "explicit"
$PlainC003 = $C003.dynamic_summary.plain_probe
$SingleC003 = $C003.dynamic_summary.single_probe
$MergedC003 = $C003.dynamic_summary.merged_probe

$Cases = @(
  [ordered]@{
    case_id = "single-image-default"
    status = if ($DefaultD003.startup_registered_image_count -eq 1 -and $DefaultD003.post_reset_registered_image_count -eq 0 -and $DefaultD003.second_restart_replay_generation -eq 2) { "PASS" } else { "FAIL" }
    kind = "single-image"
    startup_registered_image_count = [int]$DefaultD003.startup_registered_image_count
    post_reset_registered_image_count = [int]$DefaultD003.post_reset_registered_image_count
    post_restart_registered_image_count = [int]$DefaultD003.second_restart_registered_image_count
    second_restart_replay_generation = [int]$DefaultD003.second_restart_replay_generation
    source_summary = $D003SummaryRel
  }
  [ordered]@{
    case_id = "single-image-explicit"
    status = if ($ExplicitD003.startup_registered_image_count -eq 1 -and $ExplicitD003.post_reset_registered_image_count -eq 0 -and $ExplicitD003.second_restart_replay_generation -eq 2) { "PASS" } else { "FAIL" }
    kind = "single-image"
    startup_registered_image_count = [int]$ExplicitD003.startup_registered_image_count
    post_reset_registered_image_count = [int]$ExplicitD003.post_reset_registered_image_count
    post_restart_registered_image_count = [int]$ExplicitD003.second_restart_registered_image_count
    second_restart_replay_generation = [int]$ExplicitD003.second_restart_replay_generation
    source_summary = $D003SummaryRel
  }
  [ordered]@{
    case_id = "archive-backed-plain"
    status = if ($PlainC003.startup_registered_image_count -eq 0 -and $PlainC003.post_replay_registered_image_count -eq 0) { "PASS" } else { "FAIL" }
    kind = "archive-backed"
    retention = "unretained"
    startup_registered_image_count = [int]$PlainC003.startup_registered_image_count
    post_replay_registered_image_count = [int]$PlainC003.post_replay_registered_image_count
    source_summary = $C003SummaryRel
  }
  [ordered]@{
    case_id = "archive-backed-single-retained"
    status = if ($SingleC003.startup_registered_image_count -eq 1 -and $SingleC003.post_replay_registered_image_count -eq 1) { "PASS" } else { "FAIL" }
    kind = "archive-backed"
    retention = "single-retained"
    startup_registered_image_count = [int]$SingleC003.startup_registered_image_count
    post_replay_registered_image_count = [int]$SingleC003.post_replay_registered_image_count
    source_summary = $C003SummaryRel
  }
  [ordered]@{
    case_id = "archive-backed-merged-retained"
    status = if ($MergedC003.startup_registered_image_count -eq 2 -and $MergedC003.post_replay_registered_image_count -eq 2 -and $MergedC003.post_replay_next_expected_registration_order_ordinal -eq 3) { "PASS" } else { "FAIL" }
    kind = "multi-image"
    retention = "merged-retained"
    startup_registered_image_count = [int]$MergedC003.startup_registered_image_count
    post_replay_registered_image_count = [int]$MergedC003.post_replay_registered_image_count
    post_replay_next_expected_registration_order_ordinal = [int]$MergedC003.post_replay_next_expected_registration_order_ordinal
    source_summary = $C003SummaryRel
  }
)

$FailedCases = @($Cases | Where-Object { $_.status -ne "PASS" })
$Status = if ($FailedCases.Count -eq 0) { "PASS" } else { "FAIL" }
$Summary = [ordered]@{
  mode = "bootstrap_matrix-bootstrap-matrix-v1"
  run_id = $RunId
  status = $Status
  native_exe = $NativeExe
  runtime_library = $RuntimeLibrary
  llc_source = $ResolvedLlcPath
  archive_replay_summary = $C003SummaryRel
  restart_hardening_summary = $D003SummaryRel
  cases = $Cases
  commands = [ordered]@{
    build = "npm run build:objc3c-native"
    archive_replay = "python scripts/check_m263_archive_static_link_bootstrap_replay_corpus.py"
    restart_hardening = "python scripts/check_m263_live_restart_hardening.py"
  }
}

$Summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $SummaryPath -Encoding UTF8
Write-Host "summary_path: $SummaryPath"
Write-Host "status: $Status"

if ($Status -ne "PASS") {
  throw "bootstrap matrix reported FAIL"
}
