$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$llvmRoot = if ($env:LLVM_ROOT) { $env:LLVM_ROOT } else { "C:\Program Files\LLVM" }
$clangxx = Join-Path $llvmRoot "bin\clang++.exe"

if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
  $clangCommand = Get-Command clang++ -ErrorAction SilentlyContinue
  if ($null -ne $clangCommand -and (Test-Path -LiteralPath $clangCommand.Source -PathType Leaf)) {
    $clangxx = $clangCommand.Source
    $clangBinDir = Split-Path -Parent $clangxx
    $llvmRoot = Split-Path -Parent $clangBinDir
  }
}

$libclangCandidates = @(
  (Join-Path $llvmRoot "lib\libclang.lib"),
  (Join-Path $llvmRoot "lib\clang.lib")
)
$libclang = $null
foreach ($candidate in $libclangCandidates) {
  if (Test-Path -LiteralPath $candidate -PathType Leaf) {
    $libclang = $candidate
    break
  }
}

$includeDir = Join-Path $llvmRoot "include"
$nativeSourceRoot = Join-Path $repoRoot "native/objc3c/src"

if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
  throw ("clang++ not found. set LLVM_ROOT or ensure clang++ is on PATH (attempted: " + $clangxx + ")")
}
if ($null -eq $libclang) {
  $attempted = [string]::Join(", ", $libclangCandidates)
  throw ("LLVM import library not found. set LLVM_ROOT to a full LLVM install (attempted: " + $attempted + ")")
}
if (!(Test-Path -LiteralPath $includeDir -PathType Container)) { throw "LLVM include dir not found at $includeDir" }
if (!(Test-Path -LiteralPath $nativeSourceRoot -PathType Container)) { throw "native source root not found at $nativeSourceRoot" }

$outDir = Join-Path $repoRoot "artifacts/bin"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outExe = Join-Path $outDir "objc3c-native.exe"
$outCapiExe = Join-Path $outDir "objc3c-frontend-c-api-runner.exe"
$tmpOutDir = Join-Path $repoRoot "tmp/build-objc3c-native"
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null
$runSuffix = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
$stagedOutExe = Join-Path $tmpOutDir ("objc3c-native.{0}.exe" -f $runSuffix)
$stagedOutCapiExe = Join-Path $tmpOutDir ("objc3c-frontend-c-api-runner.{0}.exe" -f $runSuffix)

function Publish-ArtifactWithRetry {
  param(
    [Parameter(Mandatory = $true)]
    [string]$StagedPath,
    [Parameter(Mandatory = $true)]
    [string]$FinalPath,
    [int]$MaxAttempts = 40,
    [int]$SleepMilliseconds = 250
  )

  for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
    try {
      Move-Item -LiteralPath $StagedPath -Destination $FinalPath -Force
      return
    } catch {
      if ($attempt -eq $MaxAttempts) {
        throw "failed to publish $FinalPath after $MaxAttempts attempt(s): $($_.Exception.Message)"
      }
      Start-Sleep -Milliseconds $SleepMilliseconds
    }
  }
}

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RootPath,
    [Parameter(Mandatory = $true)]
    [string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path.TrimEnd('\', '/')
  $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  $rootUri = [System.Uri]::new(($resolvedRoot + '\'))
  $targetUri = [System.Uri]::new($resolvedTarget)
  $relative = [System.Uri]::UnescapeDataString($rootUri.MakeRelativeUri($targetUri).ToString())
  return $relative.Replace('\', '/')
}

function Get-FrontendSharedSourcesFromModules {
  param([object[]]$Modules)

  $seen = @{}
  $flattened = New-Object System.Collections.Generic.List[string]
  foreach ($module in $Modules) {
    $name = [string]$module.name
    $sources = @($module.sources)
    if ([string]::IsNullOrWhiteSpace($name)) {
      throw "frontend module entry missing name"
    }
    if ($sources.Count -eq 0) {
      throw "frontend module '$name' must declare at least one source"
    }
    foreach ($source in $sources) {
      if ($seen.ContainsKey($source)) {
        throw "duplicate frontend shared source entry: $source"
      }
      $seen[$source] = $true
      $flattened.Add($source)
    }
  }
  return $flattened.ToArray()
}

function Write-FrontendModuleScaffoldArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string[]]$BinaryTargets
  )

  $modulePayload = New-Object System.Collections.Generic.List[object]
  foreach ($module in $Modules) {
    $name = [string]$module.name
    $sources = @($module.sources)
    if ([string]::IsNullOrWhiteSpace($name) -or $sources.Count -eq 0) {
      throw "frontend scaffold module metadata is invalid"
    }
    $modulePayload.Add([ordered]@{
      name = $name
      source_count = $sources.Count
      sources = $sources
    })
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
    schema_version = 1
    module_count = $modulePayload.Count
    shared_source_count = $SharedSources.Count
    modules = $modulePayload.ToArray()
    shared_sources = $SharedSources
    binary_targets = $BinaryTargets
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

function Get-FileSha256Hex {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Path
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "cannot hash missing file: $Path"
  }

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

function Write-FrontendInvocationLockArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath
  )

  if (!(Test-Path -LiteralPath $NativeBinaryPath -PathType Leaf)) {
    throw "native binary missing for invocation lock artifact: $NativeBinaryPath"
  }
  if (!(Test-Path -LiteralPath $CapiBinaryPath -PathType Leaf)) {
    throw "c-api runner missing for invocation lock artifact: $CapiBinaryPath"
  }
  if (!(Test-Path -LiteralPath $FrontendScaffoldPath -PathType Leaf)) {
    throw "frontend scaffold missing for invocation lock artifact: $FrontendScaffoldPath"
  }

  try {
    $scaffoldPayload = Get-Content -LiteralPath $FrontendScaffoldPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend scaffold is not valid JSON for invocation lock artifact: $FrontendScaffoldPath"
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
  if ([string]$scaffoldPayload.contract_id -ne $expectedScaffoldContractId) {
    throw "frontend scaffold contract id mismatch for invocation lock artifact: $FrontendScaffoldPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
    schema_version = 1
    scaffold_contract_id = [string]$scaffoldPayload.contract_id
    scaffold = [ordered]@{
      path = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $FrontendScaffoldPath
      sha256 = Get-FileSha256Hex -Path $FrontendScaffoldPath
    }
    binaries = @(
      [ordered]@{
        name = "objc3c-native"
        path = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath
        sha256 = Get-FileSha256Hex -Path $NativeBinaryPath
      },
      [ordered]@{
        name = "objc3c-frontend-c-api-runner"
        path = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath
        sha256 = Get-FileSha256Hex -Path $CapiBinaryPath
      }
    )
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

function Write-FrontendCoreFeatureExpansionArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendInvocationLockPath
  )

  if (!(Test-Path -LiteralPath $FrontendScaffoldPath -PathType Leaf)) {
    throw "frontend scaffold missing for core feature expansion artifact: $FrontendScaffoldPath"
  }
  if (!(Test-Path -LiteralPath $FrontendInvocationLockPath -PathType Leaf)) {
    throw "frontend invocation lock missing for core feature expansion artifact: $FrontendInvocationLockPath"
  }
  if (!(Test-Path -LiteralPath $NativeBinaryPath -PathType Leaf)) {
    throw "native binary missing for core feature expansion artifact: $NativeBinaryPath"
  }
  if (!(Test-Path -LiteralPath $CapiBinaryPath -PathType Leaf)) {
    throw "c-api runner missing for core feature expansion artifact: $CapiBinaryPath"
  }

  try {
    $scaffoldPayload = Get-Content -LiteralPath $FrontendScaffoldPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend scaffold is not valid JSON for core feature expansion artifact: $FrontendScaffoldPath"
  }

  try {
    $invocationLockPayload = Get-Content -LiteralPath $FrontendInvocationLockPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend invocation lock is not valid JSON for core feature expansion artifact: $FrontendInvocationLockPath"
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1"
  if ([string]$scaffoldPayload.contract_id -ne $expectedScaffoldContractId) {
    throw "frontend scaffold contract id mismatch for core feature expansion artifact: $FrontendScaffoldPath"
  }
  $expectedInvocationLockContractId = "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
  if ([string]$invocationLockPayload.contract_id -ne $expectedInvocationLockContractId) {
    throw "frontend invocation lock contract id mismatch for core feature expansion artifact: $FrontendInvocationLockPath"
  }

  $moduleNames = @($Modules | ForEach-Object { [string]$_.name })
  foreach ($moduleName in $moduleNames) {
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      throw "frontend module metadata contains empty module name for core feature expansion artifact"
    }
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedScaffoldContractId
      $expectedInvocationLockContractId
    )
    module_names = $moduleNames
    shared_source_count = $SharedSources.Count
    binaries = @(
      [ordered]@{
        name = "objc3c-native"
        path = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath
      },
      [ordered]@{
        name = "objc3c-frontend-c-api-runner"
        path = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath
      }
    )
    invocation = [ordered]@{
      default_out_dir = "tmp/artifacts/compilation/objc3c-native"
      cache_root = "tmp/artifacts/objc3c-native/cache"
      supports_cache = $true
    }
    backend_routing = [ordered]@{
      allowed_ir_object_backends = @("clang", "llvm-direct")
      supports_capability_routing = $true
      capability_summary_flag = "--llvm-capabilities-summary"
      route_flag = "--objc3-route-backend-from-capabilities"
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

function Write-FrontendEdgeCompatibilityArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendCoreFeatureExpansionPath
  )

  if (!(Test-Path -LiteralPath $FrontendCoreFeatureExpansionPath -PathType Leaf)) {
    throw "frontend core feature expansion missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  try {
    $coreFeaturePayload = Get-Content -LiteralPath $FrontendCoreFeatureExpansionPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend core feature expansion is not valid JSON for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $expectedCoreFeatureContractId = "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"
  if ([string]$coreFeaturePayload.contract_id -ne $expectedCoreFeatureContractId) {
    throw "frontend core feature contract id mismatch for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $allowedBackends = @()
  foreach ($backend in @($coreFeaturePayload.backend_routing.allowed_ir_object_backends)) {
    $backendText = [string]$backend
    if (![string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends += $backendText
    }
  }
  if ($allowedBackends.Count -eq 0) {
    throw "frontend core feature expansion allowed_ir_object_backends missing for edge compatibility artifact: $FrontendCoreFeatureExpansionPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCoreFeatureContractId
      "objc3c-frontend-build-invocation-manifest-guard/m226-d003-v1"
    )
    backend_compat = [ordered]@{
      canonical_allowed_backends = $allowedBackends
      alias_to_canonical = [ordered]@{
        "clang" = "clang"
        "clang++" = "clang"
        "clang-cl" = "clang"
        "llvm-direct" = "llvm-direct"
        "llvm_direct" = "llvm-direct"
        "llvmdirect" = "llvm-direct"
        "llvm" = "llvm-direct"
      }
      single_value_flags = @(
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
      )
    }
    invocation_edge_compat = [ordered]@{
      supports_equals_form_flags = @(
        "--out-dir"
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
      )
      supports_boolean_equals_flags = @(
        "--use-cache"
        "--objc3-route-backend-from-capabilities"
      )
      route_flag = "--objc3-route-backend-from-capabilities"
      capability_summary_flag = "--llvm-capabilities-summary"
      fail_closed_exit_code = 2
      disallow_relative_parent_segments = $true
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

function Write-FrontendEdgeRobustnessArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeCompatibilityPath
  )

  if (!(Test-Path -LiteralPath $FrontendEdgeCompatibilityPath -PathType Leaf)) {
    throw "frontend edge compatibility artifact missing for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  try {
    $edgeCompatPayload = Get-Content -LiteralPath $FrontendEdgeCompatibilityPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend edge compatibility artifact is not valid JSON for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $expectedEdgeCompatContractId = "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"
  if ([string]$edgeCompatPayload.contract_id -ne $expectedEdgeCompatContractId) {
    throw "frontend edge compatibility contract id mismatch for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeCompatContractId
      "objc3c-frontend-build-invocation-core-feature-expansion/m226-d004-v1"
    )
    wrapper_guardrails = [ordered]@{
      wrapper_single_value_flags = @(
        "--use-cache"
        "--out-dir"
      )
      compile_single_value_flags = @(
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
        "--objc3-route-backend-from-capabilities"
      )
      reject_empty_equals_value_flags = @(
        "--out-dir"
        "--emit-prefix"
        "--clang"
        "--objc3-ir-object-backend"
        "--llvm-capabilities-summary"
        "--objc3-route-backend-from-capabilities"
        "--use-cache"
      )
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

function Write-FrontendDiagnosticsHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendEdgeRobustnessPath
  )

  if (!(Test-Path -LiteralPath $FrontendEdgeRobustnessPath -PathType Leaf)) {
    throw "frontend edge robustness artifact missing for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  try {
    $edgeRobustnessPayload = Get-Content -LiteralPath $FrontendEdgeRobustnessPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend edge robustness artifact is not valid JSON for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $expectedEdgeRobustnessContractId = "objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1"
  if ([string]$edgeRobustnessPayload.contract_id -ne $expectedEdgeRobustnessContractId) {
    throw "frontend edge robustness contract id mismatch for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeRobustnessContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"
    )
    wrapper_diagnostics = [ordered]@{
      fail_closed_exit_code = 2
      required_error_messages = @(
        "--use-cache can be provided at most once"
        "invalid --use-cache value"
        "--out-dir can be provided at most once"
        "missing value for --out-dir"
        "empty value for --out-dir"
        "missing value for --emit-prefix"
        "empty value for --emit-prefix"
        "missing value for --clang"
        "empty value for --clang"
      )
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

$frontendModules = @(
  [ordered]@{
    name = "driver"
    sources = @(
      "native/objc3c/src/driver/objc3_cli_options.cpp"
      "native/objc3c/src/driver/objc3_driver_main.cpp"
      "native/objc3c/src/driver/objc3_driver_shell.cpp"
      "native/objc3c/src/driver/objc3_frontend_options.cpp"
      "native/objc3c/src/driver/objc3_llvm_capability_routing.cpp"
      "native/objc3c/src/driver/objc3_objc3_path.cpp"
      "native/objc3c/src/driver/objc3_objectivec_path.cpp"
      "native/objc3c/src/driver/objc3_compilation_driver.cpp"
    )
  }
  [ordered]@{
    name = "diagnostics-io"
    sources = @(
      "native/objc3c/src/diag/objc3_diag_utils.cpp"
      "native/objc3c/src/io/objc3_diagnostics_artifacts.cpp"
      "native/objc3c/src/io/objc3_file_io.cpp"
      "native/objc3c/src/io/objc3_manifest_artifacts.cpp"
      "native/objc3c/src/io/objc3_process.cpp"
    )
  }
  [ordered]@{
    name = "ir"
    sources = @(
      "native/objc3c/src/ir/objc3_ir_emitter.cpp"
    )
  }
  [ordered]@{
    name = "lex-parse"
    sources = @(
      "native/objc3c/src/lex/objc3_lexer.cpp"
      "native/objc3c/src/parse/objc3_ast_builder.cpp"
      "native/objc3c/src/parse/objc3_ast_builder_contract.cpp"
      "native/objc3c/src/parse/objc3_parse_support.cpp"
      "native/objc3c/src/parse/objc3_parser.cpp"
    )
  }
  [ordered]@{
    name = "frontend-api"
    sources = @(
      "native/objc3c/src/libobjc3c_frontend/c_api.cpp"
      "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp"
      "native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp"
    )
  }
  [ordered]@{
    name = "lowering"
    sources = @(
      "native/objc3c/src/lower/objc3_lowering_contract.cpp"
    )
  }
  [ordered]@{
    name = "pipeline"
    sources = @(
      "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
      "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
    )
  }
  [ordered]@{
    name = "sema"
    sources = @(
      "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
      "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
      "native/objc3c/src/sema/objc3_semantic_passes.cpp"
      "native/objc3c/src/sema/objc3_static_analysis.cpp"
      "native/objc3c/src/sema/objc3_pure_contract.cpp"
    )
  }
)
$sharedSources = @(Get-FrontendSharedSourcesFromModules -Modules $frontendModules)
$frontendScaffoldPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json"
$frontendInvocationLockPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
$frontendCoreFeatureExpansionPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
$frontendEdgeCompatPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
$frontendEdgeRobustnessPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
$frontendDiagnosticsHardeningPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"

$nativeSources = @(
  "native/objc3c/src/main.cpp"
) + $sharedSources
$nativeSourcePaths = @($nativeSources | ForEach-Object { Join-Path $repoRoot $_ })

$capiRunnerSources = @(
  "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"
) + $sharedSources
$capiRunnerSourcePaths = @($capiRunnerSources | ForEach-Object { Join-Path $repoRoot $_ })

foreach ($sourcePath in @($nativeSourcePaths + $capiRunnerSourcePaths)) {
  if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
    throw "native source file missing: $sourcePath"
  }
}

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  -DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1 `
  "-I$includeDir" `
  "-I$nativeSourceRoot" `
  @nativeSourcePaths `
  $libclang `
  -o $stagedOutExe

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Publish-ArtifactWithRetry -StagedPath $stagedOutExe -FinalPath $outExe

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  -DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1 `
  "-I$includeDir" `
  "-I$nativeSourceRoot" `
  @capiRunnerSourcePaths `
  $libclang `
  -o $stagedOutCapiExe

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Publish-ArtifactWithRetry -StagedPath $stagedOutCapiExe -FinalPath $outCapiExe
Write-FrontendModuleScaffoldArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendScaffoldPath `
  -Modules $frontendModules `
  -SharedSources $sharedSources `
  -BinaryTargets @(
    (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe),
    (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe)
  )
Write-FrontendInvocationLockArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendInvocationLockPath `
  -NativeBinaryPath $outExe `
  -CapiBinaryPath $outCapiExe `
  -FrontendScaffoldPath $frontendScaffoldPath
Write-FrontendCoreFeatureExpansionArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendCoreFeatureExpansionPath `
  -Modules $frontendModules `
  -SharedSources $sharedSources `
  -NativeBinaryPath $outExe `
  -CapiBinaryPath $outCapiExe `
  -FrontendScaffoldPath $frontendScaffoldPath `
  -FrontendInvocationLockPath $frontendInvocationLockPath
Write-FrontendEdgeCompatibilityArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendEdgeCompatPath `
  -FrontendCoreFeatureExpansionPath $frontendCoreFeatureExpansionPath
Write-FrontendEdgeRobustnessArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendEdgeRobustnessPath `
  -FrontendEdgeCompatibilityPath $frontendEdgeCompatPath
Write-FrontendDiagnosticsHardeningArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendDiagnosticsHardeningPath `
  -FrontendEdgeRobustnessPath $frontendEdgeRobustnessPath
Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
Write-Output ("frontend_scaffold=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendScaffoldPath))
Write-Output ("frontend_invocation_lock=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendInvocationLockPath))
Write-Output ("frontend_core_feature_expansion=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendCoreFeatureExpansionPath))
Write-Output ("frontend_edge_compat=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendEdgeCompatPath))
Write-Output ("frontend_edge_robustness=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendEdgeRobustnessPath))
Write-Output ("frontend_diagnostics_hardening=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendDiagnosticsHardeningPath))
