$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$toolchainScriptRoot = Split-Path -Parent $PSScriptRoot
$compileIoModule = Join-Path $toolchainScriptRoot "objc3c_native_compile_io.psm1"
$artifactsModule = Join-Path $PSScriptRoot "artifacts.psm1"
foreach ($dependencyModule in @($compileIoModule, $artifactsModule)) {
  if (!(Test-Path -LiteralPath $dependencyModule -PathType Leaf)) {
    Write-Error "native compile toolchain dependency missing at $dependencyModule"
    exit 2
  }
  Import-Module $dependencyModule -Force -DisableNameChecking
}

function Get-NativeCompilerBuildInputPaths {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return @(
    (Join-Path $RepoRoot "native/objc3c/src/main.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/contracts/objc3_frontend_diagnostics_bus_contract.h")
    (Join-Path $RepoRoot "native/objc3c/src/diag/objc3_diag_utils.h")
    (Join-Path $RepoRoot "native/objc3c/src/diag/objc3_diag_utils.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_ascii_predicates.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_file_reading.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_identifier_safe_suffix.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_ir_object_backend_token.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_method_family.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_runtime_dispatch_symbol.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_runtime_metadata_record_set.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_string_predicates.h")
    (Join-Path $RepoRoot "native/objc3c/src/support/objc3_value_type_names.h")
    (Join-Path $RepoRoot "native/objc3c/src/pipeline/objc3_frontend_types.h")
    (Join-Path $RepoRoot "native/objc3c/src/artifacts/objc3_frontend_artifacts.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_json.h")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_json.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_process.h")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_process.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/driver/objc3_objc3_path.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/runtime/objc3_runtime.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    (Join-Path $RepoRoot "scripts/build_objc3c_native.ps1")
  )
}

function Test-AnyPathNewerThanTarget {
  param(
    [Parameter(Mandatory = $true)][string]$TargetPath,
    [Parameter(Mandatory = $true)][string[]]$InputPaths
  )

  if (!(Test-Path -LiteralPath $TargetPath -PathType Leaf)) {
    return $true
  }

  $targetTimestamp = (Get-Item -LiteralPath $TargetPath).LastWriteTimeUtc
  foreach ($inputPath in $InputPaths) {
    if (!(Test-Path -LiteralPath $inputPath -PathType Leaf)) {
      continue
    }

    $inputTimestamp = (Get-Item -LiteralPath $inputPath).LastWriteTimeUtc
    if ($inputTimestamp -gt $targetTimestamp) {
      return $true
    }
  }

  return $false
}

function Test-FrontendInvocationLockCurrent {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [object]$ExistingBuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult
  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult
  if (!(Test-Path -LiteralPath $lockPath -PathType Leaf)) {
    return $false
  }
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    return $false
  }

  try {
    $payload = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
  } catch {
    return $false
  }

  if ([string]$payload.contract_id -ne "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1") {
    return $false
  }
  if ([string]$payload.scaffold_contract_id -ne "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1") {
    return $false
  }
  if ($null -eq $payload.scaffold -or [string]$payload.scaffold.sha256 -ne (Get-FileSha256Hex -Path $scaffoldPath)) {
    return $false
  }

  $expectedBinaries = @{
    "objc3c-native" = (Join-Path $CompilerRepoRoot "artifacts/bin/objc3c-native.exe")
    "objc3c-frontend-c-api-runner" = (Join-Path $CompilerRepoRoot "artifacts/bin/objc3c-frontend-c-api-runner.exe")
  }
  $observed = @{}
  foreach ($entry in @($payload.binaries)) {
    $binaryName = [string]$entry.name
    if ([string]::IsNullOrWhiteSpace($binaryName) -or -not $expectedBinaries.ContainsKey($binaryName)) {
      return $false
    }

    $binaryPath = $expectedBinaries[$binaryName]
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      return $false
    }
    $expectedRelativePath = (Resolve-Path -LiteralPath $binaryPath).Path.Substring((Resolve-Path -LiteralPath $CompilerRepoRoot).Path.Length).TrimStart('\', '/').Replace('\', '/')
    if ([string]$entry.path -ne $expectedRelativePath) {
      return $false
    }
    if ([string]$entry.sha256 -ne (Get-FileSha256Hex -Path $binaryPath)) {
      return $false
    }
    $observed[$binaryName] = $true
  }

  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $observed.ContainsKey($binaryName)) {
      return $false
    }
  }

  return $true
}

function Test-NativeCompilerBuildArtifactsReady {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [object]$ExistingBuildResult
  )

  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $CompilerRepoRoot
  $runtimeLibrary = Join-Path $CompilerRepoRoot "artifacts/lib/objc3_runtime.lib"
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    return $false
  }
  if (!(Test-Path -LiteralPath $runtimeLibrary -PathType Leaf)) {
    return $false
  }

  $requiredArtifacts = @(
    (Resolve-FrontendScaffoldPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendInvocationLockPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendEdgeCompatibilityPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendEdgeRobustnessPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendConformanceMatrixPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendConformanceCorpusPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendIntegrationCloseoutPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
  )

  foreach ($artifactPath in $requiredArtifacts) {
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      return $false
    }
  }
  if (-not (Test-FrontendInvocationLockCurrent -CompilerRepoRoot $CompilerRepoRoot -ExistingBuildResult $ExistingBuildResult)) {
    return $false
  }

  $buildInputPaths = Get-NativeCompilerBuildInputPaths -RepoRoot $CompilerRepoRoot
  if (Test-AnyPathNewerThanTarget -TargetPath $exe -InputPaths $buildInputPaths) {
    return $false
  }
  if (Test-AnyPathNewerThanTarget -TargetPath $runtimeLibrary -InputPaths $buildInputPaths) {
    return $false
  }

  return $true
}

Export-ModuleMember -Function @(
  "Get-NativeCompilerBuildInputPaths",
  "Test-AnyPathNewerThanTarget",
  "Test-FrontendInvocationLockCurrent",
  "Test-NativeCompilerBuildArtifactsReady"
)
