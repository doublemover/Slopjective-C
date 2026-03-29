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
  "showcase/README.md",
  "showcase/portfolio.json",
  "showcase/tutorial_walkthrough.json",
  "showcase/auroraBoard/main.objc3",
  "showcase/auroraBoard/workspace.json",
  "showcase/signalMesh/main.objc3",
  "showcase/signalMesh/workspace.json",
  "showcase/patchKit/main.objc3",
  "showcase/patchKit/workspace.json",
  "docs/tutorials/getting_started.md",
  "docs/tutorials/build_run_verify.md",
  "docs/tutorials/guided_walkthrough.md",
  "scripts/probe_objc3c_llvm_capabilities.py",
  "tmp/artifacts/objc3c-native/frontend_source_graph.json",
  "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
  "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
  "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
  "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
  "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
  "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
  "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
  "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
  "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
  "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json",
  "native/objc3c/src/runtime/objc3_runtime.h",
  "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
  "schemas/objc3-conformance-dashboard-status-v1.schema.json",
  "scripts/check_release_evidence.py",
  "spec/conformance/release_evidence_gate_maintenance.md",
  "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
  "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp",
  "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp",
  "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
  "tests/tooling/runtime/live_error_runtime_integration_probe.cpp",
  "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp",
  "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
  "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
  "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp",
  "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp",
  "tests/tooling/runtime/header_module_bridge_generation_probe.cpp",
  "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp",
  "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp",
  "tests/tooling/fixtures/native/hello.objc3",
  "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3",
  "tests/tooling/fixtures/native/live_dispatch_fast_path_positive.objc3",
  "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3",
  "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3",
  "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
  "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3",
  "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
  "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
  "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3",
  "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3",
  "tests/tooling/fixtures/native/macro_host_process_provider.objc3",
  "tests/tooling/fixtures/native/macro_host_process_consumer.objc3",
  "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3",
  "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
  "tests/tooling/fixtures/native/header_module_bridge_provider.objc3",
  "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
)

$executionFixtureFiles = @(Get-RepoRelativeExecutionFixtureFiles -RepoRoot $repoRoot)
$copiedRelativePaths = New-Object System.Collections.Generic.List[string]
foreach ($relativePath in @($requiredRelativeFiles + $executionFixtureFiles)) {
  Copy-RepoRelativeFile -RepoRoot $repoRoot -PackageRoot $packageRoot -RelativePath $relativePath | Out-Null
  $copiedRelativePaths.Add($relativePath.Replace('\\', '/')) | Out-Null
}

$repoSupercleanSurfaceRelativePath = "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"
$repoSupercleanSurfacePath = Join-Path $packageRoot ($repoSupercleanSurfaceRelativePath.Replace('/', '\'))
$repoSupercleanSurfacePayload = Get-Content -LiteralPath $repoSupercleanSurfacePath -Raw | ConvertFrom-Json -AsHashtable
if (-not $repoSupercleanSurfacePayload.ContainsKey("bonus_experience_surfaces")) {
  throw "runnable toolchain package FAIL: missing bonus_experience_surfaces in $repoSupercleanSurfaceRelativePath"
}

$manifestPayload = [ordered]@{
  contract_id = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
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
  showcase_root = "showcase"
  showcase_readme = "showcase/README.md"
  showcase_portfolio = "showcase/portfolio.json"
  showcase_examples = @(
    [ordered]@{
      example_id = "auroraBoard"
      source = "showcase/auroraBoard/main.objc3"
      workspace_manifest = "showcase/auroraBoard/workspace.json"
      expected_exit_code = 33
    },
    [ordered]@{
      example_id = "signalMesh"
      source = "showcase/signalMesh/main.objc3"
      workspace_manifest = "showcase/signalMesh/workspace.json"
      expected_exit_code = 13
    },
    [ordered]@{
      example_id = "patchKit"
      source = "showcase/patchKit/main.objc3"
      workspace_manifest = "showcase/patchKit/workspace.json"
      expected_exit_code = 7
    }
  )
  canonical_runnable_fixture = "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3"
  runtime_public_header = "native/objc3c/src/runtime/objc3_runtime.h"
  runtime_internal_header = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
  release_evidence_dashboard_schema = "schemas/objc3-conformance-dashboard-status-v1.schema.json"
  release_evidence_gate_script = "scripts/check_release_evidence.py"
  release_evidence_runbook = "spec/conformance/release_evidence_gate_maintenance.md"
  release_candidate_fixture = "tests/tooling/fixtures/native/hello.objc3"
  release_candidate_claim_probe = "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp"
  release_candidate_evidence_probe = "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp"
  object_model_probe = "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp"
  block_arc_fixture = "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3"
  block_arc_runtime_abi_probe = "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp"
  block_arc_byref_forwarding_probe = "tests/tooling/runtime/block_runtime_byref_forwarding_probe.cpp"
  storage_reflection_fixture = "tests/tooling/fixtures/native/runtime_backed_storage_ownership_reflection_positive.objc3"
  storage_reflection_probe = "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp"
  error_runtime_fixture = "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3"
  error_runtime_probe = "tests/tooling/runtime/live_error_runtime_integration_probe.cpp"
  continuation_runtime_fixture = "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3"
  continuation_runtime_probe = "tests/tooling/runtime/live_continuation_runtime_integration_probe.cpp"
  task_runtime_fixture = "tests/tooling/fixtures/native/live_task_runtime_and_executor_implementation_positive.objc3"
  task_runtime_probe = "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp"
  actor_runtime_fixture = "tests/tooling/fixtures/native/actor_lowering_runtime_positive.objc3"
  actor_runtime_probe = "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp"
  metaprogramming_runtime_fixture = "tests/tooling/fixtures/native/macro_host_process_provider.objc3"
  metaprogramming_runtime_consumer_fixture = "tests/tooling/fixtures/native/macro_host_process_consumer.objc3"
  metaprogramming_runtime_probe = "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
  interop_runtime_fixture = "tests/tooling/fixtures/native/bridge_packaging_toolchain_provider.objc3"
  interop_runtime_consumer_fixture = "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3"
  interop_header_bridge_fixture = "tests/tooling/fixtures/native/header_module_bridge_provider.objc3"
  interop_header_bridge_consumer_fixture = "tests/tooling/fixtures/native/header_module_bridge_consumer.objc3"
  interop_packaging_probe = "tests/tooling/runtime/bridge_packaging_toolchain_probe.cpp"
  interop_bridge_generation_probe = "tests/tooling/runtime/header_module_bridge_generation_probe.cpp"
  execution_fixture_root = "tests/tooling/fixtures/native/execution"
  frontend_contract_artifacts = @(
    "tmp/artifacts/objc3c-native/frontend_source_graph.json",
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
  repo_superclean_surface = $repoSupercleanSurfaceRelativePath
  bonus_experience_surfaces = $repoSupercleanSurfacePayload["bonus_experience_surfaces"]
  guided_walkthrough_manifest = "showcase/tutorial_walkthrough.json"
  tutorial_guides = @(
    "docs/tutorials/getting_started.md",
    "docs/tutorials/build_run_verify.md",
    "docs/tutorials/guided_walkthrough.md"
  )
  capability_probe_script = "scripts/probe_objc3c_llvm_capabilities.py"
  command_surfaces = [ordered]@{
    build = "npm run build:objc3c-native"
    package = "npm run package:objc3c-native:runnable-toolchain"
    compile = "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module"
    build_playground = "npm run build:objc3c:playground"
    inspect_playground = "npm run inspect:objc3c:playground"
    inspect_capabilities = "npm run inspect:objc3c:capabilities"
    inspect_runtime = "npm run inspect:objc3c:runtime"
    trace_stages = "npm run trace:objc3c:stages"
    developer_tooling = "npm run test:objc3c:developer-tooling"
    showcase = "npm run test:showcase"
    showcase_e2e = "npm run test:showcase:e2e"
    getting_started = "npm run test:getting-started"
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
