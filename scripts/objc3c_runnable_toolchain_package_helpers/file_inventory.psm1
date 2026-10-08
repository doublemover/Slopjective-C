Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "path_normalization.psm1") -Force -DisableNameChecking

function Assert-RepoFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $fullPath = Join-PackageRelativePath -RootPath $RepoRoot -RelativePath $RelativePath
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
  $destinationPath = Join-PackageRelativePath -RootPath $PackageRoot -RelativePath $RelativePath
  $destinationDir = Split-Path -Parent $destinationPath
  New-Item -ItemType Directory -Force -Path $destinationDir | Out-Null
  Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -Force
  return $destinationPath
}

function Get-RepoRelativeFilesUnderRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativeRoot,
    [Parameter(Mandatory = $true)][string]$MissingRootMessage,
    [string]$Filter = "*"
  )

  $root = Join-Path $RepoRoot $RelativeRoot
  if (!(Test-Path -LiteralPath $root -PathType Container)) {
    throw $MissingRootMessage
  }

  return @(
    Get-ChildItem -LiteralPath $root -Recurse -File -Filter $Filter |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeExecutionFixtureFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $fixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/execution"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native/execution" `
    -MissingRootMessage "runnable toolchain package FAIL: missing execution fixture root $fixtureRoot")
}

function Get-RepoRelativeNativeFixtureFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $fixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native" `
    -MissingRootMessage "runnable toolchain package FAIL: missing native fixture root $fixtureRoot")
}

function Get-RepoRelativeStdlibFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $stdlibRoot = Join-Path $RepoRoot "stdlib"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "stdlib" `
    -MissingRootMessage "runnable toolchain package FAIL: missing stdlib root $stdlibRoot")
}

function Get-RepoRelativeConformanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $conformanceRoot = Join-Path $RepoRoot "tests/conformance"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/conformance" `
      -MissingRootMessage "runnable toolchain package FAIL: missing conformance root $conformanceRoot")
}

function Get-RepoRelativeConformanceSurfacePythonFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $surfaceModelRoot = Join-Path $RepoRoot "scripts/conformance_corpus_surface_model"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/conformance_corpus_surface_model" `
    -MissingRootMessage "runnable toolchain package FAIL: missing conformance corpus surface model root $surfaceModelRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeNativeDocsFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $docsRoot = Join-Path $RepoRoot "docs/objc3c-native"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "docs/objc3c-native" `
    -MissingRootMessage "runnable toolchain package FAIL: missing native docs root $docsRoot")
}

function Get-RepoRelativeNativeRuntimeSourceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $runtimeRoot = Join-Path $RepoRoot "native/objc3c/src/runtime"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "native/objc3c/src/runtime" `
    -MissingRootMessage "runnable toolchain package FAIL: missing native runtime source root $runtimeRoot")
}

function Get-RepoRelativePythonToolingFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $toolingRoot = Join-Path $RepoRoot "scripts/objc3c_tooling"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_tooling" `
    -MissingRootMessage "runnable toolchain package FAIL: missing Python tooling root $toolingRoot" `
    -Filter "*.py")
}

function Get-RepoRelativePythonSharedFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $sharedRoot = Join-Path $RepoRoot "scripts/objc3c_shared"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_shared" `
    -MissingRootMessage "runnable toolchain package FAIL: missing Python shared tooling root $sharedRoot" `
    -Filter "*.py")
}

function Get-RepoRelativePackagedPythonScriptFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $scriptsRoot = Join-Path $RepoRoot "scripts"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts" `
    -MissingRootMessage "runnable toolchain package FAIL: missing scripts root $scriptsRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeWorkflowPythonFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $workflowRoot = Join-Path $RepoRoot "scripts/objc3c_workflow"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_workflow" `
    -MissingRootMessage "runnable toolchain package FAIL: missing workflow Python root $workflowRoot" `
    -Filter "*.py")
}

function Get-RepoRelativePerformanceBenchmarkFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $performanceRoots = @(
    "scripts/objc3c_performance_benchmark",
    "scripts/objc3c_comparative_baselines"
  )

  return @(
    "scripts/benchmark_objc3c_performance.py",
    "scripts/benchmark_objc3c_runtime_performance.py",
    "scripts/check_objc3c_performance_integration.py",
    "scripts/check_objc3c_runtime_performance_integration.py",
    "scripts/check_objc3c_runnable_performance_end_to_end.py",
    "scripts/check_objc3c_runnable_runtime_performance_end_to_end.py",
    "scripts/run_objc3c_comparative_baselines.py",
    "scripts/objc3c_performance_reproducibility.py"
    foreach ($performanceRoot in $performanceRoots) {
      $absoluteRoot = Join-Path $RepoRoot $performanceRoot
      Get-RepoRelativeFilesUnderRoot `
        -RepoRoot $RepoRoot `
        -RelativeRoot $performanceRoot `
        -MissingRootMessage "runnable toolchain package FAIL: missing performance benchmark Python root $absoluteRoot" `
        -Filter "*.py"
    }
  )
}

function Get-RepoRelativeRuntimeAcceptanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $acceptanceRoot = Join-Path $RepoRoot "scripts/objc3c_runtime_acceptance"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_runtime_acceptance" `
    -MissingRootMessage "runnable toolchain package FAIL: missing runtime acceptance package root $acceptanceRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeRuntimeProbeFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $runtimeProbeRoot = Join-Path $RepoRoot "tests/tooling/runtime"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/runtime" `
    -MissingRootMessage "runnable toolchain package FAIL: missing runtime probe root $runtimeProbeRoot")
}

function Get-RepoRelativeNativeCompileSupportFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $supportRoots = @(
    "scripts/objc3c_native_cmake",
    "scripts/objc3c_native_frontend_contracts",
    "scripts/objc3c_native_frontend_artifacts",
    "scripts/objc3c_native_frontend_closeout_edge_artifacts",
    "scripts/objc3c_native_frontend_closeout_conformance_artifacts",
    "scripts/objc3c_native_superclean_surface_catalog",
    "scripts/objc3c_native_compile_arguments",
    "scripts/objc3c_native_compile_io",
    "scripts/objc3c_native_compile_toolchain",
    "scripts/objc3c_native_compile_frontend_guards",
    "scripts/objc3c_native_compile_frontend_artifact_guards",
    "scripts/objc3c_native_compile_frontend_feature_guards",
    "scripts/objc3c_native_compile_frontend_hardening_guards",
    "scripts/objc3c_native_compile_frontend_conformance_guards",
    "scripts/objc3c_native_compile_provenance",
    "scripts/objc3c_native_compile_wrapper"
  )

  return @(
    "scripts/normalize_coff_archive_timestamps.py",
    "scripts/objc3c_native_artifact_io.psm1",
    "scripts/objc3c_native_cmake.psm1",
    "scripts/objc3c_native_frontend_contracts.psm1",
    "scripts/objc3c_native_frontend_artifacts.psm1",
    "scripts/objc3c_native_frontend_closeout_artifacts.psm1",
    "scripts/objc3c_native_frontend_closeout_edge_artifacts.psm1",
    "scripts/objc3c_native_frontend_closeout_conformance_artifacts.psm1",
    "scripts/objc3c_native_superclean_surface.psm1",
    "scripts/objc3c_native_superclean_surface_catalog.psm1"
    foreach ($supportRoot in $supportRoots) {
      $absoluteRoot = Join-Path $RepoRoot $supportRoot
      Get-RepoRelativeFilesUnderRoot `
        -RepoRoot $RepoRoot `
        -RelativeRoot $supportRoot `
        -MissingRootMessage "runnable toolchain package FAIL: missing native compile support root $absoluteRoot"
    }
  )
}

function Get-RepoRelativeNativeExecutionSupportFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $supportRoots = @(
    "scripts/objc3c_native_execution_smoke_helpers",
    "scripts/objc3c_native_execution_smoke_runner",
    "scripts/objc3c_execution_replay_proof_helpers"
  )

  return @(
    "scripts/objc3c_native_execution_smoke_helpers.psm1",
    "scripts/objc3c_native_execution_smoke_runner.psm1",
    "scripts/objc3c_execution_replay_proof_helpers.psm1"
    foreach ($supportRoot in $supportRoots) {
      $absoluteRoot = Join-Path $RepoRoot $supportRoot
      Get-RepoRelativeFilesUnderRoot `
        -RepoRoot $RepoRoot `
        -RelativeRoot $supportRoot `
        -MissingRootMessage "runnable toolchain package FAIL: missing native execution support root $absoluteRoot"
    }
  )
}

function Get-RepoRelativeRecoveryPositiveFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $recoveryRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery/positive"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native/recovery/positive" `
    -MissingRootMessage "runnable toolchain package FAIL: missing recovery-positive root $recoveryRoot")
}

Export-ModuleMember -Function @(
  "Copy-RepoRelativeFile",
  "Get-RepoRelativeConformanceFiles",
  "Get-RepoRelativeConformanceSurfacePythonFiles",
  "Get-RepoRelativeExecutionFixtureFiles",
  "Get-RepoRelativeNativeCompileSupportFiles",
  "Get-RepoRelativeNativeDocsFiles",
  "Get-RepoRelativeNativeExecutionSupportFiles",
  "Get-RepoRelativeNativeFixtureFiles",
  "Get-RepoRelativeNativeRuntimeSourceFiles",
  "Get-RepoRelativePackagedPythonScriptFiles",
  "Get-RepoRelativePerformanceBenchmarkFiles",
  "Get-RepoRelativePythonSharedFiles",
  "Get-RepoRelativePythonToolingFiles",
  "Get-RepoRelativeRecoveryPositiveFiles",
  "Get-RepoRelativeRuntimeAcceptanceFiles",
  "Get-RepoRelativeRuntimeProbeFiles",
  "Get-RepoRelativeStdlibFiles",
  "Get-RepoRelativeWorkflowPythonFiles"
)
