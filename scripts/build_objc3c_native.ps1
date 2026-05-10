param(
  [ValidateSet("full", "binaries-only", "contracts-source", "contracts-binary", "contracts-closeout", "contracts-all")]
  [string]$ExecutionMode = "full",
  [switch]$ForceReconfigure
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Import-Module (Join-Path $PSScriptRoot "objc3c_native_artifact_io.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_cmake.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_frontend_contracts.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_frontend_artifacts.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "objc3c_native_superclean_surface.psm1") -Force

$nativeToolchain = Resolve-Objc3cNativeToolchain -RepoRoot $repoRoot
$llvmRoot = $nativeToolchain.LlvmRoot
$clangxx = $nativeToolchain.Clangxx
$llvmLibTool = $nativeToolchain.LlvmLibTool
$cmakeTool = $nativeToolchain.CmakeTool
$ninjaTool = $nativeToolchain.NinjaTool
$libclang = $nativeToolchain.Libclang
$includeDir = $nativeToolchain.IncludeDir
$nativeSourceRoot = $nativeToolchain.NativeSourceRoot

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

$nativeBuildPaths = Get-Objc3cNativeCMakeBuildPaths -RepoRoot $repoRoot
$outDir = $nativeBuildPaths.RuntimeOutputDir
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outLibDir = $nativeBuildPaths.LibraryOutputDir
New-Item -ItemType Directory -Force -Path $outLibDir | Out-Null
$outExe = $nativeBuildPaths.NativeExecutable
$outCapiExe = $nativeBuildPaths.CapiRunnerExecutable
$outRuntimeLib = $nativeBuildPaths.RuntimeLibrary
$tmpOutDir = $nativeBuildPaths.BuildDir
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null
$cmakeSourceDir = $nativeBuildPaths.CmakeSourceDir
$compileCommandsPath = $nativeBuildPaths.CompileCommands
$buildFingerprintPath = $nativeBuildPaths.BuildFingerprint

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
$runSuffix = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
$stagedOutExe = Join-Path $tmpOutDir ("objc3c-native.{0}.exe" -f $runSuffix)
$stagedOutCapiExe = Join-Path $tmpOutDir ("objc3c-frontend-c-api-runner.{0}.exe" -f $runSuffix)
$stagedRuntimeObj = Join-Path $tmpOutDir ("objc3_runtime.{0}.obj" -f $runSuffix)
$stagedRuntimeLib = Join-Path $tmpOutDir ("objc3_runtime.{0}.lib" -f $runSuffix)

function Write-BuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}

function Test-ExecutionModeRunsNativeBuild {
  param([Parameter(Mandatory = $true)][string]$Mode)

  return $Mode -in @("full", "binaries-only")
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
    $relativeSource = Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $sourcePath
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

  Write-BuildStep ("link_start=" + $TargetName + " -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
  & $Clangxx @ObjectPaths $Libclang -o $StagedOutput
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-BuildStep ("link_done=" + $TargetName + " -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $StagedOutput))
}

$frontendModules = @(Get-Objc3cNativeFrontendModules)
$sharedSources = @(Get-Objc3cNativeFrontendSharedSources -Modules $frontendModules)
$runtimeLibrarySourcePath = Join-Path $repoRoot "native/objc3c/src/runtime/objc3_runtime.cpp"
$runtimeLibraryHeaderPath = Join-Path $repoRoot "native/objc3c/src/runtime/public/objc3_runtime_api.h"
$frontendArtifactPaths = Get-Objc3cNativeFrontendArtifactPaths -RepoRoot $repoRoot
$frontendScaffoldPath = $frontendArtifactPaths.SourceGraph
$frontendInvocationLockPath = $frontendArtifactPaths.InvocationLock
$frontendCoreFeatureExpansionPath = $frontendArtifactPaths.CoreFeatureExpansion
$frontendEdgeCompatPath = $frontendArtifactPaths.EdgeCompatibility
$frontendEdgeRobustnessPath = $frontendArtifactPaths.EdgeRobustness
$frontendDiagnosticsHardeningPath = $frontendArtifactPaths.DiagnosticsHardening
$frontendRecoveryDeterminismHardeningPath = $frontendArtifactPaths.RecoveryDeterminismHardening
$frontendConformanceMatrixPath = $frontendArtifactPaths.ConformanceMatrix
$frontendConformanceCorpusPath = $frontendArtifactPaths.ConformanceCorpus
$frontendIntegrationCloseoutPath = $frontendArtifactPaths.IntegrationCloseout
$repoSupercleanSurfacePath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"

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
Write-BuildStep ("cmake=" + $cmakeTool)
Write-BuildStep ("ninja=" + $ninjaTool)
Write-BuildStep ("llvm_lib=" + $llvmLibTool)
Write-BuildStep ("native_sources=" + $nativeSourcePaths.Count + "; capi_sources=" + $capiRunnerSourcePaths.Count)
Write-BuildStep ("execution_mode=" + $ExecutionMode)

$frontendPacketDefinitions = @(Get-Objc3cNativeFrontendPacketDefinitions -ArtifactPaths $frontendArtifactPaths)

$buildFingerprint = Get-Objc3cNativeBuildFingerprint `
  -Clangxx $clangxx `
  -CmakeTool $cmakeTool `
  -NinjaTool $ninjaTool `
  -LlvmRoot $llvmRoot `
  -IncludeDir $includeDir `
  -Libclang $libclang `
  -BuildDir $tmpOutDir `
  -RuntimeOutputDir $outDir `
  -LibraryOutputDir $outLibDir `
  -SourceDir $cmakeSourceDir

if (Test-ExecutionModeRunsNativeBuild -Mode $ExecutionMode) {
  Invoke-Objc3cNativeCMakeConfigure `
    -CmakeTool $cmakeTool `
    -NinjaTool $ninjaTool `
    -SourceDir $cmakeSourceDir `
    -BuildDir $tmpOutDir `
    -Clangxx $clangxx `
    -LlvmRoot $llvmRoot `
    -IncludeDir $includeDir `
    -Libclang $libclang `
    -RuntimeOutputDir $outDir `
    -LibraryOutputDir $outLibDir `
    -FingerprintPath $buildFingerprintPath `
    -Fingerprint $buildFingerprint `
    -ForceReconfigure $ForceReconfigure

  Invoke-Objc3cNativeCMakeBuild `
    -CmakeTool $cmakeTool `
    -BuildDir $tmpOutDir

  if (!(Test-Path -LiteralPath $outExe -PathType Leaf)) { throw "native binary missing after CMake/Ninja build: $outExe" }
  if (!(Test-Path -LiteralPath $outCapiExe -PathType Leaf)) { throw "c-api runner missing after CMake/Ninja build: $outCapiExe" }
  if (!(Test-Path -LiteralPath $outRuntimeLib -PathType Leaf)) { throw "runtime library missing after CMake/Ninja build: $outRuntimeLib" }
  if (!(Test-Path -LiteralPath $compileCommandsPath -PathType Leaf)) { throw "compile_commands.json missing after CMake/Ninja configure: $compileCommandsPath" }

  Write-BuildStep ("artifact_ready=objc3c-native -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
  Write-BuildStep ("artifact_ready=objc3c-frontend-c-api-runner -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
  Write-BuildStep ("artifact_ready=objc3_runtime -> " + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
  Write-BuildStep ("compile_commands=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $compileCommandsPath))
} else {
  Write-BuildStep "cmake_build_skip=native-binaries"
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

if (Test-Path -LiteralPath $outExe -PathType Leaf) {
  Write-Output ("built=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
}
if (Test-Path -LiteralPath $outCapiExe -PathType Leaf) {
  Write-Output ("built=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
}
if (Test-Path -LiteralPath $outRuntimeLib -PathType Leaf) {
  Write-Output ("built=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
}
foreach ($packetDefinition in $frontendPacketDefinitions) {
  if (Test-Path -LiteralPath $packetDefinition.OutputPath -PathType Leaf) {
    Write-Output ($packetDefinition.Name + "=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $packetDefinition.OutputPath))
  }
}
Write-Output ("repo_superclean_surface=" + (Get-Objc3cNativeRepoRelativePath -RootPath $repoRoot -TargetPath $repoSupercleanSurfacePath))
