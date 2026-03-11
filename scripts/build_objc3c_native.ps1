$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$llvmRoot = if ($env:LLVM_ROOT) { $env:LLVM_ROOT } else { "C:\Program Files\LLVM" }
$clangxx = Join-Path $llvmRoot "bin\clang++.exe"
$llvmLibTool = Join-Path $llvmRoot "bin\llvm-lib.exe"

if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
  $clangCommand = Get-Command clang++ -ErrorAction SilentlyContinue
  if ($null -ne $clangCommand -and (Test-Path -LiteralPath $clangCommand.Source -PathType Leaf)) {
    $clangxx = $clangCommand.Source
    $clangBinDir = Split-Path -Parent $clangxx
    $llvmRoot = Split-Path -Parent $clangBinDir
    $llvmLibTool = Join-Path $llvmRoot "bin\llvm-lib.exe"
  }
}

if (!(Test-Path -LiteralPath $llvmLibTool -PathType Leaf)) {
  $llvmLibCommand = Get-Command llvm-lib -ErrorAction SilentlyContinue
  if ($null -ne $llvmLibCommand -and (Test-Path -LiteralPath $llvmLibCommand.Source -PathType Leaf)) {
    $llvmLibTool = $llvmLibCommand.Source
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
if (!(Test-Path -LiteralPath $llvmLibTool -PathType Leaf)) {
  throw ("llvm-lib not found. set LLVM_ROOT or ensure llvm-lib is on PATH (attempted: " + $llvmLibTool + ")")
}
if ($null -eq $libclang) {
  $attempted = [string]::Join(", ", $libclangCandidates)
  throw ("LLVM import library not found. set LLVM_ROOT to a full LLVM install (attempted: " + $attempted + ")")
}
if (!(Test-Path -LiteralPath $includeDir -PathType Container)) { throw "LLVM include dir not found at $includeDir" }
if (!(Test-Path -LiteralPath $nativeSourceRoot -PathType Container)) { throw "native source root not found at $nativeSourceRoot" }

$outDir = Join-Path $repoRoot "artifacts/bin"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outLibDir = Join-Path $repoRoot "artifacts/lib"
New-Item -ItemType Directory -Force -Path $outLibDir | Out-Null
$outExe = Join-Path $outDir "objc3c-native.exe"
$outCapiExe = Join-Path $outDir "objc3c-frontend-c-api-runner.exe"
$outRuntimeLib = Join-Path $outLibDir "objc3_runtime.lib"
$tmpOutDir = Join-Path $repoRoot "tmp/build-objc3c-native"
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null

# M276-A001 native-build-command-surface anchor:
# - current truthful state: this script remains the monolithic authoritative
#   build path behind `npm run build:objc3c-native`
# - future command taxonomy reserved by contract only at this stage:
#   - build:objc3c-native              => eventual fast local binary-build default
#   - build:objc3c-native:contracts   => reserved packet-generation path
#   - build:objc3c-native:full        => reserved closeout/CI full-build path
#   - build:objc3c-native:reconfigure => reserved fingerprint refresh/self-heal
# - later M276 issues must prove parity before `build:objc3c-native` changes
#   behavior
$runSuffix = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
$stagedOutExe = Join-Path $tmpOutDir ("objc3c-native.{0}.exe" -f $runSuffix)
$stagedOutCapiExe = Join-Path $tmpOutDir ("objc3c-frontend-c-api-runner.{0}.exe" -f $runSuffix)
$stagedRuntimeObj = Join-Path $tmpOutDir ("objc3_runtime.{0}.obj" -f $runSuffix)
$stagedRuntimeLib = Join-Path $tmpOutDir ("objc3_runtime.{0}.lib" -f $runSuffix)

function Write-BuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}

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

function New-StagedObjectPath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$ObjectDir,
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [int]$Index
  )

  return (Join-Path $ObjectDir ("{0}.{1:D3}.obj" -f $TargetName, $Index))
}

function Compile-ObjectFiles {
  param(
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [string[]]$SourcePaths,
    [Parameter(Mandatory = $true)]
    [string]$ObjectDir,
    [Parameter(Mandatory = $true)]
    [string]$Clangxx,
    [Parameter(Mandatory = $true)]
    [string]$IncludeDir,
    [Parameter(Mandatory = $true)]
    [string]$NativeSourceRoot,
    [bool]$EnableLlvmDirectObjectEmission = $true
  )

  New-Item -ItemType Directory -Force -Path $ObjectDir | Out-Null
  $objectPaths = New-Object System.Collections.Generic.List[string]
  for ($index = 0; $index -lt $SourcePaths.Count; $index++) {
    $sourcePath = $SourcePaths[$index]
    $objectPath = New-StagedObjectPath -ObjectDir $ObjectDir -TargetName $TargetName -Index $index
    $relativeSource = Get-RepoRelativePath -RootPath $repoRoot -TargetPath $sourcePath
    Write-BuildStep ("compile_unit=" + $TargetName + " [" + ($index + 1) + "/" + $SourcePaths.Count + "] -> " + $relativeSource)
    $compileArgs = @(
      "-std=c++20"
      "-Wall"
      "-Wextra"
      "-pedantic"
    )
    if ($EnableLlvmDirectObjectEmission) {
      $compileArgs += "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1"
    }
    $compileArgs += @(
      "-I$IncludeDir"
      "-I$NativeSourceRoot"
      "-c"
      $sourcePath
      "-o"
      $objectPath
    )
    & $Clangxx @compileArgs
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $objectPaths.Add($objectPath) | Out-Null
  }

  return $objectPaths.ToArray()
}

function Link-ExecutableFromObjects {
  param(
    [Parameter(Mandatory = $true)]
    [string]$TargetName,
    [Parameter(Mandatory = $true)]
    [string[]]$ObjectPaths,
    [Parameter(Mandatory = $true)]
    [string]$Libclang,
    [Parameter(Mandatory = $true)]
    [string]$Clangxx,
    [Parameter(Mandatory = $true)]
    [string]$StagedOutput,
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot
  )

  Write-BuildStep ("link_start=" + $TargetName + " -> " + (Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
  & $Clangxx @ObjectPaths $Libclang -o $StagedOutput
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-BuildStep ("link_done=" + $TargetName + " -> " + (Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
}

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RootPath,
    [Parameter(Mandatory = $true)]
    [string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path.TrimEnd('\', '/')
  if (Test-Path -LiteralPath $TargetPath) {
    $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  } else {
    $resolvedTarget = [System.IO.Path]::GetFullPath($TargetPath)
  }
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

function Write-FrontendRecoveryDeterminismHardeningArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendDiagnosticsHardeningPath
  )

  if (!(Test-Path -LiteralPath $FrontendDiagnosticsHardeningPath -PathType Leaf)) {
    throw "frontend diagnostics hardening artifact missing for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  try {
    $diagnosticsPayload = Get-Content -LiteralPath $FrontendDiagnosticsHardeningPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend diagnostics hardening artifact is not valid JSON for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $expectedDiagnosticsContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/m226-d007-v1"
  if ([string]$diagnosticsPayload.contract_id -ne $expectedDiagnosticsContractId) {
    throw "frontend diagnostics hardening contract id mismatch for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedDiagnosticsContractId
      "objc3c-frontend-build-invocation-edge-robustness/m226-d006-v1"
    )
    cache_determinism = [ordered]@{
      fail_closed_exit_code = 2
      entry_contract_id = "objc3c-native-cache-entry/m226-d008-v1"
      cache_status_tokens = @(
        "cache_hit=true"
        "cache_hit=false"
      )
      required_entry_files = @(
        "files"
        "exit_code.txt"
        "ready.marker"
        "metadata.json"
      )
      recovery_signals = @(
        "cache_recovery=metadata_missing"
        "cache_recovery=metadata_invalid"
        "cache_recovery=metadata_contract_mismatch"
        "cache_recovery=metadata_cache_key_mismatch"
        "cache_recovery=metadata_exit_code_mismatch"
        "cache_recovery=metadata_digest_mismatch"
        "cache_recovery=restore_failed"
      )
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8
}

function Write-FrontendConformanceMatrixArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendRecoveryDeterminismHardeningPath
  )

  if (!(Test-Path -LiteralPath $FrontendRecoveryDeterminismHardeningPath -PathType Leaf)) {
    throw "frontend recovery determinism hardening artifact missing for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  try {
    $recoveryPayload = Get-Content -LiteralPath $FrontendRecoveryDeterminismHardeningPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend recovery determinism hardening artifact is not valid JSON for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  $expectedRecoveryContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1"
  if ([string]$recoveryPayload.contract_id -ne $expectedRecoveryContractId) {
    throw "frontend recovery determinism hardening contract id mismatch for conformance matrix artifact: $FrontendRecoveryDeterminismHardeningPath"
  }

  $cacheModes = @("no-cache", "cache-aware")
  $backendModes = @("default", "clang", "llvm-direct")
  $summaryModes = @("none", "present")
  $acceptRows = New-Object System.Collections.Generic.List[object]
  $caseOrdinal = 1
  foreach ($cacheMode in $cacheModes) {
    foreach ($backendMode in $backendModes) {
      foreach ($summaryMode in $summaryModes) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $acceptRows.Add([ordered]@{
          case_id = ("D009-C{0:D3}" -f $caseOrdinal)
          profile_key = $profileKey
          expected_result = "accept"
          cache_mode = $cacheMode
          backend_mode = $backendMode
          routing_mode = "manual"
          capability_summary_mode = $summaryMode
        })
        $caseOrdinal++
      }

      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $acceptRows.Add([ordered]@{
        case_id = ("D009-C{0:D3}" -f $caseOrdinal)
        profile_key = $profileKey
        expected_result = "accept"
        cache_mode = $cacheMode
        backend_mode = $backendMode
        routing_mode = "capability-route"
        capability_summary_mode = "present"
      })
      $caseOrdinal++
    }
  }

  $rejectRows = @(
    [ordered]@{
      case_id = "D009-R001"
      profile_key = "any|any|capability-route|none"
      expected_result = "reject"
      required_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R002"
      profile_key = "any|unsupported-backend|any|any"
      expected_result = "reject"
      required_diagnostic = "unsupported value '<backend>' for --objc3-ir-object-backend"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R003"
      profile_key = "any|any|any|any"
      expected_result = "reject"
      required_diagnostic = "--objc3-ir-object-backend can be provided at most once"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R004"
      profile_key = "any|any|any|path-parent-segment"
      expected_result = "reject"
      required_diagnostic = "--llvm-capabilities-summary must not contain '..' relative segments"
      fail_closed_exit_code = 2
    }
    [ordered]@{
      case_id = "D009-R005"
      profile_key = "any|any|duplicate-route-flag|any"
      expected_result = "reject"
      required_diagnostic = "--objc3-route-backend-from-capabilities can be provided at most once"
      fail_closed_exit_code = 2
    }
  )

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedRecoveryContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    matrix_dimensions = [ordered]@{
      cache_modes = $cacheModes
      backend_modes = $backendModes
      routing_modes = @("manual", "capability-route")
      capability_summary_modes = $summaryModes
    }
    acceptance_profile_count = $acceptRows.Count
    rejection_profile_count = $rejectRows.Count
    acceptance_matrix = $acceptRows.ToArray()
    rejection_matrix = $rejectRows
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 10) -Encoding utf8
}

function Write-FrontendConformanceCorpusArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceMatrixPath
  )

  if (!(Test-Path -LiteralPath $FrontendConformanceMatrixPath -PathType Leaf)) {
    throw "frontend conformance matrix artifact missing for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  try {
    $matrixPayload = Get-Content -LiteralPath $FrontendConformanceMatrixPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend conformance matrix artifact is not valid JSON for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  $expectedMatrixContractId = "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1"
  if ([string]$matrixPayload.contract_id -ne $expectedMatrixContractId) {
    throw "frontend conformance matrix contract id mismatch for conformance corpus artifact: $FrontendConformanceMatrixPath"
  }

  $acceptRows = New-Object System.Collections.Generic.List[object]
  $acceptOrdinal = 1
  foreach ($row in @($matrixPayload.acceptance_matrix)) {
    $profileKey = [string]$row.profile_key
    if ([string]::IsNullOrWhiteSpace($profileKey)) {
      continue
    }
    $segments = $profileKey.Split("|")
    if ($segments.Length -ne 4) {
      continue
    }
    $cacheMode = [string]$segments[0]
    $backendMode = [string]$segments[1]
    $routingMode = [string]$segments[2]
    $summaryMode = [string]$segments[3]
    $compileArgs = New-Object System.Collections.Generic.List[string]
    $compileArgs.Add("tests/tooling/fixtures/m226_a010_parser_conformance_corpus/accept_void_pointer_param.objc3")
    $compileArgs.Add("--out-dir=tmp/reports/m226/M226-D010/out")
    if ($backendMode -ne "default") {
      $compileArgs.Add("--objc3-ir-object-backend=$backendMode")
    }
    if ($summaryMode -eq "present") {
      $compileArgs.Add("--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json")
    }
    if ($routingMode -eq "capability-route") {
      $compileArgs.Add("--objc3-route-backend-from-capabilities")
    }
    $acceptRows.Add([ordered]@{
      corpus_case_id = ("D010-C{0:D3}" -f $acceptOrdinal)
      profile_key = $profileKey
      expected_result = "accept"
      use_cache = ($cacheMode -eq "cache-aware")
      expected_exit_code = 0
      compile_args = $compileArgs.ToArray()
    })
    $acceptOrdinal++
  }

  $rejectRows = @(
    [ordered]@{
      corpus_case_id = "D010-R001"
      matrix_case_id = "D009-R001"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
      compile_args = @(
        "tests/tooling/fixtures/m226_a010_parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/m226/M226-D010/out"
        "--objc3-route-backend-from-capabilities"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R002"
      matrix_case_id = "D009-R002"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "unsupported value '<backend>' for --objc3-ir-object-backend"
      compile_args = @(
        "tests/tooling/fixtures/m226_a010_parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/m226/M226-D010/out"
        "--objc3-ir-object-backend=unsupported-backend"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R003"
      matrix_case_id = "D009-R003"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-ir-object-backend can be provided at most once"
      compile_args = @(
        "tests/tooling/fixtures/m226_a010_parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/m226/M226-D010/out"
        "--objc3-ir-object-backend=clang"
        "--objc3-ir-object-backend=llvm-direct"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R004"
      matrix_case_id = "D009-R004"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--llvm-capabilities-summary must not contain '..' relative segments"
      compile_args = @(
        "tests/tooling/fixtures/m226_a010_parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/m226/M226-D010/out"
        "--llvm-capabilities-summary=../outside/capabilities.json"
      )
    }
    [ordered]@{
      corpus_case_id = "D010-R005"
      matrix_case_id = "D009-R005"
      expected_result = "reject"
      expected_exit_code = 2
      expected_diagnostic = "--objc3-route-backend-from-capabilities can be provided at most once"
      compile_args = @(
        "tests/tooling/fixtures/m226_a010_parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/m226/M226-D010/out"
        "--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json"
        "--objc3-route-backend-from-capabilities"
        "--objc3-route-backend-from-capabilities"
      )
    }
  )

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-corpus/m226-d010-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedMatrixContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/m226-d005-v1"
    )
    profile_key_fields = @(
      "cache_mode"
      "backend_mode"
      "routing_mode"
      "capability_summary_mode"
    )
    acceptance_corpus_count = $acceptRows.Count
    rejection_corpus_count = $rejectRows.Count
    corpus_case_count = $acceptRows.Count + $rejectRows.Count
    acceptance_corpus = $acceptRows.ToArray()
    rejection_corpus = $rejectRows
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 12) -Encoding utf8
}

function Write-FrontendIntegrationCloseoutArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendConformanceCorpusPath
  )

  if (!(Test-Path -LiteralPath $FrontendConformanceCorpusPath -PathType Leaf)) {
    throw "frontend conformance corpus artifact missing for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  try {
    $corpusPayload = Get-Content -LiteralPath $FrontendConformanceCorpusPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend conformance corpus artifact is not valid JSON for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $expectedCorpusContractId = "objc3c-frontend-build-invocation-conformance-corpus/m226-d010-v1"
  if ([string]$corpusPayload.contract_id -ne $expectedCorpusContractId) {
    throw "frontend conformance corpus contract id mismatch for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $acceptanceCount = [int]$corpusPayload.acceptance_corpus_count
  $rejectionCount = [int]$corpusPayload.rejection_corpus_count
  if ($acceptanceCount -le 0 -or $rejectionCount -le 0) {
    throw "frontend conformance corpus must provide non-empty acceptance and rejection coverage for integration closeout"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-integration-closeout/m226-d011-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCorpusContractId
      "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1"
      "objc3c-frontend-build-invocation-recovery-determinism-hardening/m226-d008-v1"
    )
    closeout_gate = [ordered]@{
      build_integration_gate_signoff = $true
      invocation_profile_gate_signoff = $true
      corpus_coverage_gate_signoff = $true
      deterministic_fail_closed_exit_code = 2
      acceptance_corpus_count = $acceptanceCount
      rejection_corpus_count = $rejectionCount
    }
  }

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }
  Set-Content -LiteralPath $OutputPath -Value ($payload | ConvertTo-Json -Depth 12) -Encoding utf8
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
      "native/objc3c/src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp"
      "native/objc3c/src/parse/objc3_diagnostic_source_precision_scaffold.cpp"
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
      "native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp"
      "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
      "native/objc3c/src/pipeline/objc3_ir_emission_completeness_scaffold.cpp"
      "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp"
      "native/objc3c/src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp"
    )
  }
  [ordered]@{
    name = "sema"
    sources = @(
      "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
      "native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp"
      "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
      "native/objc3c/src/sema/objc3_semantic_passes.cpp"
      "native/objc3c/src/sema/objc3_type_form_scaffold.cpp"
      "native/objc3c/src/sema/objc3_static_analysis.cpp"
      "native/objc3c/src/sema/objc3_pure_contract.cpp"
    )
  }
)
$sharedSources = @(Get-FrontendSharedSourcesFromModules -Modules $frontendModules)
$runtimeLibrarySourcePath = Join-Path $repoRoot "native/objc3c/src/runtime/objc3_runtime.cpp"
$runtimeLibraryHeaderPath = Join-Path $repoRoot "native/objc3c/src/runtime/objc3_runtime.h"
$frontendScaffoldPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json"
$frontendInvocationLockPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
$frontendCoreFeatureExpansionPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
$frontendEdgeCompatPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
$frontendEdgeRobustnessPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
$frontendDiagnosticsHardeningPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
$frontendRecoveryDeterminismHardeningPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
$frontendConformanceMatrixPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
$frontendConformanceCorpusPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
$frontendIntegrationCloseoutPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"

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
foreach ($runtimePath in @($runtimeLibrarySourcePath, $runtimeLibraryHeaderPath)) {
  if (!(Test-Path -LiteralPath $runtimePath -PathType Leaf)) {
    throw "runtime library file missing: $runtimePath"
  }
}

Write-BuildStep ("repo_root=" + $repoRoot)
Write-BuildStep ("llvm_root=" + $llvmRoot)
Write-BuildStep ("clangxx=" + $clangxx)
Write-BuildStep ("llvm_lib=" + $llvmLibTool)
Write-BuildStep ("native_sources=" + $nativeSourcePaths.Count + "; capi_sources=" + $capiRunnerSourcePaths.Count)
Write-BuildStep ("compile_start=objc3c-native -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
$nativeObjectDir = Join-Path $tmpOutDir ("objects.native." + $runSuffix)
$nativeObjectPaths = Compile-ObjectFiles `
  -TargetName "objc3c-native" `
  -SourcePaths $nativeSourcePaths `
  -ObjectDir $nativeObjectDir `
  -Clangxx $clangxx `
  -IncludeDir $includeDir `
  -NativeSourceRoot $nativeSourceRoot `
  -EnableLlvmDirectObjectEmission $true
Link-ExecutableFromObjects `
  -TargetName "objc3c-native" `
  -ObjectPaths $nativeObjectPaths `
  -Libclang $libclang `
  -Clangxx $clangxx `
  -StagedOutput $stagedOutExe `
  -RepoRoot $repoRoot
Publish-ArtifactWithRetry -StagedPath $stagedOutExe -FinalPath $outExe
Write-BuildStep ("compile_done=objc3c-native -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
Write-BuildStep ("compile_start=objc3c-frontend-c-api-runner -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
$capiObjectDir = Join-Path $tmpOutDir ("objects.capi." + $runSuffix)
$capiObjectPaths = Compile-ObjectFiles `
  -TargetName "objc3c-frontend-c-api-runner" `
  -SourcePaths $capiRunnerSourcePaths `
  -ObjectDir $capiObjectDir `
  -Clangxx $clangxx `
  -IncludeDir $includeDir `
  -NativeSourceRoot $nativeSourceRoot `
  -EnableLlvmDirectObjectEmission $true
Link-ExecutableFromObjects `
  -TargetName "objc3c-frontend-c-api-runner" `
  -ObjectPaths $capiObjectPaths `
  -Libclang $libclang `
  -Clangxx $clangxx `
  -StagedOutput $stagedOutCapiExe `
  -RepoRoot $repoRoot
Publish-ArtifactWithRetry -StagedPath $stagedOutCapiExe -FinalPath $outCapiExe
Write-BuildStep ("compile_done=objc3c-frontend-c-api-runner -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
Write-BuildStep ("compile_start=objc3_runtime.obj -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $stagedRuntimeObj))

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  "-I$nativeSourceRoot" `
  -c `
  $runtimeLibrarySourcePath `
  -o $stagedRuntimeObj

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-BuildStep ("compile_done=objc3_runtime.obj -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $stagedRuntimeObj))
Write-BuildStep ("archive_start=objc3_runtime -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))

& $llvmLibTool `
  /NOLOGO `
  "/OUT:$stagedRuntimeLib" `
  $stagedRuntimeObj

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Publish-ArtifactWithRetry -StagedPath $stagedRuntimeLib -FinalPath $outRuntimeLib
Write-BuildStep ("archive_done=objc3_runtime -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
Write-BuildStep "artifact_generation_start=frontend_contract_packets"
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
Write-FrontendRecoveryDeterminismHardeningArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendRecoveryDeterminismHardeningPath `
  -FrontendDiagnosticsHardeningPath $frontendDiagnosticsHardeningPath
Write-FrontendConformanceMatrixArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendConformanceMatrixPath `
  -FrontendRecoveryDeterminismHardeningPath $frontendRecoveryDeterminismHardeningPath
Write-FrontendConformanceCorpusArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendConformanceCorpusPath `
  -FrontendConformanceMatrixPath $frontendConformanceMatrixPath
Write-FrontendIntegrationCloseoutArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $frontendIntegrationCloseoutPath `
  -FrontendConformanceCorpusPath $frontendConformanceCorpusPath
Write-BuildStep "artifact_generation_done=frontend_contract_packets"
Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
Write-Output ("frontend_scaffold=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendScaffoldPath))
Write-Output ("frontend_invocation_lock=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendInvocationLockPath))
Write-Output ("frontend_core_feature_expansion=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendCoreFeatureExpansionPath))
Write-Output ("frontend_edge_compat=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendEdgeCompatPath))
Write-Output ("frontend_edge_robustness=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendEdgeRobustnessPath))
Write-Output ("frontend_diagnostics_hardening=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendDiagnosticsHardeningPath))
Write-Output ("frontend_recovery_determinism_hardening=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendRecoveryDeterminismHardeningPath))
Write-Output ("frontend_conformance_matrix=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendConformanceMatrixPath))
Write-Output ("frontend_conformance_corpus=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendConformanceCorpusPath))
Write-Output ("frontend_integration_closeout=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $frontendIntegrationCloseoutPath))
