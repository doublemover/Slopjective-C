param(
  [ValidateSet("full", "binaries-only", "contracts-source", "contracts-binary", "contracts-closeout", "contracts-all")]
  [string]$ExecutionMode = "full",
  [switch]$ForceReconfigure,
  [string]$CleanRoomRoot = "",
  [string]$BuildDir = "",
  [string]$RuntimeOutputDir = "",
  [string]$LibraryOutputDir = "",
  [string]$FrontendArtifactRoot = "",
  [string]$SummaryPath = "",
  [ValidateSet("release", "address", "undefined")]
  [string]$SanitizerVariant = "release",
  [int]$Parallelism = 0
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$archiveNormalizer = Join-Path $PSScriptRoot "normalize_coff_archive_timestamps.py"
if (!(Test-Path -LiteralPath $archiveNormalizer -PathType Leaf)) {
  throw "COFF archive normalizer missing: $archiveNormalizer"
}
Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force -DisableNameChecking -Global
Import-Module (Join-Path $PSScriptRoot "objc3c_native_cmake.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_platform_host_evidence_producers.psm1") -Force -DisableNameChecking
$frontendContractModuleRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_contracts"
$frontendContractExportModule = Join-Path $frontendContractModuleRoot "exports.psm1"
if (!(Test-Path -LiteralPath $frontendContractExportModule -PathType Leaf)) {
  throw "frontend contract export module missing: $frontendContractExportModule"
}
Import-Module $frontendContractExportModule -Force -DisableNameChecking -Global
foreach ($frontendContractModule in @(Get-Objc3cNativeFrontendContractModuleNames)) {
  $frontendContractModulePath = Join-Path $frontendContractModuleRoot $frontendContractModule
  if (!(Test-Path -LiteralPath $frontendContractModulePath -PathType Leaf)) {
    throw "frontend contract support module missing: $frontendContractModulePath"
  }
  Import-Module $frontendContractModulePath -Force -DisableNameChecking -Global
}
$frontendArtifactModuleRoot = Join-Path $PSScriptRoot "objc3c_native_frontend_artifacts"
foreach ($frontendArtifactModule in @(
  "constants.psm1",
  "loading.psm1",
  "assertions.psm1",
  "payloads.psm1",
  "orchestration/status.psm1",
  "orchestration/core_artifacts.psm1",
  "orchestration/packet_generation.psm1"
)) {
  $frontendArtifactModulePath = Join-Path $frontendArtifactModuleRoot $frontendArtifactModule
  if (!(Test-Path -LiteralPath $frontendArtifactModulePath -PathType Leaf)) {
    throw "frontend artifact support module missing: $frontendArtifactModulePath"
  }
  Import-Module $frontendArtifactModulePath -Force -DisableNameChecking -Global
}
Import-Module (Join-Path $PSScriptRoot "objc3c_native_superclean_surface.psm1") -Force

# objc3c.nativebuild.toolchainparity.v1 anchor:
# - authoritative toolchain flow today is `LLVM_ROOT` -> direct wrapper
#   resolution for clang++, llvm-lib, libclang, and include/library discovery
# - later incremental backend work must preserve this authoritative wrapper
#   contract when forwarding configuration into CMake/Ninja
# - compile database parity frozen to `tmp/build-objc3c-native/compile_commands.json`
#
# objc3c.nativebuild.incrementalbackend.v1 anchor:
# - native binaries now build through a persistent CMake/Ninja tree rooted at
#   `tmp/build-objc3c-native`
# - canonical outputs remain published at `artifacts/bin` and `artifacts/lib`
# - this script still owns frontend contract-artifact generation after the native build

$resolvedCleanRoomRoot = ""
if ($CleanRoomRoot) {
  $resolvedCleanRoomRoot = [System.IO.Path]::GetFullPath($CleanRoomRoot)
  New-Item -ItemType Directory -Force -Path $resolvedCleanRoomRoot | Out-Null
}
$nativeBuildPaths = Get-Objc3cNativeCMakeBuildPaths `
  -RepoRoot $repoRoot `
  -CleanRoomRoot $resolvedCleanRoomRoot `
  -BuildDir $BuildDir `
  -RuntimeOutputDir $RuntimeOutputDir `
  -LibraryOutputDir $LibraryOutputDir
$outDir = $nativeBuildPaths.RuntimeOutputDir
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outLibDir = $nativeBuildPaths.LibraryOutputDir
New-Item -ItemType Directory -Force -Path $outLibDir | Out-Null
$outExe = $nativeBuildPaths.NativeExecutable
$outCapiExe = $nativeBuildPaths.CapiRunnerExecutable
$outRuntimeLib = $nativeBuildPaths.RuntimeLibrary
$targetPlatformId = $nativeBuildPaths.TargetPlatformId
$targetTriple = $nativeBuildPaths.TargetTriple
$runtimeLibraryKind = $nativeBuildPaths.RuntimeLibraryKind
$runtimeLibraryFileName = $nativeBuildPaths.RuntimeLibraryFileName
$objectFormat = $nativeBuildPaths.ObjectFormat
$debugFormat = $nativeBuildPaths.DebugFormat
$tmpOutDir = $nativeBuildPaths.BuildDir
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null
$cmakeSourceDir = $nativeBuildPaths.CmakeSourceDir
$compileCommandsPath = $nativeBuildPaths.CompileCommands
$buildFingerprintPath = $nativeBuildPaths.BuildFingerprint
if (!$SummaryPath) {
  $SummaryPath = Join-Path $tmpOutDir "native_build_summary.json"
}
$sourceDateEpoch = "1704067200"
$env:SOURCE_DATE_EPOCH = $sourceDateEpoch

# objc3c.nativebuild.commandsurface.v1 anchor:
# - current truthful state: this script remains the authoritative wrapper
#   behind the public npm build surface
# - the public npm command taxonomy now maps to:
#   - build:objc3c-native              => fast binary-build default
#   - build:objc3c-native:contracts   => source-derived + binary-derived contract-artifact path
#   - build:objc3c-native:full        => binary + full contract-artifact family path
#   - build:objc3c-native:reconfigure => reserved fingerprint refresh/self-heal
# - direct script callers still default to `full` until the helper/runner
#   migration tranche lands

# objc3c.nativebuild.incrementalbackend.runtime.v1 anchor:
# - native binary compilation now routes through a persistent CMake/Ninja build
#   tree under `tmp/build-objc3c-native`
# - final published native artifacts remain under `artifacts/bin` and
#   `artifacts/lib`
# - the wrapper still owns frontend contract-artifact generation after the native binaries
#   are built
#
# objc3c.nativebuild.contractartifactshape.v1 anchor:
# - contract-artifact generation is internally classified into source-derived,
#   binary-derived, and closeout-derived families
# - the wrapper can execute those contract-artifact families independently without
#   silently re-triggering native binary compilation
# - public command-surface exposure remains the responsibility of the shared command contract surface

function Write-BuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}

function Get-Objc3cNativeBuildRepoRelativePath {
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
  $relative = [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)
  return $relative.Replace([System.IO.Path]::DirectorySeparatorChar, '/').Replace([System.IO.Path]::AltDirectorySeparatorChar, '/')
}

function Get-Objc3cNativeBuildFileDigest {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $relativePath = Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $TargetPath
  if (!(Test-Path -LiteralPath $TargetPath -PathType Leaf)) {
    return [ordered]@{
      path = $relativePath
      exists = $false
    }
  }

  $item = Get-Item -LiteralPath $TargetPath
  return [ordered]@{
    path = $relativePath
    exists = $true
    size_bytes = [int64]$item.Length
    sha256 = (Get-FileHash -LiteralPath $TargetPath -Algorithm SHA256).Hash.ToLowerInvariant()
  }
}

function Get-Objc3cNativeBuildLockTimeoutSeconds {
  $defaultTimeoutSeconds = 900
  if ([string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_BUILD_LOCK_TIMEOUT_SECONDS)) {
    return $defaultTimeoutSeconds
  }

  $parsedTimeoutSeconds = 0
  if (![int]::TryParse($env:OBJC3C_NATIVE_BUILD_LOCK_TIMEOUT_SECONDS, [ref]$parsedTimeoutSeconds) -or $parsedTimeoutSeconds -lt 1) {
    throw "OBJC3C_NATIVE_BUILD_LOCK_TIMEOUT_SECONDS must be a positive integer when set"
  }
  return $parsedTimeoutSeconds
}

function Enter-Objc3cNativeBuildDirectoryLock {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$BuildDirPath
  )

  New-Item -ItemType Directory -Force -Path $BuildDirPath | Out-Null
  $lockPath = Join-Path $BuildDirPath ".objc3c-native-build.lock"
  $lockRelativePath = Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $lockPath
  $timeoutSeconds = Get-Objc3cNativeBuildLockTimeoutSeconds
  $startedAt = [System.Diagnostics.Stopwatch]::StartNew()

  while ($true) {
    try {
      $stream = [System.IO.File]::Open(
        $lockPath,
        [System.IO.FileMode]::OpenOrCreate,
        [System.IO.FileAccess]::ReadWrite,
        [System.IO.FileShare]::None
      )
      $startedAt.Stop()
      $waitSeconds = [Math]::Round($startedAt.Elapsed.TotalSeconds, 3)
      $metadata = @(
        "pid=$PID",
        "build_dir=$(Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $BuildDirPath)",
        "acquired_utc=$([DateTimeOffset]::UtcNow.ToString('O'))"
      ) -join [Environment]::NewLine
      $bytes = [System.Text.Encoding]::UTF8.GetBytes($metadata + [Environment]::NewLine)
      $stream.SetLength(0)
      $stream.Write($bytes, 0, $bytes.Length)
      $stream.Flush()

      Write-BuildStep ("native_build_lock_acquired=" + $lockRelativePath)
      Write-BuildStep ("native_build_lock_wait_seconds=" + $waitSeconds)
      Write-BuildStep ("native_build_lock_timeout_seconds=" + $timeoutSeconds)
      return [ordered]@{
        acquired = $true
        path = $lockPath
        path_relative = $lockRelativePath
        wait_seconds = $waitSeconds
        timeout_seconds = $timeoutSeconds
        stream = $stream
      }
    } catch [System.IO.IOException] {
      if ($startedAt.Elapsed.TotalSeconds -ge $timeoutSeconds) {
        throw "timed out waiting for native build directory lock: $lockRelativePath"
      }
      Start-Sleep -Milliseconds 250
    }
  }
}

function Exit-Objc3cNativeBuildDirectoryLock {
  param([object]$LockState)

  if ($null -eq $LockState) {
    return
  }
  if ($null -ne $LockState.stream) {
    $LockState.stream.Dispose()
  }
  if ($LockState.path_relative) {
    Write-BuildStep ("native_build_lock_released=" + $LockState.path_relative)
  }
}

function Get-Objc3cNativeBuildLockTelemetry {
  param([object]$LockState)

  if ($null -eq $LockState) {
    return [ordered]@{
      acquired = $false
      path = ""
      wait_seconds = 0.0
      timeout_seconds = Get-Objc3cNativeBuildLockTimeoutSeconds
    }
  }

  return [ordered]@{
    acquired = [bool]$LockState.acquired
    path = [string]$LockState.path_relative
    wait_seconds = [double]$LockState.wait_seconds
    timeout_seconds = [int]$LockState.timeout_seconds
  }
}

function Write-Objc3cNativeBuildSummary {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$ExecutionModeValue,
    [Parameter(Mandatory = $true)][bool]$NativeBuildRan,
    [string]$CleanRoomRootPath = "",
    [Parameter(Mandatory = $true)][string]$BuildDirPath,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDirPath,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDirPath,
    [Parameter(Mandatory = $true)][string]$FrontendArtifactRootPath,
    [Parameter(Mandatory = $true)][string]$TargetPlatformId,
    [Parameter(Mandatory = $true)][string]$TargetTriple,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryKind,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryFileName,
    [Parameter(Mandatory = $true)][string]$ObjectFormat,
    [Parameter(Mandatory = $true)][string]$DebugFormat,
    [Parameter(Mandatory = $true)][string]$SanitizerVariantValue,
    [Parameter(Mandatory = $true)][string]$SourceDateEpoch,
    [Parameter(Mandatory = $true)][int]$Parallelism,
    [Parameter(Mandatory = $true)][string]$NativeExecutablePath,
    [Parameter(Mandatory = $true)][string]$CapiRunnerPath,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryPath,
    [Parameter(Mandatory = $true)][bool]$RuntimeArchiveNormalized,
    [Parameter(Mandatory = $true)][string]$CompileCommandsFilePath,
    [Parameter(Mandatory = $true)][string]$BuildFingerprintFilePath,
    [Parameter(Mandatory = $true)][string]$RepoSupercleanSurfaceFilePath,
    [object[]]$SelectedFrontendPacketDefinitions = @(),
    [object]$NativeBuildLockTelemetry = $null
  )

  $frontendPackets = @(
    $SelectedFrontendPacketDefinitions | ForEach-Object {
      [ordered]@{
        name = $_.Name
        family = $_.Family
        artifact = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $_.OutputPath)
      }
    }
  )

  $cleanRoomRelativePath = ""
  if ($CleanRoomRootPath) {
    $cleanRoomRelativePath = Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $CleanRoomRootPath
  }

  $summary = [ordered]@{
    contract_id = "objc3c-native-bootstrap-reproducible-build-v1"
    execution_mode = $ExecutionModeValue
    native_build_ran = $NativeBuildRan
    force_reconfigure = [bool]$ForceReconfigure
    parallelism = $Parallelism
    native_build_lock = if ($null -ne $NativeBuildLockTelemetry) { $NativeBuildLockTelemetry } else { Get-Objc3cNativeBuildLockTelemetry -LockState $null }
    source_date_epoch = $SourceDateEpoch
    sanitizer_variant = $SanitizerVariantValue
    target = [ordered]@{
      platform_id = $TargetPlatformId
      target_triple = $TargetTriple
      object_format = $ObjectFormat
      debug_format = $DebugFormat
      runtime_library_kind = $RuntimeLibraryKind
      runtime_library_file_name = $RuntimeLibraryFileName
    }
    runtime_archive_timestamps_normalized = $RuntimeArchiveNormalized
    clean_room = [bool]$CleanRoomRootPath
    clean_room_root = $cleanRoomRelativePath
    output_roots = [ordered]@{
      build_dir = (Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $BuildDirPath)
      runtime_output_dir = (Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $RuntimeOutputDirPath)
      library_output_dir = (Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $LibraryOutputDirPath)
      frontend_artifact_root = (Get-Objc3cNativeBuildRepoRelativePath -RootPath $RootPath -TargetPath $FrontendArtifactRootPath)
    }
    artifacts = [ordered]@{
      native_executable = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $NativeExecutablePath)
      capi_runner = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $CapiRunnerPath)
      runtime_library = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $RuntimeLibraryPath)
      compile_commands = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $CompileCommandsFilePath)
      build_fingerprint = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $BuildFingerprintFilePath)
      repo_superclean_surface = (Get-Objc3cNativeBuildFileDigest -RootPath $RootPath -TargetPath $RepoSupercleanSurfaceFilePath)
      frontend_packets = $frontendPackets
    }
  }

  $summaryParent = Split-Path -Parent $Path
  if ($summaryParent) {
    New-Item -ItemType Directory -Force -Path $summaryParent | Out-Null
  }
  $summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $Path -Encoding utf8
}

function Test-ExecutionModeRunsNativeBuild {
  param([Parameter(Mandatory = $true)][string]$Mode)

  return $Mode -in @("full", "binaries-only", "contracts-binary", "contracts-closeout", "contracts-all")
}

if ($Parallelism -eq 0 -and ![string]::IsNullOrWhiteSpace($env:OBJC3C_NATIVE_BUILD_PARALLELISM)) {
  $parsedParallelism = 0
  if (![int]::TryParse($env:OBJC3C_NATIVE_BUILD_PARALLELISM, [ref]$parsedParallelism) -or $parsedParallelism -lt 1) {
    throw "OBJC3C_NATIVE_BUILD_PARALLELISM must be a positive integer when set"
  }
  $Parallelism = $parsedParallelism
}
if ($Parallelism -eq 0) {
  $Parallelism = 4
}
if ($Parallelism -lt 1) {
  throw "native build parallelism must be a positive integer"
}

$modeRunsNativeBuild = Test-ExecutionModeRunsNativeBuild -Mode $ExecutionMode
if ($modeRunsNativeBuild) {
  $nativeToolchain = Resolve-Objc3cNativeToolchain -RepoRoot $repoRoot
  $llvmRoot = $nativeToolchain.LlvmRoot
  $clangxx = $nativeToolchain.Clangxx
  $llvmArTool = $nativeToolchain.LlvmArTool
  $llvmRanlibTool = $nativeToolchain.LlvmRanlibTool
  $llvmLibTool = $nativeToolchain.LlvmLibTool
  $cmakeTool = $nativeToolchain.CmakeTool
  $ninjaTool = $nativeToolchain.NinjaTool
  $libclang = $nativeToolchain.Libclang
  $includeDir = $nativeToolchain.IncludeDir
}

$frontendModules = @(Get-Objc3cNativeFrontendModules)
$sharedSources = @(Get-Objc3cNativeFrontendSharedSources -Modules $frontendModules)
$runtimeLibrarySourcePath = Join-Path $repoRoot "native/objc3c/src/runtime/objc3_runtime.cpp"
$runtimeLibraryHeaderPath = Join-Path $repoRoot "native/objc3c/src/runtime/public/objc3_runtime_api.h"
$resolvedFrontendArtifactRoot = if ($FrontendArtifactRoot) {
  [System.IO.Path]::GetFullPath($FrontendArtifactRoot)
} elseif ($resolvedCleanRoomRoot) {
  Join-Path $resolvedCleanRoomRoot "artifacts/frontend-contracts"
} else {
  Join-Path $repoRoot "tmp/artifacts/objc3c-native"
}
New-Item -ItemType Directory -Force -Path $resolvedFrontendArtifactRoot | Out-Null
$frontendArtifactPaths = Get-Objc3cNativeFrontendArtifactPaths -RepoRoot $repoRoot -ArtifactRoot $resolvedFrontendArtifactRoot
$repoSupercleanSurfacePath = Join-Path $tmpOutDir "repo_superclean_source_of_truth.json"

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
if ($modeRunsNativeBuild) {
  Write-BuildStep ("llvm_root=" + $llvmRoot)
  Write-BuildStep ("clangxx=" + $clangxx)
  Write-BuildStep ("cmake=" + $cmakeTool)
  Write-BuildStep ("ninja=" + $ninjaTool)
  Write-BuildStep ("llvm_ar=" + $llvmArTool)
  Write-BuildStep ("llvm_ranlib=" + $llvmRanlibTool)
  if ($llvmLibTool) {
    Write-BuildStep ("llvm_lib=" + $llvmLibTool)
  } else {
    Write-BuildStep "llvm_lib=not-required-for-host"
  }
} else {
  Write-BuildStep "toolchain_resolution=skipped-source-contracts"
}
Write-BuildStep ("native_sources=" + $nativeSourcePaths.Count + "; capi_sources=" + $capiRunnerSourcePaths.Count)
Write-BuildStep ("execution_mode=" + $ExecutionMode)
Write-BuildStep ("sanitizer_variant=" + $SanitizerVariant)
Write-BuildStep ("target_platform_id=" + $targetPlatformId)
Write-BuildStep ("target_triple=" + $targetTriple)
Write-BuildStep ("object_format=" + $objectFormat)
Write-BuildStep ("debug_format=" + $debugFormat)
Write-BuildStep ("runtime_library_kind=" + $runtimeLibraryKind)
$parallelismLabel = if ($Parallelism -gt 0) { [string]$Parallelism } else { "host-default" }
Write-BuildStep ("requested_parallelism=" + $parallelismLabel)
if ($resolvedCleanRoomRoot) {
  Write-BuildStep ("clean_room_root=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $resolvedCleanRoomRoot))
}
Write-BuildStep ("build_dir=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $tmpOutDir))
Write-BuildStep ("runtime_output_dir=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outDir))
Write-BuildStep ("library_output_dir=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outLibDir))
Write-BuildStep ("frontend_artifact_root=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $resolvedFrontendArtifactRoot))

$frontendPacketDefinitions = @(Get-Objc3cNativeFrontendPacketDefinitions -ArtifactPaths $frontendArtifactPaths)
$selectedFrontendPacketDefinitions = @(
  Get-Objc3cNativeSelectedFrontendPacketDefinitions `
    -Mode $ExecutionMode `
    -PacketDefinitions $frontendPacketDefinitions
)

$nativeBuildLockState = $null
$nativeBuildLockTelemetry = Get-Objc3cNativeBuildLockTelemetry -LockState $null
if ($modeRunsNativeBuild) {
  $nativeBuildLockState = Enter-Objc3cNativeBuildDirectoryLock `
    -RootPath $repoRoot `
    -BuildDirPath $tmpOutDir
  $nativeBuildLockTelemetry = Get-Objc3cNativeBuildLockTelemetry -LockState $nativeBuildLockState
  try {
  $buildFingerprint = Get-Objc3cNativeBuildFingerprint `
    -Clangxx $clangxx `
    -CmakeTool $cmakeTool `
    -NinjaTool $ninjaTool `
    -LlvmArTool $llvmArTool `
    -LlvmRanlibTool $llvmRanlibTool `
    -LlvmLibTool $llvmLibTool `
    -LlvmRoot $llvmRoot `
    -IncludeDir $includeDir `
    -Libclang $libclang `
    -BuildDir $tmpOutDir `
    -RuntimeOutputDir $outDir `
    -LibraryOutputDir $outLibDir `
    -SourceDir $cmakeSourceDir `
    -SanitizerVariant $SanitizerVariant `
    -SourceDateEpoch $sourceDateEpoch

  Invoke-Objc3cNativeCMakeConfigure `
    -CmakeTool $cmakeTool `
    -NinjaTool $ninjaTool `
    -SourceDir $cmakeSourceDir `
    -BuildDir $tmpOutDir `
    -Clangxx $clangxx `
    -LlvmArTool $llvmArTool `
    -LlvmRanlibTool $llvmRanlibTool `
    -LlvmRoot $llvmRoot `
    -IncludeDir $includeDir `
    -Libclang $libclang `
    -RuntimeOutputDir $outDir `
    -LibraryOutputDir $outLibDir `
    -SanitizerVariant $SanitizerVariant `
    -FingerprintPath $buildFingerprintPath `
    -Fingerprint $buildFingerprint `
    -ForceReconfigure $ForceReconfigure

  Invoke-Objc3cNativeCMakeBuild `
    -CmakeTool $cmakeTool `
    -BuildDir $tmpOutDir `
    -Parallelism $Parallelism

  if (!(Test-Path -LiteralPath $outExe -PathType Leaf)) { throw "native binary missing after CMake/Ninja build: $outExe" }
  if (!(Test-Path -LiteralPath $outCapiExe -PathType Leaf)) { throw "c-api runner missing after CMake/Ninja build: $outCapiExe" }
  if (!(Test-Path -LiteralPath $outRuntimeLib -PathType Leaf)) { throw "runtime library missing after CMake/Ninja build: $outRuntimeLib" }
  if (!(Test-Path -LiteralPath $compileCommandsPath -PathType Leaf)) { throw "compile_commands.json missing after CMake/Ninja configure: $compileCommandsPath" }

  if ($runtimeLibraryKind -eq "static-archive" -and $objectFormat -eq "COFF") {
    & python $archiveNormalizer $outRuntimeLib
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $runtimeArchiveNormalized = $true
  } else {
    $runtimeArchiveNormalized = $false
    Write-BuildStep "runtime_archive_normalization=not-required-for-host"
  }

  Write-BuildStep ("artifact_ready=objc3c-native -> " + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
  Write-BuildStep ("artifact_ready=objc3c-frontend-c-api-runner -> " + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
  Write-BuildStep ("artifact_ready=objc3_runtime -> " + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
  Write-BuildStep ("compile_commands=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $compileCommandsPath))
  } finally {
    Exit-Objc3cNativeBuildDirectoryLock -LockState $nativeBuildLockState
  }
} else {
  Write-BuildStep "cmake_build_skip=native-binaries"
  $runtimeArchiveNormalized = $false
}

Invoke-Objc3cNativeFrontendPacketGeneration `
  -Mode $ExecutionMode `
  -PacketDefinitions $frontendPacketDefinitions `
  -ArtifactPaths $frontendArtifactPaths `
  -RepoRoot $repoRoot `
  -NativeBinaryPath $outExe `
  -CapiBinaryPath $outCapiExe `
  -Modules $frontendModules `
  -SharedSources $sharedSources

Write-Objc3cNativeRepoSupercleanSourceOfTruthArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $repoSupercleanSurfacePath `
  -ExecutionMode $ExecutionMode `
  -CompileCommandsPath $compileCommandsPath `
  -NativeExecutablePath $outExe `
  -FrontendCapiRunnerPath $outCapiExe `
  -RuntimeLibraryPath $outRuntimeLib `
  -FrontendDefinitions $frontendPacketDefinitions

Write-Objc3cNativeBuildSummary `
  -RootPath $repoRoot `
  -Path $SummaryPath `
  -ExecutionModeValue $ExecutionMode `
  -NativeBuildRan $modeRunsNativeBuild `
  -CleanRoomRootPath $resolvedCleanRoomRoot `
  -BuildDirPath $tmpOutDir `
  -RuntimeOutputDirPath $outDir `
  -LibraryOutputDirPath $outLibDir `
  -FrontendArtifactRootPath $resolvedFrontendArtifactRoot `
  -TargetPlatformId $targetPlatformId `
  -TargetTriple $targetTriple `
  -RuntimeLibraryKind $runtimeLibraryKind `
  -RuntimeLibraryFileName $runtimeLibraryFileName `
  -ObjectFormat $objectFormat `
  -DebugFormat $debugFormat `
  -SanitizerVariantValue $SanitizerVariant `
  -SourceDateEpoch $sourceDateEpoch `
  -Parallelism $Parallelism `
  -NativeExecutablePath $outExe `
  -CapiRunnerPath $outCapiExe `
  -RuntimeLibraryPath $outRuntimeLib `
  -RuntimeArchiveNormalized $runtimeArchiveNormalized `
  -CompileCommandsFilePath $compileCommandsPath `
  -BuildFingerprintFilePath $buildFingerprintPath `
  -RepoSupercleanSurfaceFilePath $repoSupercleanSurfacePath `
  -SelectedFrontendPacketDefinitions $selectedFrontendPacketDefinitions `
  -NativeBuildLockTelemetry $nativeBuildLockTelemetry

Write-Objc3cDarwinObjectDebugIdentityEvidence `
  -RepoRoot $repoRoot `
  -PlatformId $targetPlatformId `
  -TargetTriple $targetTriple `
  -ObjectFormat $objectFormat `
  -DebugFormat $debugFormat `
  -NativeExecutablePath $outExe `
  -CapiRunnerPath $outCapiExe `
  -RuntimeLibraryPath $outRuntimeLib `
  -BuildSummaryPath $SummaryPath

if ($modeRunsNativeBuild) {
  if (Test-Path -LiteralPath $outExe -PathType Leaf) {
    Write-Output ("built=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
  }
  if (Test-Path -LiteralPath $outCapiExe -PathType Leaf) {
    Write-Output ("built=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
  }
  if (Test-Path -LiteralPath $outRuntimeLib -PathType Leaf) {
    Write-Output ("built=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
  }
}
foreach ($packetDefinition in $selectedFrontendPacketDefinitions) {
  if (Test-Path -LiteralPath $packetDefinition.OutputPath -PathType Leaf) {
    Write-Output ($packetDefinition.Name + "=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $packetDefinition.OutputPath))
  }
}
Write-Output ("repo_superclean_surface=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $repoSupercleanSurfacePath))
Write-Output ("native_build_summary=" + (Get-Objc3cNativeBuildRepoRelativePath -RootPath $repoRoot -TargetPath $SummaryPath))
