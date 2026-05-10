$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$defaultOutDir = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native"
$compileArgumentsScript = Join-Path $repoRoot "scripts/objc3c_native_compile_arguments.ps1"
$compileIoModule = Join-Path $repoRoot "scripts/objc3c_native_compile_io.psm1"
$runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
if (!(Test-Path -LiteralPath $compileArgumentsScript -PathType Leaf)) {
  Write-Error "native compile argument helper missing at $compileArgumentsScript"
  exit 2
}
if (!(Test-Path -LiteralPath $compileIoModule -PathType Leaf)) {
  Write-Error "native compile IO helper missing at $compileIoModule"
  exit 2
}
if (!(Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
  Write-Error "runtime launch contract helper missing at $runtimeLaunchContractScript"
  exit 2
}
. $compileArgumentsScript
Import-Module $compileIoModule -Force -DisableNameChecking
. $runtimeLaunchContractScript


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

function Assert-FrontendModuleScaffold {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend modular scaffold artifact missing at $scaffoldPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $scaffoldPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend modular scaffold artifact is not valid JSON at $scaffoldPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend modular scaffold contract id mismatch in $scaffoldPath"
    exit 2
  }

  $modules = @($payload.modules)
  $requiredModules = @("driver","diagnostics-io","ir","lex-parse","frontend-api","lowering","pipeline","sema")
  $presentModules = @{}
  foreach ($module in $modules) {
    $moduleName = [string]$module.name
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      Write-Error "frontend modular scaffold has module with missing name in $scaffoldPath"
      exit 2
    }
    $moduleSources = @($module.sources)
    if ($moduleSources.Count -eq 0) {
      Write-Error "frontend modular scaffold module '$moduleName' has no sources in $scaffoldPath"
      exit 2
    }
    $presentModules[$moduleName] = $true
  }
  foreach ($moduleName in $requiredModules) {
    if (-not $presentModules.ContainsKey($moduleName)) {
      Write-Error "frontend modular scaffold missing required module '$moduleName' in $scaffoldPath"
      exit 2
    }
  }

  $sharedSources = @($payload.shared_sources)
  if ($sharedSources.Count -eq 0) {
    Write-Error "frontend modular scaffold shared_sources must be non-empty in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.module_count -ne $modules.Count) {
    Write-Error "frontend modular scaffold module_count mismatch in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.shared_source_count -ne $sharedSources.Count) {
    Write-Error "frontend modular scaffold shared_source_count mismatch in $scaffoldPath"
    exit 2
  }
}

function Assert-FrontendInvocationLock {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $lockPath -PathType Leaf)) {
    Write-Error "frontend invocation lock artifact missing at $lockPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend invocation lock artifact is not valid JSON at $lockPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend invocation lock contract id mismatch in $lockPath"
    exit 2
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$payload.scaffold_contract_id -ne $expectedScaffoldContractId) {
    Write-Error "frontend invocation lock scaffold contract id mismatch in $lockPath"
    exit 2
  }

  $scaffold = $payload.scaffold
  if ($null -eq $scaffold) {
    Write-Error "frontend invocation lock scaffold metadata missing in $lockPath"
    exit 2
  }

  $scaffoldRelativePath = [string]$scaffold.path
  $scaffoldExpectedHash = [string]$scaffold.sha256
  if ([string]::IsNullOrWhiteSpace($scaffoldRelativePath) -or [string]::IsNullOrWhiteSpace($scaffoldExpectedHash)) {
    Write-Error "frontend invocation lock scaffold metadata invalid in $lockPath"
    exit 2
  }

  $scaffoldPath = $scaffoldRelativePath
  if (-not [System.IO.Path]::IsPathRooted($scaffoldPath)) {
    $scaffoldPath = Join-Path $RepoRoot $scaffoldPath
  }
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend invocation lock scaffold path missing at $scaffoldPath"
    exit 2
  }
  $scaffoldActualHash = Get-FileSha256Hex -Path $scaffoldPath
  if ($scaffoldActualHash -ne $scaffoldExpectedHash.ToLowerInvariant()) {
    Write-Error "frontend invocation lock scaffold sha256 mismatch in $lockPath"
    exit 2
  }

  $binaries = @($payload.binaries)
  if ($binaries.Count -lt 2) {
    Write-Error "frontend invocation lock binaries list must include native and c-api runner entries in $lockPath"
    exit 2
  }

  $expectedBinaries = [ordered]@{
    "objc3c-native" = "artifacts/bin/objc3c-native.exe"
    "objc3c-frontend-c-api-runner" = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  }
  $binaryIndex = @{}
  foreach ($binary in $binaries) {
    $binaryName = [string]$binary.name
    $binaryPath = [string]$binary.path
    $binaryHash = [string]$binary.sha256
    if ([string]::IsNullOrWhiteSpace($binaryName) -or
        [string]::IsNullOrWhiteSpace($binaryPath) -or
        [string]::IsNullOrWhiteSpace($binaryHash)) {
      Write-Error "frontend invocation lock binary entry is invalid in $lockPath"
      exit 2
    }
    if ($binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock contains duplicate binary entry '$binaryName' in $lockPath"
      exit 2
    }
    $binaryIndex[$binaryName] = $binary
  }

  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock missing binary '$binaryName' in $lockPath"
      exit 2
    }
    $binary = $binaryIndex[$binaryName]
    $expectedRelativePath = [string]$expectedBinaries[$binaryName]
    $manifestRelativePath = ([string]$binary.path).Replace('\', '/')
    if ($manifestRelativePath -ne $expectedRelativePath) {
      Write-Error "frontend invocation lock binary path mismatch for '$binaryName' in $lockPath"
      exit 2
    }
    $binaryPath = Join-Path $RepoRoot $expectedRelativePath
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      Write-Error "frontend invocation lock binary path missing at $binaryPath"
      exit 2
    }
    $binaryActualHash = Get-FileSha256Hex -Path $binaryPath
    if ($binaryActualHash -ne ([string]$binary.sha256).ToLowerInvariant()) {
      Write-Error "frontend invocation lock binary sha256 mismatch for '$binaryName' in $lockPath"
      exit 2
    }
  }
}

function Assert-FrontendCoreFeatureExpansion {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs
  )

  $featurePath = Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $featurePath -PathType Leaf)) {
    Write-Error "frontend core feature expansion artifact missing at $featurePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $featurePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend core feature expansion artifact is not valid JSON at $featurePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend core feature expansion contract id mismatch in $featurePath"
    exit 2
  }

  $expectedDependencyContracts = @(
    "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1",
    "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  )
  $presentDependencyContracts = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (![string]::IsNullOrWhiteSpace($contractIdText)) {
      $presentDependencyContracts[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencyContracts) {
    if (-not $presentDependencyContracts.ContainsKey($requiredContractId)) {
      Write-Error "frontend core feature expansion missing dependency contract '$requiredContractId' in $featurePath"
      exit 2
    }
  }

  $requiredModules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
  $presentModules = @{}
  foreach ($moduleName in @($payload.module_names)) {
    $moduleText = [string]$moduleName
    if (![string]::IsNullOrWhiteSpace($moduleText)) {
      $presentModules[$moduleText] = $true
    }
  }
  foreach ($requiredModule in $requiredModules) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Write-Error "frontend core feature expansion missing required module '$requiredModule' in $featurePath"
      exit 2
    }
  }

  $invocation = $payload.invocation
  if ($null -eq $invocation) {
    Write-Error "frontend core feature expansion invocation metadata missing in $featurePath"
    exit 2
  }
  if ([string]$invocation.default_out_dir -ne "tmp/artifacts/compilation/objc3c-native") {
    Write-Error "frontend core feature expansion default_out_dir mismatch in $featurePath"
    exit 2
  }
  if ([string]$invocation.cache_root -ne "tmp/artifacts/objc3c-native/cache") {
    Write-Error "frontend core feature expansion cache_root mismatch in $featurePath"
    exit 2
  }
  if (-not [bool]$invocation.supports_cache) {
    Write-Error "frontend core feature expansion supports_cache must be true in $featurePath"
    exit 2
  }

  $backendRouting = $payload.backend_routing
  if ($null -eq $backendRouting) {
    Write-Error "frontend core feature expansion backend_routing metadata missing in $featurePath"
    exit 2
  }
  if (-not [bool]$backendRouting.supports_capability_routing) {
    Write-Error "frontend core feature expansion supports_capability_routing must be true in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend core feature expansion capability_summary_flag mismatch in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend core feature expansion route_flag mismatch in $featurePath"
    exit 2
  }

  $allowedBackends = @{}
  foreach ($backend in @($backendRouting.allowed_ir_object_backends)) {
    $backendText = ([string]$backend).Trim()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends[$backendText.ToLowerInvariant()] = $backendText
    }
  }
  foreach ($requiredBackend in @("clang", "llvm-direct")) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Write-Error "frontend core feature expansion missing backend '$requiredBackend' in $featurePath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $requestedBackend = $null
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $requestedBackend = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $requestedBackend = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $usesCapabilityRouting = $true
      continue
    }
    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        $usesCapabilityRouting = $false
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($requestedBackend)) {
    $normalizedRequestedBackend = $requestedBackend.Trim().ToLowerInvariant().Replace("_", "-")
    if (-not $allowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Write-Error "requested --objc3-ir-object-backend '$requestedBackend' is not allowed by frontend core feature expansion in $featurePath"
      exit 2
    }
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    feature_path = $featurePath
    allowed_ir_object_backends = @($allowedBackends.Keys)
  }
}

function Assert-FrontendEdgeCompatibility {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $compatPath = Resolve-FrontendEdgeCompatibilityPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $compatPath -PathType Leaf)) {
    Write-Error "frontend edge compatibility artifact missing at $compatPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $compatPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge compatibility artifact is not valid JSON at $compatPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge compatibility contract id mismatch in $compatPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1",
    "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge compatibility missing dependency contract '$requiredContractId' in $compatPath"
      exit 2
    }
  }

  $edgeCompat = $payload.invocation_edge_compat
  if ($null -eq $edgeCompat) {
    Write-Error "frontend edge compatibility invocation_edge_compat metadata missing in $compatPath"
    exit 2
  }
  if ([int]$edgeCompat.fail_closed_exit_code -ne 2) {
    Write-Error "frontend edge compatibility fail_closed_exit_code must be 2 in $compatPath"
    exit 2
  }
  if (-not [bool]$edgeCompat.disallow_relative_parent_segments) {
    Write-Error "frontend edge compatibility disallow_relative_parent_segments must be true in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend edge compatibility route_flag mismatch in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend edge compatibility capability_summary_flag mismatch in $compatPath"
    exit 2
  }

  $backendCompat = $payload.backend_compat
  if ($null -eq $backendCompat) {
    Write-Error "frontend edge compatibility backend_compat metadata missing in $compatPath"
    exit 2
  }

  $canonicalBackends = @{}
  foreach ($backend in @($backendCompat.canonical_allowed_backends)) {
    $backendText = ([string]$backend).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $canonicalBackends[$backendText] = $true
    }
  }
  if ($canonicalBackends.Count -eq 0) {
    Write-Error "frontend edge compatibility canonical_allowed_backends must be non-empty in $compatPath"
    exit 2
  }
  if ($null -ne $CoreFeatureGuard) {
    foreach ($coreBackend in @($CoreFeatureGuard.allowed_ir_object_backends)) {
      $coreBackendText = ([string]$coreBackend).Trim().ToLowerInvariant()
      if (-not [string]::IsNullOrWhiteSpace($coreBackendText) -and
          -not $canonicalBackends.ContainsKey($coreBackendText)) {
        Write-Error "frontend edge compatibility missing backend '$coreBackendText' declared by frontend core feature expansion"
        exit 2
      }
    }
  }

  $aliasMap = @{}
  $aliasPayload = $backendCompat.alias_to_canonical
  if ($null -eq $aliasPayload) {
    Write-Error "frontend edge compatibility alias_to_canonical mapping missing in $compatPath"
    exit 2
  }
  foreach ($property in $aliasPayload.PSObject.Properties) {
    $alias = ([string]$property.Name).Trim().ToLowerInvariant().Replace("_", "-")
    $canonical = ([string]$property.Value).Trim().ToLowerInvariant().Replace("_", "-")
    if ([string]::IsNullOrWhiteSpace($alias) -or [string]::IsNullOrWhiteSpace($canonical)) {
      Write-Error "frontend edge compatibility alias_to_canonical entries must be non-empty in $compatPath"
      exit 2
    }
    if (-not $canonicalBackends.ContainsKey($canonical)) {
      Write-Error "frontend edge compatibility alias '$alias' maps to unknown canonical backend '$canonical' in $compatPath"
      exit 2
    }
    $aliasMap[$alias] = $canonical
  }
  foreach ($canonicalBackend in $canonicalBackends.Keys) {
    if (-not $aliasMap.ContainsKey($canonicalBackend)) {
      $aliasMap[$canonicalBackend] = $canonicalBackend
    }
  }

  $singleValueFlags = @{}
  foreach ($flag in @($backendCompat.single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $singleValueFlags[$flagText] = 0
    }
  }
  foreach ($requiredSingleValueFlag in @("--objc3-ir-object-backend", "--llvm-capabilities-summary")) {
    if (-not $singleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Write-Error "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $compatPath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $normalizedArgs = New-Object System.Collections.Generic.List[string]
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false
  $routeFlagOccurrences = 0

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $emitPrefix = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--emit-prefix") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --emit-prefix"
        exit 2
      }
      $i++
      $emitPrefix = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add("--emit-prefix")
      $normalizedArgs.Add($emitPrefix)
      continue
    }

    if ($token.StartsWith("--clang=", [System.StringComparison]::Ordinal)) {
      $clangPath = $token.Substring("--clang=".Length)
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--clang") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --clang"
        exit 2
      }
      $i++
      $clangPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
      }
      $normalizedArgs.Add("--clang")
      $normalizedArgs.Add($clangPath)
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeFlagOccurrences++
      $usesCapabilityRouting = $true
      $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeFlagOccurrences++
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }

    $normalizedArgs.Add($token)
  }

  foreach ($flag in $singleValueFlags.Keys) {
    if ([int]$singleValueFlags[$flag] -gt 1) {
      Write-Error "$flag can be provided at most once"
      exit 2
    }
  }
  if ($routeFlagOccurrences -gt 1) {
    Write-Error "--objc3-route-backend-from-capabilities can be provided at most once"
    exit 2
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    edge_compat_path = $compatPath
    normalized_compile_args = $normalizedArgs.ToArray()
  }
}

function Assert-FrontendEdgeRobustness {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $robustnessPath = Resolve-FrontendEdgeRobustnessPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $robustnessPath -PathType Leaf)) {
    Write-Error "frontend edge robustness artifact missing at $robustnessPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $robustnessPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge robustness artifact is not valid JSON at $robustnessPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge robustness contract id mismatch in $robustnessPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1",
    "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge robustness missing dependency contract '$requiredContractId' in $robustnessPath"
      exit 2
    }
  }

  $guardrails = $payload.wrapper_guardrails
  if ($null -eq $guardrails) {
    Write-Error "frontend edge robustness wrapper_guardrails metadata missing in $robustnessPath"
    exit 2
  }

  $requiredWrapperSingleFlags = @("--use-cache", "--out-dir")
  $wrapperSingleSet = @{}
  foreach ($flag in @($guardrails.wrapper_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $wrapperSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredWrapperSingleFlags) {
    if (-not $wrapperSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing wrapper_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredCompileSingleFlags = @(
    "--objc3-ir-object-backend",
    "--llvm-capabilities-summary",
    "--objc3-route-backend-from-capabilities"
  )
  $compileSingleSet = @{}
  foreach ($flag in @($guardrails.compile_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $compileSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredCompileSingleFlags) {
    if (-not $compileSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing compile_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredRejectEmptyFlags = @("--emit-prefix", "--clang", "--use-cache")
  $rejectEmptySet = @{}
  foreach ($flag in @($guardrails.reject_empty_equals_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $rejectEmptySet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredRejectEmptyFlags) {
    if (-not $rejectEmptySet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing reject_empty_equals_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    edge_robustness_path = $robustnessPath
  }
}

function Assert-FrontendDiagnosticsHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $diagnosticsPath = Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $diagnosticsPath -PathType Leaf)) {
    Write-Error "frontend diagnostics hardening artifact missing at $diagnosticsPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $diagnosticsPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend diagnostics hardening artifact is not valid JSON at $diagnosticsPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend diagnostics hardening contract id mismatch in $diagnosticsPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend diagnostics hardening missing dependency contract '$requiredContractId' in $diagnosticsPath"
      exit 2
    }
  }

  $wrapperDiagnostics = $payload.wrapper_diagnostics
  if ($null -eq $wrapperDiagnostics) {
    Write-Error "frontend diagnostics hardening wrapper_diagnostics metadata missing in $diagnosticsPath"
    exit 2
  }
  if ([int]$wrapperDiagnostics.fail_closed_exit_code -ne 2) {
    Write-Error "frontend diagnostics hardening fail_closed_exit_code must be 2 in $diagnosticsPath"
    exit 2
  }

  $requiredMessages = @(
    "--use-cache can be provided at most once",
    "invalid --use-cache value",
    "--out-dir can be provided at most once",
    "missing value for --out-dir",
    "empty value for --out-dir",
    "missing value for --emit-prefix",
    "empty value for --emit-prefix",
    "missing value for --clang",
    "empty value for --clang"
  )
  $messageSet = @{}
  foreach ($message in @($wrapperDiagnostics.required_error_messages)) {
    $messageText = [string]$message
    if (-not [string]::IsNullOrWhiteSpace($messageText)) {
      $messageSet[$messageText] = $true
    }
  }
  foreach ($requiredMessage in $requiredMessages) {
    if (-not $messageSet.ContainsKey($requiredMessage)) {
      Write-Error "frontend diagnostics hardening missing required_error_messages entry '$requiredMessage' in $diagnosticsPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    diagnostics_hardening_path = $diagnosticsPath
  }
}

function Assert-FrontendRecoveryDeterminismHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $recoveryPath = Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $recoveryPath -PathType Leaf)) {
    Write-Error "frontend recovery determinism hardening artifact missing at $recoveryPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $recoveryPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend recovery determinism hardening artifact is not valid JSON at $recoveryPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend recovery determinism hardening contract id mismatch in $recoveryPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1",
    "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend recovery determinism hardening missing dependency contract '$requiredContractId' in $recoveryPath"
      exit 2
    }
  }

  $cacheDeterminism = $payload.cache_determinism
  if ($null -eq $cacheDeterminism) {
    Write-Error "frontend recovery determinism hardening cache_determinism metadata missing in $recoveryPath"
    exit 2
  }
  if ([int]$cacheDeterminism.fail_closed_exit_code -ne 2) {
    Write-Error "frontend recovery determinism hardening fail_closed_exit_code must be 2 in $recoveryPath"
    exit 2
  }
  if ([string]$cacheDeterminism.entry_contract_id -ne "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1") {
    Write-Error "frontend recovery determinism hardening entry_contract_id mismatch in $recoveryPath"
    exit 2
  }

  $requiredStatusTokens = @("cache_hit=true", "cache_hit=false")
  $statusTokenSet = @{}
  foreach ($token in @($cacheDeterminism.cache_status_tokens)) {
    $tokenText = [string]$token
    if (-not [string]::IsNullOrWhiteSpace($tokenText)) {
      $statusTokenSet[$tokenText] = $true
    }
  }
  foreach ($requiredToken in $requiredStatusTokens) {
    if (-not $statusTokenSet.ContainsKey($requiredToken)) {
      Write-Error "frontend recovery determinism hardening missing cache_status_tokens entry '$requiredToken' in $recoveryPath"
      exit 2
    }
  }

  $requiredEntryFiles = @("files", "exit_code.txt", "ready.marker", "metadata.json")
  $entryFileSet = @{}
  foreach ($entryFile in @($cacheDeterminism.required_entry_files)) {
    $entryFileText = [string]$entryFile
    if (-not [string]::IsNullOrWhiteSpace($entryFileText)) {
      $entryFileSet[$entryFileText] = $true
    }
  }
  foreach ($requiredEntryFile in $requiredEntryFiles) {
    if (-not $entryFileSet.ContainsKey($requiredEntryFile)) {
      Write-Error "frontend recovery determinism hardening missing required_entry_files entry '$requiredEntryFile' in $recoveryPath"
      exit 2
    }
  }

  $requiredRecoverySignals = @(
    "cache_recovery=metadata_missing",
    "cache_recovery=metadata_invalid",
    "cache_recovery=metadata_contract_mismatch",
    "cache_recovery=metadata_cache_key_mismatch",
    "cache_recovery=metadata_exit_code_mismatch",
    "cache_recovery=metadata_digest_mismatch",
    "cache_recovery=restore_failed"
  )
  $recoverySignalSet = @{}
  foreach ($signal in @($cacheDeterminism.recovery_signals)) {
    $signalText = [string]$signal
    if (-not [string]::IsNullOrWhiteSpace($signalText)) {
      $recoverySignalSet[$signalText] = $true
    }
  }
  foreach ($requiredSignal in $requiredRecoverySignals) {
    if (-not $recoverySignalSet.ContainsKey($requiredSignal)) {
      Write-Error "frontend recovery determinism hardening missing recovery_signals entry '$requiredSignal' in $recoveryPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    recovery_determinism_hardening_path = $recoveryPath
  }
}

function Assert-FrontendConformanceMatrix {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  $conformancePath = Resolve-FrontendConformanceMatrixPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $conformancePath -PathType Leaf)) {
    Write-Error "frontend conformance matrix artifact missing at $conformancePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $conformancePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend conformance matrix artifact is not valid JSON at $conformancePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend conformance matrix contract id mismatch in $conformancePath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend conformance matrix missing dependency contract '$requiredContractId' in $conformancePath"
      exit 2
    }
  }

  if ([int]$payload.acceptance_profile_count -le 0) {
    Write-Error "frontend conformance matrix acceptance_profile_count must be positive in $conformancePath"
    exit 2
  }
  if ([int]$payload.rejection_profile_count -le 0) {
    Write-Error "frontend conformance matrix rejection_profile_count must be positive in $conformancePath"
    exit 2
  }

  $acceptanceRows = @($payload.acceptance_matrix)
  $acceptanceProfileSet = @{}
  foreach ($row in $acceptanceRows) {
    $caseId = [string]$row.case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Write-Error "frontend conformance matrix acceptance rows must define case_id and profile_key in $conformancePath"
      exit 2
    }
    if ($expectedResult -ne "accept") {
      Write-Error "frontend conformance matrix acceptance row '$caseId' must declare expected_result='accept' in $conformancePath"
      exit 2
    }
    if ($acceptanceProfileSet.ContainsKey($profileKey)) {
      Write-Error "frontend conformance matrix duplicate acceptance profile '$profileKey' in $conformancePath"
      exit 2
    }
    $acceptanceProfileSet[$profileKey] = $caseId
  }
  if ([int]$payload.acceptance_profile_count -ne $acceptanceRows.Count) {
    Write-Error "frontend conformance matrix acceptance_profile_count mismatch in $conformancePath"
    exit 2
  }

  $expectedProfileSet = @{}
  foreach ($cacheMode in @("no-cache", "cache-aware")) {
    foreach ($backendMode in @("default", "clang", "llvm-direct")) {
      foreach ($summaryMode in @("none", "present")) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $expectedProfileSet[$profileKey] = $true
      }
      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $expectedProfileSet[$profileKey] = $true
    }
  }
  foreach ($expectedProfile in $expectedProfileSet.Keys) {
    if (-not $acceptanceProfileSet.ContainsKey($expectedProfile)) {
      Write-Error "frontend conformance matrix missing acceptance profile '$expectedProfile' in $conformancePath"
      exit 2
    }
  }

  $requiredRejectDiagnostics = @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
  $rejectRows = @($payload.rejection_matrix)
  if ([int]$payload.rejection_profile_count -ne $rejectRows.Count) {
    Write-Error "frontend conformance matrix rejection_profile_count mismatch in $conformancePath"
    exit 2
  }
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectRows) {
    $caseId = [string]$row.case_id
    $expectedResult = [string]$row.expected_result
    $requiredDiagnostic = [string]$row.required_diagnostic
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($requiredDiagnostic)) {
      Write-Error "frontend conformance matrix rejection rows must define case_id and required_diagnostic in $conformancePath"
      exit 2
    }
    if ($expectedResult -ne "reject") {
      Write-Error "frontend conformance matrix rejection row '$caseId' must declare expected_result='reject' in $conformancePath"
      exit 2
    }
    $rejectDiagnosticSet[$requiredDiagnostic] = $true
  }
  foreach ($requiredDiagnostic in $requiredRejectDiagnostics) {
    if (-not $rejectDiagnosticSet.ContainsKey($requiredDiagnostic)) {
      Write-Error "frontend conformance matrix missing rejection diagnostic '$requiredDiagnostic' in $conformancePath"
      exit 2
    }
  }

  $compileArgs = @($EffectiveCompileArgs)
  $cacheMode = if ($null -ne $ParsedArgs -and [bool]$ParsedArgs.use_cache) { "cache-aware" } else { "no-cache" }
  $backendMode = "default"
  $routingMode = "manual"
  $summaryMode = "none"
  $routeEnabled = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (@("clang", "llvm-direct") -notcontains $backendKey) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $backendMode = $backendKey
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (@("clang", "llvm-direct") -notcontains $backendKey) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $backendMode = $backendKey
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $summaryMode = "present"
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryValue = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $summaryMode = "present"
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeEnabled = $true
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $routeEnabled = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
  }

  if ($routeEnabled) {
    $routingMode = "capability-route"
  }
  if ($routingMode -eq "capability-route" -and $summaryMode -ne "present") {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  $invocationProfileKey = "{0}|{1}|{2}|{3}" -f $cacheMode, $backendMode, $routingMode, $summaryMode
  if (-not $acceptanceProfileSet.ContainsKey($invocationProfileKey)) {
    Write-Error "frontend conformance matrix has no acceptance row for invocation profile '$invocationProfileKey' in $conformancePath"
    exit 2
  }

  return [pscustomobject]@{
    conformance_matrix_path = $conformancePath
    profile_key = $invocationProfileKey
    case_id = [string]$acceptanceProfileSet[$invocationProfileKey]
  }
}

function Assert-FrontendConformanceCorpus {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [string]$InvocationProfileKey
  )

  $corpusPath = Resolve-FrontendConformanceCorpusPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $corpusPath -PathType Leaf)) {
    Write-Error "frontend conformance corpus artifact missing at $corpusPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $corpusPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend conformance corpus artifact is not valid JSON at $corpusPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend conformance corpus contract id mismatch in $corpusPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend conformance corpus missing dependency contract '$requiredContractId' in $corpusPath"
      exit 2
    }
  }

  $acceptanceRows = @($payload.acceptance_corpus)
  $rejectionRows = @($payload.rejection_corpus)
  if ([int]$payload.acceptance_corpus_count -ne $acceptanceRows.Count) {
    Write-Error "frontend conformance corpus acceptance_corpus_count mismatch in $corpusPath"
    exit 2
  }
  if ([int]$payload.rejection_corpus_count -ne $rejectionRows.Count) {
    Write-Error "frontend conformance corpus rejection_corpus_count mismatch in $corpusPath"
    exit 2
  }
  if ([int]$payload.corpus_case_count -ne ($acceptanceRows.Count + $rejectionRows.Count)) {
    Write-Error "frontend conformance corpus corpus_case_count mismatch in $corpusPath"
    exit 2
  }
  if ($acceptanceRows.Count -le 0 -or $rejectionRows.Count -le 0) {
    Write-Error "frontend conformance corpus requires non-empty acceptance and rejection corpus in $corpusPath"
    exit 2
  }

  $acceptanceByProfile = @{}
  foreach ($row in $acceptanceRows) {
    $caseId = [string]$row.corpus_case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Write-Error "frontend conformance corpus acceptance rows must define corpus_case_id and profile_key in $corpusPath"
      exit 2
    }
    if ($expectedResult -ne "accept") {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must declare expected_result='accept' in $corpusPath"
      exit 2
    }
    if ($expectedExitCode -ne 0) {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must declare expected_exit_code=0 in $corpusPath"
      exit 2
    }
    if ($compileArgs.Count -le 0) {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must provide compile_args in $corpusPath"
      exit 2
    }
    if (-not $acceptanceByProfile.ContainsKey($profileKey)) {
      $acceptanceByProfile[$profileKey] = New-Object System.Collections.Generic.List[string]
    }
    $acceptanceByProfile[$profileKey].Add($caseId)
  }

  $requiredRejectDiagnostics = @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectionRows) {
    $caseId = [string]$row.corpus_case_id
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $expectedDiagnostic = [string]$row.expected_diagnostic
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($expectedDiagnostic)) {
      Write-Error "frontend conformance corpus rejection rows must define corpus_case_id and expected_diagnostic in $corpusPath"
      exit 2
    }
    if ($expectedResult -ne "reject") {
      Write-Error "frontend conformance corpus rejection row '$caseId' must declare expected_result='reject' in $corpusPath"
      exit 2
    }
    if ($expectedExitCode -ne 2) {
      Write-Error "frontend conformance corpus rejection row '$caseId' must declare expected_exit_code=2 in $corpusPath"
      exit 2
    }
    if ($compileArgs.Count -le 0) {
      Write-Error "frontend conformance corpus rejection row '$caseId' must provide compile_args in $corpusPath"
      exit 2
    }
    $rejectDiagnosticSet[$expectedDiagnostic] = $true
  }
  foreach ($requiredDiagnostic in $requiredRejectDiagnostics) {
    if (-not $rejectDiagnosticSet.ContainsKey($requiredDiagnostic)) {
      Write-Error "frontend conformance corpus missing rejection diagnostic '$requiredDiagnostic' in $corpusPath"
      exit 2
    }
  }

  if ([string]::IsNullOrWhiteSpace($InvocationProfileKey)) {
    Write-Error "frontend conformance corpus invocation profile key is required"
    exit 2
  }
  if (-not $acceptanceByProfile.ContainsKey($InvocationProfileKey)) {
    Write-Error "frontend conformance corpus has no acceptance case for invocation profile '$InvocationProfileKey' in $corpusPath"
    exit 2
  }

  return [pscustomobject]@{
    conformance_corpus_path = $corpusPath
    profile_key = $InvocationProfileKey
    acceptance_case_count = [int]$acceptanceByProfile[$InvocationProfileKey].Count
  }
}

function Assert-FrontendIntegrationCloseout {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $closeoutPath = Resolve-FrontendIntegrationCloseoutPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $closeoutPath -PathType Leaf)) {
    Write-Error "frontend integration closeout artifact missing at $closeoutPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $closeoutPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend integration closeout artifact is not valid JSON at $closeoutPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend integration closeout contract id mismatch in $closeoutPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1",
    "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1",
    "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend integration closeout missing dependency contract '$requiredContractId' in $closeoutPath"
      exit 2
    }
  }

  $closeoutGate = $payload.closeout_gate
  if ($null -eq $closeoutGate) {
    Write-Error "frontend integration closeout closeout_gate metadata missing in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.build_integration_gate_signoff) {
    Write-Error "frontend integration closeout build_integration_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.invocation_profile_gate_signoff) {
    Write-Error "frontend integration closeout invocation_profile_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.corpus_coverage_gate_signoff) {
    Write-Error "frontend integration closeout corpus_coverage_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if ([int]$closeoutGate.deterministic_fail_closed_exit_code -ne 2) {
    Write-Error "frontend integration closeout deterministic_fail_closed_exit_code must be 2 in $closeoutPath"
    exit 2
  }
  if ([int]$closeoutGate.acceptance_corpus_count -le 0 -or [int]$closeoutGate.rejection_corpus_count -le 0) {
    Write-Error "frontend integration closeout acceptance/rejection corpus counts must be positive in $closeoutPath"
    exit 2
  }

  return [pscustomobject]@{
    integration_closeout_path = $closeoutPath
  }
}

$parsed = Parse-Objc3cNativeCompileArguments -RawArgs $args -DefaultOutDir $defaultOutDir
$exe = Resolve-NativeCompilerExecutablePath -RepoRoot $repoRoot
$buildResult = $null
$buildResult = Ensure-NativeCompilerAvailable -RepoRoot $repoRoot -BuildResult $buildResult

Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
Assert-FrontendInvocationLock -RepoRoot $repoRoot -BuildResult $buildResult
$coreFeatureGuard = Assert-FrontendCoreFeatureExpansion -RepoRoot $repoRoot -BuildResult $buildResult -ParsedArgs $parsed
$edgeCompatGuard = Assert-FrontendEdgeCompatibility `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -ParsedArgs $parsed `
  -CoreFeatureGuard $coreFeatureGuard
Assert-FrontendEdgeRobustness -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
Assert-FrontendDiagnosticsHardening -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
Assert-FrontendRecoveryDeterminismHardening -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
$effectiveCompileArgs = @($parsed.compile_args)
if ($null -ne $edgeCompatGuard -and $null -ne $edgeCompatGuard.normalized_compile_args) {
  $effectiveCompileArgs = @($edgeCompatGuard.normalized_compile_args)
}
$matrixGuard = Assert-FrontendConformanceMatrix `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -ParsedArgs $parsed `
  -EffectiveCompileArgs $effectiveCompileArgs
Assert-FrontendConformanceCorpus `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -InvocationProfileKey ([string]$matrixGuard.profile_key) | Out-Null
Assert-FrontendIntegrationCloseout -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null

$argsWithoutOutDir = @(Get-Objc3cNativeCompileArgsWithoutOutDir -CompileArgs $effectiveCompileArgs)
$inputPath = Get-Objc3cNativeCompileInputPath -ArgsWithoutOutDir $argsWithoutOutDir
$cacheContext = New-Objc3cNativeCompileCacheContext `
  -RepoRoot $repoRoot `
  -InputPath $inputPath `
  -ArgsWithoutOutDir $argsWithoutOutDir `
  -WrapperScriptPath $PSCommandPath

if ($parsed.use_cache -and $null -ne $cacheContext.cache_key) {
  $cacheRestore = Restore-Objc3cNativeCompileCacheEntry `
    -CacheContext $cacheContext `
    -DestinationRoot $parsed.out_dir
  if ($cacheRestore.restored) {
    Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix
    Write-CompileOutputProvenance `
      -RepoRoot $repoRoot `
      -CompileDir $parsed.out_dir `
      -EmitPrefix $parsed.emit_prefix `
      -InputPath $inputPath `
      -CompilerBinaryPath $exe `
      -RuntimeLibraryPath (Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib") `
      -WrapperScriptPath $PSCommandPath
    Write-Output "cache_hit=true"
    exit ([int]$cacheRestore.exit_code)
  }
}

$compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments $effectiveCompileArgs

if ($compileExit -eq 0) {
  Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix
  Write-CompileOutputProvenance `
    -RepoRoot $repoRoot `
    -CompileDir $parsed.out_dir `
    -EmitPrefix $parsed.emit_prefix `
    -InputPath $inputPath `
    -CompilerBinaryPath $exe `
    -RuntimeLibraryPath (Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib") `
    -WrapperScriptPath $PSCommandPath
}

if ($parsed.use_cache -and $null -ne $cacheContext.cache_key) {
  Save-Objc3cNativeCompileCacheEntry `
    -CacheContext $cacheContext `
    -SourceRoot $parsed.out_dir `
    -CompileExit $compileExit
}

Write-Output "cache_hit=false"
exit $compileExit
