param(
  [string]$PackageRoot = "",
  [string]$ManifestRelativePath = "artifacts/package/objc3c-runnable-toolchain-package.json"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"

function Get-RepoRelativePathCompat {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  if (Test-Path -LiteralPath $TargetPath) {
    $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  }
  else {
    $resolvedTarget = [System.IO.Path]::GetFullPath($TargetPath)
  }

  if ($resolvedRoot.EndsWith('\\') -or $resolvedRoot.EndsWith('/')) {
    $rootWithSeparator = $resolvedRoot
  }
  else {
    $rootWithSeparator = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  }

  $relativePath = $null
  $getRelativeMethod = [System.IO.Path].GetMethod("GetRelativePath", [Type[]]@([string], [string]))
  if ($null -ne $getRelativeMethod) {
    $relativePath = [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)
  }
  else {
    $rootUri = New-Object System.Uri($rootWithSeparator)
    $targetUri = New-Object System.Uri($resolvedTarget)
    $relativeUri = $rootUri.MakeRelativeUri($targetUri)
    $relativePath = [System.Uri]::UnescapeDataString($relativeUri.ToString())
  }

  return $relativePath.Replace('\\', '/')
}

function Resolve-PackageRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$RequestedRoot
  )

  if ([string]::IsNullOrWhiteSpace($RequestedRoot)) {
    $runId = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
    return (Join-Path $RepoRoot (Join-Path "tmp/pkg/objc3c-native-runnable-toolchain" $runId))
  }

  if ([System.IO.Path]::IsPathRooted($RequestedRoot)) {
    return [System.IO.Path]::GetFullPath($RequestedRoot)
  }

  return [System.IO.Path]::GetFullPath((Join-Path $RepoRoot $RequestedRoot))
}

function Assert-RepoFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $fullPath = Join-Path $RepoRoot ($RelativePath.Replace('/', '\\'))
  if (!(Test-Path -LiteralPath $fullPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: missing required file $RelativePath"
  }

  return $fullPath
}

function Copy-RepoRelativeFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $sourcePath = Assert-RepoFile -RepoRoot $RepoRoot -RelativePath $RelativePath
  $destinationPath = Join-Path $PackageRoot ($RelativePath.Replace('/', '\\'))
  $destinationDir = Split-Path -Parent $destinationPath
  New-Item -ItemType Directory -Force -Path $destinationDir | Out-Null
  Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -Force
  return $destinationPath
}

function Get-RepoRelativeExecutionFixtureFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $fixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/execution"
  if (!(Test-Path -LiteralPath $fixtureRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: missing execution fixture root $fixtureRoot"
  }

  return @(
    Get-ChildItem -LiteralPath $fixtureRoot -Recurse -File |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

if (!(Test-Path -LiteralPath $buildScript -PathType Leaf)) {
  throw "runnable toolchain package FAIL: missing build script $buildScript"
}

$packageRoot = Resolve-PackageRoot -RepoRoot $repoRoot -RequestedRoot $PackageRoot
$manifestPath = Join-Path $packageRoot ($ManifestRelativePath.Replace('/', '\\'))
$manifestDir = Split-Path -Parent $manifestPath
New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null

& $buildScript
if ($LASTEXITCODE -ne 0) {
  exit $LASTEXITCODE
}

$requiredRelativeFiles = @(
  "artifacts/bin/objc3c-native.exe",
  "artifacts/bin/objc3c-frontend-c-api-runner.exe",
  "artifacts/lib/objc3_runtime.lib",
  "scripts/objc3c_native_compile.ps1",
  "scripts/objc3c_runtime_launch_contract.ps1",
  "scripts/run_objc3c_native_compile_proof.ps1",
  "scripts/check_objc3c_native_execution_smoke.ps1",
  "scripts/check_objc3c_execution_replay_proof.ps1",
  "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json",
  "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
  "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
  "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
  "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
  "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
  "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
  "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
  "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
  "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
  "native/objc3c/src/runtime/objc3_runtime.h",
  "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
  "tests/tooling/runtime/m259_d002_realization_lookup_reflection_runtime_probe.cpp",
  "tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp",
  "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp",
  "tests/tooling/runtime/m281_d001_block_arc_runtime_abi_probe.cpp",
  "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3",
  "tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3",
  "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3"
)

$executionFixtureFiles = @(Get-RepoRelativeExecutionFixtureFiles -RepoRoot $repoRoot)
$copiedRelativePaths = New-Object System.Collections.Generic.List[string]
foreach ($relativePath in @($requiredRelativeFiles + $executionFixtureFiles)) {
  Copy-RepoRelativeFile -RepoRoot $repoRoot -PackageRoot $packageRoot -RelativePath $relativePath | Out-Null
  $copiedRelativePaths.Add($relativePath.Replace('\\', '/')) | Out-Null
}

$manifestPayload = [ordered]@{
  contract_id = "objc3c-runnable-build-install-run-package/m259-d002-v1"
  schema_version = 1
  package_model = "staged-runnable-toolchain-bundle-with-repo-relative-layout"
  install_model = "local-package-root-not-system-install"
  package_root = Get-RepoRelativePathCompat -RootPath $repoRoot -TargetPath $packageRoot
  manifest_artifact = Get-RepoRelativePathCompat -RootPath $packageRoot -TargetPath $manifestPath
  native_executable = "artifacts/bin/objc3c-native.exe"
  frontend_c_api_runner = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  runtime_library = "artifacts/lib/objc3_runtime.lib"
  compile_wrapper = "scripts/objc3c_native_compile.ps1"
  runtime_launch_contract_script = "scripts/objc3c_runtime_launch_contract.ps1"
  compile_proof_script = "scripts/run_objc3c_native_compile_proof.ps1"
  execution_smoke_script = "scripts/check_objc3c_native_execution_smoke.ps1"
  execution_replay_script = "scripts/check_objc3c_execution_replay_proof.ps1"
  canonical_runnable_fixture = "tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3"
  runtime_public_header = "native/objc3c/src/runtime/objc3_runtime.h"
  runtime_internal_header = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
  object_model_probe = "tests/tooling/runtime/m259_d002_realization_lookup_reflection_runtime_probe.cpp"
  block_arc_fixture = "tests/tooling/fixtures/native/m261_byref_cell_copy_dispose_runtime_positive.objc3"
  block_arc_runtime_abi_probe = "tests/tooling/runtime/m281_d001_block_arc_runtime_abi_probe.cpp"
  block_arc_byref_forwarding_probe = "tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp"
  storage_reflection_fixture = "tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_reflection_positive.objc3"
  storage_reflection_probe = "tests/tooling/runtime/m260_runtime_backed_storage_ownership_reflection_probe.cpp"
  execution_fixture_root = "tests/tooling/fixtures/native/execution"
  frontend_contract_artifacts = @(
    "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json",
    "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
    "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
    "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
    "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
    "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
    "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
  )
  command_surfaces = [ordered]@{
    build = "npm run build:objc3c-native"
    package = "npm run package:objc3c-native:runnable-toolchain"
    compile = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module"
    smoke = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1"
    replay = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1"
  }
  truthful_boundary = @(
    "staged local package root only",
    "no system install claim",
    "no cross-platform packaging claim",
    "no toolchain auto-provisioning claim"
  )
  copied_files = @($copiedRelativePaths | Sort-Object)
  copied_file_count = $copiedRelativePaths.Count
}

$manifestPayload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $manifestPath -Encoding utf8

Write-Output "status: PASS"
Write-Output ("package_root: " + $manifestPayload.package_root)
Write-Output ("manifest: " + $manifestPayload.manifest_artifact)
