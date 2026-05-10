$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-BuildNativeCompiler {
  param([string]$RepoRoot)

  $buildScript = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"
  $buildOutput = @(& $buildScript)
  $buildOutputLines = New-Object System.Collections.Generic.List[string]
  $frontendSourceGraphRelativePath = $null
  $frontendInvocationLockRelativePath = $null
  $frontendCoreFeatureExpansionRelativePath = $null
  $frontendEdgeCompatRelativePath = $null
  $frontendEdgeRobustnessRelativePath = $null
  $frontendDiagnosticsHardeningRelativePath = $null
  $frontendRecoveryDeterminismHardeningRelativePath = $null
  $frontendConformanceMatrixRelativePath = $null
  $frontendConformanceCorpusRelativePath = $null
  $frontendIntegrationCloseoutRelativePath = $null
  foreach ($line in $buildOutput) {
    $lineText = [string]$line
    $buildOutputLines.Add($lineText)
    if ($lineText.StartsWith("frontend_source_graph=")) {
      $frontendSourceGraphRelativePath = $lineText.Substring("frontend_source_graph=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_invocation_lock=")) {
      $frontendInvocationLockRelativePath = $lineText.Substring("frontend_invocation_lock=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_core_feature_expansion=")) {
      $frontendCoreFeatureExpansionRelativePath = $lineText.Substring("frontend_core_feature_expansion=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_edge_compat=")) {
      $frontendEdgeCompatRelativePath = $lineText.Substring("frontend_edge_compat=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_edge_robustness=")) {
      $frontendEdgeRobustnessRelativePath = $lineText.Substring("frontend_edge_robustness=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_diagnostics_hardening=")) {
      $frontendDiagnosticsHardeningRelativePath = $lineText.Substring("frontend_diagnostics_hardening=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_recovery_determinism_hardening=")) {
      $frontendRecoveryDeterminismHardeningRelativePath = $lineText.Substring("frontend_recovery_determinism_hardening=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_conformance_matrix=")) {
      $frontendConformanceMatrixRelativePath = $lineText.Substring("frontend_conformance_matrix=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_conformance_corpus=")) {
      $frontendConformanceCorpusRelativePath = $lineText.Substring("frontend_conformance_corpus=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_integration_closeout=")) {
      $frontendIntegrationCloseoutRelativePath = $lineText.Substring("frontend_integration_closeout=".Length).Trim()
    }
  }
  return [pscustomobject]@{
    exit_code = [int]$LASTEXITCODE
    build_output_lines = $buildOutputLines.ToArray()
    frontend_source_graph_relative_path = $frontendSourceGraphRelativePath
    frontend_invocation_lock_relative_path = $frontendInvocationLockRelativePath
    frontend_core_feature_expansion_relative_path = $frontendCoreFeatureExpansionRelativePath
    frontend_edge_compat_relative_path = $frontendEdgeCompatRelativePath
    frontend_edge_robustness_relative_path = $frontendEdgeRobustnessRelativePath
    frontend_diagnostics_hardening_relative_path = $frontendDiagnosticsHardeningRelativePath
    frontend_recovery_determinism_hardening_relative_path = $frontendRecoveryDeterminismHardeningRelativePath
    frontend_conformance_matrix_relative_path = $frontendConformanceMatrixRelativePath
    frontend_conformance_corpus_relative_path = $frontendConformanceCorpusRelativePath
    frontend_integration_closeout_relative_path = $frontendIntegrationCloseoutRelativePath
  }
}

function Invoke-NativeCompiler {
  param(
    [string]$ExePath,
    [string[]]$Arguments
  )

  $process = Start-Process -FilePath $ExePath -ArgumentList $Arguments -NoNewWindow -Wait -PassThru
  return [int]$process.ExitCode
}

function Resolve-FrontendScaffoldPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_source_graph.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_source_graph_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend source graph"
}

function Resolve-FrontendInvocationLockPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_invocation_lock_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend invocation lock"
}

function Resolve-FrontendCoreFeatureExpansionPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_core_feature_expansion_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend core feature expansion"
}

function Resolve-FrontendEdgeCompatibilityPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_edge_compat_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend edge compatibility"
}

function Resolve-FrontendEdgeRobustnessPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_edge_robustness_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend edge robustness"
}

function Resolve-FrontendDiagnosticsHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_diagnostics_hardening_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend diagnostics hardening"
}

function Resolve-FrontendRecoveryDeterminismHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_recovery_determinism_hardening_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend recovery determinism hardening"
}

function Resolve-FrontendConformanceMatrixPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_conformance_matrix_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend conformance matrix"
}

function Resolve-FrontendConformanceCorpusPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_conformance_corpus_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend conformance corpus"
}

function Resolve-FrontendIntegrationCloseoutPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_integration_closeout_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend integration closeout"
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

function Ensure-NativeCompilerAvailable {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult
  )

  if (Test-NativeCompilerBuildArtifactsReady -CompilerRepoRoot $RepoRoot -ExistingBuildResult $BuildResult) {
    return $BuildResult
  }

  $nextBuildResult = Invoke-BuildNativeCompiler -RepoRoot $RepoRoot
  foreach ($lineText in @($nextBuildResult.build_output_lines)) {
    Write-Host $lineText
  }
  $buildExit = [int]$nextBuildResult.exit_code
  if ($buildExit -ne 0) {
    exit $buildExit
  }

  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $RepoRoot
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "native compiler executable missing at $exe"
    exit 2
  }

  return $nextBuildResult
}

function Resolve-NativeCompilerExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $configuredNativeExe = [string]$env:OBJC3C_NATIVE_EXECUTABLE
  if (-not [string]::IsNullOrWhiteSpace($configuredNativeExe)) {
    return [System.IO.Path]::GetFullPath($configuredNativeExe)
  }
  return (Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe")
}

Export-ModuleMember -Function @(
  "Ensure-NativeCompilerAvailable",
  "Invoke-NativeCompiler",
  "Resolve-FrontendConformanceCorpusPath",
  "Resolve-FrontendConformanceMatrixPath",
  "Resolve-FrontendCoreFeatureExpansionPath",
  "Resolve-FrontendDiagnosticsHardeningPath",
  "Resolve-FrontendEdgeCompatibilityPath",
  "Resolve-FrontendEdgeRobustnessPath",
  "Resolve-FrontendIntegrationCloseoutPath",
  "Resolve-FrontendInvocationLockPath",
  "Resolve-FrontendRecoveryDeterminismHardeningPath",
  "Resolve-FrontendScaffoldPath",
  "Resolve-NativeCompilerExecutablePath"
)
