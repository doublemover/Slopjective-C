param(
  [ValidateSet("full", "binaries-only", "contracts-source", "contracts-binary", "contracts-closeout", "contracts-all")]
  [string]$ExecutionMode = "full",
  [switch]$ForceReconfigure
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$llvmRoot = if ($env:LLVM_ROOT) { $env:LLVM_ROOT } else { "C:\Program Files\LLVM" }
$clangxx = Join-Path $llvmRoot "bin\clang++.exe"
$llvmLibTool = Join-Path $llvmRoot "bin\llvm-lib.exe"
$cmakeTool = $null
$ninjaTool = $null

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

$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
if ($null -ne $cmakeCommand -and (Test-Path -LiteralPath $cmakeCommand.Source -PathType Leaf)) {
  $cmakeTool = $cmakeCommand.Source
}

$ninjaCommand = Get-Command ninja -ErrorAction SilentlyContinue
if ($null -ne $ninjaCommand -and (Test-Path -LiteralPath $ninjaCommand.Source -PathType Leaf)) {
  $ninjaTool = $ninjaCommand.Source
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
if ($null -eq $cmakeTool) { throw "cmake not found. ensure cmake is on PATH" }
if ($null -eq $ninjaTool) { throw "ninja not found. ensure ninja is on PATH" }

# M276-A002 build-graph-and-toolchain-parity anchor:
# - authoritative toolchain flow today is `LLVM_ROOT` -> direct wrapper
#   resolution for clang++, llvm-lib, libclang, and include/library discovery
# - later incremental backend work must preserve this authoritative wrapper
#   contract when forwarding configuration into CMake/Ninja
# - compile database parity frozen to `tmp/build-objc3c-native/compile_commands.json`
#
# M276-C001 persistent-cmake-ninja-incremental-backend anchor:
# - native binaries now build through a persistent CMake/Ninja tree rooted at
#   `tmp/build-objc3c-native`
# - canonical outputs remain published at `artifacts/bin` and `artifacts/lib`
# - this script still owns frontend contract-artifact generation after the native build

$outDir = Join-Path $repoRoot "artifacts/bin"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outLibDir = Join-Path $repoRoot "artifacts/lib"
New-Item -ItemType Directory -Force -Path $outLibDir | Out-Null
$outExe = Join-Path $outDir "objc3c-native.exe"
$outCapiExe = Join-Path $outDir "objc3c-frontend-c-api-runner.exe"
$outRuntimeLib = Join-Path $outLibDir "objc3_runtime.lib"
$tmpOutDir = Join-Path $repoRoot "tmp/build-objc3c-native"
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null
$cmakeSourceDir = Join-Path $repoRoot "native/objc3c"
$compileCommandsPath = Join-Path $tmpOutDir "compile_commands.json"
$buildFingerprintPath = Join-Path $tmpOutDir "native_build_backend_fingerprint.json"

# M276-A001 native-build-command-surface anchor:
# - current truthful state: this script remains the authoritative wrapper
#   behind the public npm build surface
# - the public npm command taxonomy now maps to:
#   - build:objc3c-native              => fast binary-build default
#   - build:objc3c-native:contracts   => source-derived + binary-derived contract-artifact path
#   - build:objc3c-native:full        => binary + full contract-artifact family path
#   - build:objc3c-native:reconfigure => reserved fingerprint refresh/self-heal
# - direct script callers still default to `full` until the helper/runner
#   migration tranche lands

# M276-C001 native-binary-backend anchor:
# - native binary compilation now routes through a persistent CMake/Ninja build
#   tree under `tmp/build-objc3c-native`
# - final published native artifacts remain under `artifacts/bin` and
#   `artifacts/lib`
# - the wrapper still owns frontend contract-artifact generation after the native binaries
#   are built
#
# M276-C003 contract-artifact-dependency-shape anchor:
# - contract-artifact generation is internally classified into source-derived,
#   binary-derived, and closeout-derived families
# - the wrapper can execute those contract-artifact families independently without
#   silently re-triggering native binary compilation
# - public command-surface exposure remains the responsibility of M276-C002
$runSuffix = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
$stagedOutExe = Join-Path $tmpOutDir ("objc3c-native.{0}.exe" -f $runSuffix)
$stagedOutCapiExe = Join-Path $tmpOutDir ("objc3c-frontend-c-api-runner.{0}.exe" -f $runSuffix)
$stagedRuntimeObj = Join-Path $tmpOutDir ("objc3_runtime.{0}.obj" -f $runSuffix)
$stagedRuntimeLib = Join-Path $tmpOutDir ("objc3_runtime.{0}.lib" -f $runSuffix)

function Write-BuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[build:objc3c-native] " + $Message)
}

function Write-JsonArtifactFile {
  param(
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    $Payload,
    [int]$Depth = 8
  )

  $parent = Split-Path -Parent $OutputPath
  if (![string]::IsNullOrWhiteSpace($parent)) {
    New-Item -ItemType Directory -Force -Path $parent | Out-Null
  }

  $leaf = Split-Path -Leaf $OutputPath
  $tempPath = Join-Path $parent ('.' + $leaf + '.' + [Guid]::NewGuid().ToString('N') + '.tmp')
  $json = $Payload | ConvertTo-Json -Depth $Depth
  Set-Content -LiteralPath $tempPath -Value $json -Encoding utf8
  $overwriteMoveMethod = [System.IO.File].GetMethod("Move", [Type[]]@([string], [string], [bool]))
  $maxAttempts = 12
  for ($attempt = 1; $attempt -le $maxAttempts; $attempt++) {
    try {
      if ($null -ne $overwriteMoveMethod) {
        [System.IO.File]::Move($tempPath, $OutputPath, $true)
      }
      else {
        [System.IO.File]::Copy($tempPath, $OutputPath, $true)
        Remove-Item -LiteralPath $tempPath -Force
      }
      return
    }
    catch {
      if ($attempt -eq $maxAttempts) {
        throw
      }
      Start-Sleep -Milliseconds 100
    }
  }
}

function Test-ExecutionModeRunsNativeBuild {
  param([Parameter(Mandatory = $true)][string]$Mode)

  return $Mode -in @("full", "binaries-only")
}

function Get-FrontendPacketDefinitions {
  return @(
    [pscustomobject]@{
      Name = "frontend_source_graph"
      Family = "source-derived"
      OutputPath = $frontendScaffoldPath
      Dependencies = @()
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_invocation_lock"
      Family = "binary-derived"
      OutputPath = $frontendInvocationLockPath
      Dependencies = @("frontend_source_graph")
      RequiresNativeBinaries = $true
    }
    [pscustomobject]@{
      Name = "frontend_core_feature_expansion"
      Family = "binary-derived"
      OutputPath = $frontendCoreFeatureExpansionPath
      Dependencies = @("frontend_source_graph", "frontend_invocation_lock")
      RequiresNativeBinaries = $true
    }
    [pscustomobject]@{
      Name = "frontend_edge_compat"
      Family = "closeout-derived"
      OutputPath = $frontendEdgeCompatPath
      Dependencies = @("frontend_core_feature_expansion")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_edge_robustness"
      Family = "closeout-derived"
      OutputPath = $frontendEdgeRobustnessPath
      Dependencies = @("frontend_edge_compat")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_diagnostics_hardening"
      Family = "closeout-derived"
      OutputPath = $frontendDiagnosticsHardeningPath
      Dependencies = @("frontend_edge_robustness")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_recovery_determinism_hardening"
      Family = "closeout-derived"
      OutputPath = $frontendRecoveryDeterminismHardeningPath
      Dependencies = @("frontend_diagnostics_hardening")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_conformance_matrix"
      Family = "closeout-derived"
      OutputPath = $frontendConformanceMatrixPath
      Dependencies = @("frontend_recovery_determinism_hardening")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_conformance_corpus"
      Family = "closeout-derived"
      OutputPath = $frontendConformanceCorpusPath
      Dependencies = @("frontend_conformance_matrix")
      RequiresNativeBinaries = $false
    }
    [pscustomobject]@{
      Name = "frontend_integration_closeout"
      Family = "closeout-derived"
      OutputPath = $frontendIntegrationCloseoutPath
      Dependencies = @("frontend_conformance_corpus")
      RequiresNativeBinaries = $false
    }
  )
}

function Get-SelectedPacketDefinitions {
  param(
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions
  )

  switch ($Mode) {
    "contracts-source" {
      return @($PacketDefinitions | Where-Object { $_.Family -eq "source-derived" })
    }
    "contracts-binary" {
      return @($PacketDefinitions | Where-Object { $_.Family -in @("source-derived", "binary-derived") })
    }
    "contracts-closeout" {
      return @($PacketDefinitions | Where-Object { $_.Family -in @("source-derived", "binary-derived", "closeout-derived") })
    }
    "contracts-all" {
      return @($PacketDefinitions)
    }
    "full" {
      return @($PacketDefinitions)
    }
    default {
      return @()
    }
  }
}

function Assert-FrontendPacketPrerequisites {
  param(
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions,
    [Parameter(Mandatory = $true)][object[]]$SelectedPacketDefinitions,
    [Parameter(Mandatory = $true)][string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)][string]$CapiBinaryPath
  )

  $selectedNames = @{}
  foreach ($definition in $SelectedPacketDefinitions) {
    $selectedNames[$definition.Name] = $true
  }

  foreach ($definition in $SelectedPacketDefinitions) {
    if ($definition.RequiresNativeBinaries) {
      foreach ($binaryPath in @($NativeBinaryPath, $CapiBinaryPath)) {
        if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
          throw ("contract artifact family '{0}' requires existing native binaries: {1}" -f $definition.Family, $binaryPath)
        }
      }
    }
    foreach ($dependencyName in $definition.Dependencies) {
      if ($selectedNames.ContainsKey($dependencyName)) {
        continue
      }
      $dependency = $PacketDefinitions | Where-Object { $_.Name -eq $dependencyName } | Select-Object -First 1
      if ($null -eq $dependency) {
        throw ("frontend contract dependency not declared: " + $dependencyName)
      }
      if (!(Test-Path -LiteralPath $dependency.OutputPath -PathType Leaf)) {
        throw ("packet '{0}' requires existing dependency output '{1}' at {2}" -f $definition.Name, $dependencyName, $dependency.OutputPath)
      }
    }
  }
}

function Invoke-FrontendPacketGeneration {
  param(
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)][string]$CapiBinaryPath,
    [Parameter(Mandatory = $true)][object[]]$Modules,
    [Parameter(Mandatory = $true)][string[]]$SharedSources
  )

  $selectedPacketDefinitions = @(Get-SelectedPacketDefinitions -Mode $Mode -PacketDefinitions $PacketDefinitions)
  if ($selectedPacketDefinitions.Count -eq 0) {
    Write-BuildStep "artifact_generation_mode=none"
    return
  }

  Assert-FrontendPacketPrerequisites `
    -PacketDefinitions $PacketDefinitions `
    -SelectedPacketDefinitions $selectedPacketDefinitions `
    -NativeBinaryPath $NativeBinaryPath `
    -CapiBinaryPath $CapiBinaryPath

  $selectedFamilies = @($selectedPacketDefinitions | ForEach-Object { $_.Family } | Select-Object -Unique)
  Write-BuildStep ("artifact_generation_mode=" + $Mode)
  Write-BuildStep ("artifact_generation_families=" + ($selectedFamilies -join ","))
  Write-BuildStep "artifact_generation_start=frontend_contract_artifacts"

  foreach ($definition in $selectedPacketDefinitions) {
    Write-BuildStep ("artifact_generation_packet=" + $definition.Name + ";family=" + $definition.Family)
    switch ($definition.Name) {
      "frontend_source_graph" {
        Write-FrontendModuleScaffoldArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendScaffoldPath `
          -Modules $Modules `
          -SharedSources $SharedSources `
          -BinaryTargets @(
            (Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $NativeBinaryPath),
            (Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $CapiBinaryPath)
          )
      }
      "frontend_invocation_lock" {
        Write-FrontendInvocationLockArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendInvocationLockPath `
          -NativeBinaryPath $NativeBinaryPath `
          -CapiBinaryPath $CapiBinaryPath `
          -FrontendScaffoldPath $frontendScaffoldPath
      }
      "frontend_core_feature_expansion" {
        Write-FrontendCoreFeatureExpansionArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendCoreFeatureExpansionPath `
          -Modules $Modules `
          -SharedSources $SharedSources `
          -NativeBinaryPath $NativeBinaryPath `
          -CapiBinaryPath $CapiBinaryPath `
          -FrontendScaffoldPath $frontendScaffoldPath `
          -FrontendInvocationLockPath $frontendInvocationLockPath
      }
      "frontend_edge_compat" {
        Write-FrontendEdgeCompatibilityArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendEdgeCompatPath `
          -FrontendCoreFeatureExpansionPath $frontendCoreFeatureExpansionPath
      }
      "frontend_edge_robustness" {
        Write-FrontendEdgeRobustnessArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendEdgeRobustnessPath `
          -FrontendEdgeCompatibilityPath $frontendEdgeCompatPath
      }
      "frontend_diagnostics_hardening" {
        Write-FrontendDiagnosticsHardeningArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendDiagnosticsHardeningPath `
          -FrontendEdgeRobustnessPath $frontendEdgeRobustnessPath
      }
      "frontend_recovery_determinism_hardening" {
        Write-FrontendRecoveryDeterminismHardeningArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendRecoveryDeterminismHardeningPath `
          -FrontendDiagnosticsHardeningPath $frontendDiagnosticsHardeningPath
      }
      "frontend_conformance_matrix" {
        Write-FrontendConformanceMatrixArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendConformanceMatrixPath `
          -FrontendRecoveryDeterminismHardeningPath $frontendRecoveryDeterminismHardeningPath
      }
      "frontend_conformance_corpus" {
        Write-FrontendConformanceCorpusArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendConformanceCorpusPath `
          -FrontendConformanceMatrixPath $frontendConformanceMatrixPath
      }
      "frontend_integration_closeout" {
        Write-FrontendIntegrationCloseoutArtifact `
          -RepoRoot $RepoRoot `
          -OutputPath $frontendIntegrationCloseoutPath `
          -FrontendConformanceCorpusPath $frontendConformanceCorpusPath
      }
      default {
        throw ("unhandled frontend contract artifact definition: " + $definition.Name)
      }
    }
  }

  Write-BuildStep "artifact_generation_done=frontend_contract_artifacts"
}

function Get-BuildFingerprint {
  param(
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$NinjaTool,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDir,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDir,
    [Parameter(Mandatory = $true)][string]$SourceDir
  )

  return [ordered]@{
    schema_version = 1
    generator = "Ninja"
    cmake = $CmakeTool
    ninja = $NinjaTool
    clangxx = $Clangxx
    llvm_root = $LlvmRoot
    llvm_include_dir = $IncludeDir
    libclang = $Libclang
    build_dir = $BuildDir
    source_dir = $SourceDir
    runtime_output_dir = $RuntimeOutputDir
    library_output_dir = $LibraryOutputDir
    build_type = "Release"
    direct_object_emission = $true
    warning_parity = $true
  }
}

function Test-BuildFingerprintMatch {
  param(
    [Parameter(Mandatory = $true)][hashtable]$ExpectedFingerprint,
    [Parameter(Mandatory = $true)][string]$FingerprintPath
  )

  if (!(Test-Path -LiteralPath $FingerprintPath -PathType Leaf)) {
    return $false
  }

  try {
    $actual = Get-Content -LiteralPath $FingerprintPath -Raw | ConvertFrom-Json -AsHashtable
  } catch {
    return $false
  }

  foreach ($key in $ExpectedFingerprint.Keys) {
    if (!$actual.ContainsKey($key)) {
      return $false
    }
    if ([string]$actual[$key] -ne [string]$ExpectedFingerprint[$key]) {
      return $false
    }
  }

  return $true
}

function Invoke-CMakeConfigure {
  param(
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$NinjaTool,
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$Clangxx,
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$IncludeDir,
    [Parameter(Mandatory = $true)][string]$Libclang,
    [Parameter(Mandatory = $true)][string]$RuntimeOutputDir,
    [Parameter(Mandatory = $true)][string]$LibraryOutputDir,
    [Parameter(Mandatory = $true)][string]$FingerprintPath,
    [Parameter(Mandatory = $true)][hashtable]$Fingerprint,
    [Parameter(Mandatory = $true)][bool]$ForceReconfigure
  )

  $cachePath = Join-Path $BuildDir "CMakeCache.txt"
  $needsConfigure = $ForceReconfigure -or !(Test-Path -LiteralPath $cachePath -PathType Leaf) -or !(Test-BuildFingerprintMatch -ExpectedFingerprint $Fingerprint -FingerprintPath $FingerprintPath)
  if ($needsConfigure) {
    if ($ForceReconfigure) {
      Write-BuildStep "cmake_configure=force-reconfigure"
    } elseif (Test-Path -LiteralPath $cachePath -PathType Leaf) {
      Write-BuildStep "cmake_configure=refresh-fingerprint"
    } else {
      Write-BuildStep "cmake_configure=cold"
    }
    & $CmakeTool `
      -S $SourceDir `
      -B $BuildDir `
      -G Ninja `
      "-DCMAKE_MAKE_PROGRAM=$NinjaTool" `
      "-DCMAKE_CXX_COMPILER=$Clangxx" `
      "-DCMAKE_BUILD_TYPE=Release" `
      "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON" `
      "-DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=ON" `
      "-DOBJC3C_ENABLE_WARNING_PARITY=ON" `
      "-DOBJC3C_LLVM_ROOT=$LlvmRoot" `
      "-DOBJC3C_LLVM_INCLUDE_DIR=$IncludeDir" `
      "-DOBJC3C_LIBCLANG_LIBRARY=$Libclang" `
      "-DOBJC3C_RUNTIME_OUTPUT_DIR=$RuntimeOutputDir" `
      "-DOBJC3C_LIBRARY_OUTPUT_DIR=$LibraryOutputDir"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $Fingerprint | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $FingerprintPath -Encoding utf8
  } else {
    Write-BuildStep "cmake_configure=reuse"
  }
}

function Invoke-CMakeNativeBuild {
  param(
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$BuildDir
  )

  Write-BuildStep "cmake_build_start=native-binaries"
  & $CmakeTool --build $BuildDir --parallel --target objc3c-native objc3c-frontend-c-api-runner objc3_runtime
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-BuildStep "cmake_build_done=native-binaries"
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

function Write-RepoSupercleanSourceOfTruthArtifact {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath,
    [Parameter(Mandatory = $true)]
    [string]$ExecutionMode,
    [Parameter(Mandatory = $true)]
    [string]$CompileCommandsPath,
    [Parameter(Mandatory = $true)]
    [string]$NativeExecutablePath,
    [Parameter(Mandatory = $true)]
    [string]$FrontendCapiRunnerPath,
    [Parameter(Mandatory = $true)]
    [string]$RuntimeLibraryPath,
    [Parameter(Mandatory = $true)]
    [object[]]$FrontendDefinitions
  )

  $payload = [ordered]@{
    contract_id = "objc3c-repo-superclean-source-of-truth/build-wrapper-generated-surface-v1"
    schema_version = 1
    surface_kind = "native-build-wrapper-generated-source-of-truth"
    build_wrapper = "scripts/build_objc3c_native.ps1"
    execution_mode = $ExecutionMode
    implementation_roots = @(
      "native/objc3c",
      "scripts",
      "tests"
    )
    checked_in_doc_sources = @(
      "README.md",
      "CONTRIBUTING.md",
      "showcase",
      "stdlib",
      "site/src",
      "docs/objc3c-native/src",
      "docs/runbooks",
      "package.json"
    )
    generated_checked_in_outputs = @(
      "site/index.md",
      "docs/objc3c-native.md",
      "docs/runbooks/objc3c_public_command_surface.md"
    )
    maintainer_runbooks = @(
      "docs/runbooks/objc3c_maintainer_workflows.md",
      "docs/runbooks/objc3c_developer_tooling.md",
      "docs/runbooks/objc3c_bonus_experiences.md",
      "docs/runbooks/objc3c_performance.md",
      "docs/runbooks/objc3c_performance_governance.md",
      "docs/runbooks/objc3c_packaging_channels.md",
      "docs/runbooks/objc3c_release_foundation.md",
      "docs/runbooks/objc3c_runtime_performance.md",
      "docs/runbooks/objc3c_stress_validation.md",
      "docs/runbooks/objc3c_stdlib_foundation.md",
      "docs/runbooks/objc3c_stdlib_core.md",
      "docs/runbooks/objc3c_stdlib_advanced.md"
    )
    machine_owned_output_roots = @(
      "tmp",
      "artifacts"
    )
    package_entrypoints = [ordered]@{
      build_native = "build:objc3c-native"
      build_contracts = "build:objc3c-native:contracts"
      build_full = "build:objc3c-native:full"
      build_reconfigure = "build:objc3c-native:reconfigure"
      build_site = "build:site"
      check_site = "check:site"
      build_native_docs = "build:docs:native"
      build_public_command_surface = "build:docs:commands"
      build_stdlib = "build:objc3c:stdlib"
      check_stdlib_surface = "check:stdlib:surface"
      inspect_performance_dashboard = "inspect:objc3c:performance-dashboard"
      publish_performance_report = "publish:objc3c:performance-report"
      test_performance_governance = "test:objc3c:performance-governance"
      inspect_release_manifest = "inspect:objc3c:release-manifest"
      publish_release_provenance = "publish:objc3c:release-provenance"
      test_release_foundation = "test:objc3c:release-foundation"
      package_channels = "package:objc3c:channels"
      check_packaging_channels_surface = "check:objc3c:packaging-channels:surface"
      check_packaging_channels_schemas = "check:objc3c:packaging-channels:schemas"
      test_packaging_channels = "test:objc3c:packaging-channels"
      test_packaging_channels_e2e = "test:objc3c:packaging-channels:e2e"
      inspect_runtime_performance = "inspect:objc3c:runtime-performance"
      test_runtime_performance = "test:objc3c:runtime-performance"
      test_runtime_performance_e2e = "test:objc3c:runnable-runtime-performance"
      test_stdlib = "test:stdlib"
      test_stdlib_e2e = "test:stdlib:e2e"
      test_fast = "test:fast"
      test_ci = "test:ci"
      test_docs = "test:docs"
    }
    native_build_outputs = [ordered]@{
      native_executable = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $NativeExecutablePath
      frontend_c_api_runner = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $FrontendCapiRunnerPath
      runtime_library = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $RuntimeLibraryPath
      compile_commands = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath $CompileCommandsPath
    }
    bonus_experience_surfaces = [ordered]@{
      playground = [ordered]@{
        source_roots = @(
          "showcase/auroraBoard/main.objc3",
          "showcase/signalMesh/main.objc3",
          "showcase/patchKit/main.objc3",
          "tests/tooling/fixtures/native/hello.objc3"
        )
        artifact_roots = @(
          "tmp/artifacts/playground",
          "tmp/reports/playground",
          "tmp/artifacts/showcase"
        )
        public_actions = @(
          "materialize-playground-workspace",
          "compile-objc3c",
          "inspect-playground-repro",
          "inspect-compile-observability",
          "trace-compile-stages"
        )
      }
      runtime_inspector = [ordered]@{
        source_roots = @(
          "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
          "scripts/probe_objc3c_llvm_capabilities.py",
          "native/objc3c/src/runtime/objc3_runtime.cpp",
          "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
          "tests/tooling/runtime/block_arc_runtime_abi_probe.cpp",
          "tests/tooling/runtime/task_runtime_hardening_probe.cpp"
        )
        report_roots = @(
          "tmp/reports/objc3c-public-workflow",
          "tmp/reports/developer-tooling"
        )
        public_actions = @(
          "inspect-runtime-inspector",
          "inspect-capability-explorer",
          "benchmark-runtime-inspector",
          "trace-compile-stages",
          "validate-developer-tooling"
        )
      }
      template_and_demo_harness = [ordered]@{
        source_roots = @(
          "scripts/materialize_objc3c_project_template.py",
          "showcase/README.md",
          "showcase/portfolio.json",
          "showcase/tutorial_walkthrough.json",
          "docs/tutorials/getting_started.md",
          "docs/tutorials/build_run_verify.md",
          "docs/tutorials/guided_walkthrough.md"
        )
        report_roots = @(
          "tmp/artifacts/project-template",
          "tmp/reports/project-template",
          "tmp/reports/showcase",
          "tmp/reports/tutorials"
        )
        public_actions = @(
          "materialize-project-template",
          "validate-showcase",
          "validate-runnable-showcase",
          "validate-getting-started"
        )
      }
    }
    bonus_tool_integration_surface = [ordered]@{
      source_of_truth_artifact = "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"
      report_root = "tmp/reports/objc3c-public-workflow"
      package_stage_root = "tmp/pkg/objc3c-native-runnable-toolchain"
      portfolio_contract = "showcase/portfolio.json"
      guided_walkthrough_manifest = "showcase/tutorial_walkthrough.json"
      public_actions = @(
        "inspect-bonus-tool-integration",
        "materialize-project-template",
        "materialize-playground-workspace",
        "benchmark-runtime-inspector",
        "validate-showcase",
        "validate-runnable-showcase",
        "validate-getting-started",
        "package-runnable-toolchain"
      )
    }
    performance_benchmark_surface = [ordered]@{
      benchmark_portfolio = "tests/tooling/fixtures/performance/benchmark_portfolio.json"
      measurement_policy = "tests/tooling/fixtures/performance/measurement_policy.json"
      benchmark_parameters = "tests/tooling/fixtures/performance/benchmark_parameters.json"
      comparative_baseline_manifest = "tests/tooling/fixtures/performance/comparative_baseline_manifest.json"
      telemetry_schema = "schemas/objc3c-performance-telemetry-v1.schema.json"
      source_roots = @(
        "showcase/auroraBoard/main.objc3",
        "showcase/signalMesh/main.objc3",
        "showcase/patchKit/main.objc3",
        "tests/tooling/fixtures/performance/baselines/objc2_reference_workload.m",
        "tests/tooling/fixtures/performance/baselines/swift_reference_workload.swift",
        "tests/tooling/fixtures/performance/baselines/cpp_reference_workload.cpp"
      )
      report_roots = @(
        "tmp/artifacts/performance",
        "tmp/reports/performance",
        "tmp/pkg/objc3c-native-runnable-toolchain"
      )
      public_actions = @(
        "benchmark-performance",
        "benchmark-comparative-baselines",
        "validate-runnable-performance",
        "package-runnable-toolchain"
      )
    }
    runtime_performance_surface = [ordered]@{
      runbook = "docs/runbooks/objc3c_runtime_performance.md"
      source_surface_contract = "tests/tooling/fixtures/runtime_performance/source_surface.json"
      workload_manifest = "tests/tooling/fixtures/runtime_performance/workload_manifest.json"
      artifact_surface_contract = "tests/tooling/fixtures/runtime_performance/artifact_surface.json"
      optimization_policy = "tests/tooling/fixtures/runtime_performance/optimization_policy.json"
      telemetry_schema = "schemas/objc3c-runtime-performance-telemetry-v1.schema.json"
      source_readme = "tests/tooling/fixtures/runtime_performance/README.md"
      source_roots = @(
        "native/objc3c/src/runtime/objc3_runtime.cpp",
        "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
        "scripts/benchmark_objc3c_runtime_performance.py",
        "scripts/check_objc3c_runtime_acceptance.py",
        "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp",
        "tests/tooling/runtime/live_dispatch_fast_path_probe.cpp",
        "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        "tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp"
      )
      report_roots = @(
        "tmp/artifacts/runtime-performance",
        "tmp/reports/runtime-performance",
        "tmp/pkg/objc3c-native-runnable-toolchain"
      )
      public_actions = @(
        "inspect-runtime-inspector",
        "benchmark-runtime-performance",
        "validate-runtime-performance",
        "validate-runnable-runtime-performance",
        "package-runnable-toolchain"
      )
    }
    compiler_throughput_surface = [ordered]@{
      runbook = "docs/runbooks/objc3c_compiler_throughput.md"
      source_surface_contract = "tests/tooling/fixtures/compiler_throughput/source_surface.json"
      workload_manifest = "tests/tooling/fixtures/compiler_throughput/workload_manifest.json"
      validation_tier_map = "tests/tooling/fixtures/compiler_throughput/validation_tier_map.json"
      optimization_policy = "tests/tooling/fixtures/compiler_throughput/optimization_policy.json"
      artifact_surface_contract = "tests/tooling/fixtures/compiler_throughput/artifact_surface.json"
      summary_schema = "schemas/objc3c-compiler-throughput-summary-v1.schema.json"
      source_roots = @(
        "scripts/check_objc3c_native_perf_budget.ps1",
        "scripts/check_objc3c_native_execution_smoke.ps1",
        "scripts/check_objc3c_native_recovery_contract.ps1",
        "scripts/check_objc3c_execution_replay_proof.ps1",
        "scripts/run_objc3c_native_fixture_matrix.ps1",
        "scripts/check_objc3c_negative_fixture_expectations.ps1",
        "scripts/build_objc3c_native_docs.py",
        "scripts/render_objc3c_public_command_surface.py"
      )
      report_roots = @(
        "tmp/artifacts/objc3c-native/perf-budget",
        "tmp/reports/compiler-throughput",
        "tmp/pkg/objc3c-native-runnable-toolchain"
      )
      public_actions = @(
        "benchmark-compiler-throughput",
        "validate-compiler-throughput",
        "validate-runnable-compiler-throughput",
        "package-runnable-toolchain"
      )
    }
    performance_governance_surface = [ordered]@{
      runbook = "docs/runbooks/objc3c_performance_governance.md"
      source_surface_contract = "tests/tooling/fixtures/performance_governance/source_surface.json"
      budget_model = "tests/tooling/fixtures/performance_governance/budget_model.json"
      claim_policy = "tests/tooling/fixtures/performance_governance/claim_policy.json"
      breach_triage_policy = "tests/tooling/fixtures/performance_governance/breach_triage_policy.json"
      lab_policy = "tests/tooling/fixtures/performance_governance/lab_policy.json"
      waiver_registry = "tests/tooling/fixtures/performance_governance/waivers.json"
      workflow_surface = "tests/tooling/fixtures/performance_governance/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/performance_governance/schema_surface.json"
      dashboard_schema = "schemas/objc3c-performance-dashboard-summary-v1.schema.json"
      public_report_schema = "schemas/objc3c-performance-public-report-v1.schema.json"
      source_roots = @(
        "scripts/check_performance_governance_source_surface.py",
        "scripts/check_performance_governance_schema_surface.py",
        "scripts/build_objc3c_performance_dashboard.py",
        "scripts/publish_objc3c_performance_report.py",
        "scripts/check_objc3c_performance_governance_integration.py",
        "scripts/check_objc3c_performance_governance_end_to_end.py"
      )
      report_roots = @(
        "tmp/reports/performance-governance",
        "tmp/artifacts/performance-governance",
        "tmp/reports/objc3c-public-workflow"
      )
      public_actions = @(
        "check-performance-governance-surface",
        "check-performance-governance-schema-surface",
        "build-performance-dashboard",
        "publish-performance-report",
        "validate-performance-governance",
        "validate-performance-governance-integration",
        "validate-performance-governance-end-to-end"
      )
    }
    release_foundation_surface = [ordered]@{
      runbook = "docs/runbooks/objc3c_release_foundation.md"
      source_surface_contract = "tests/tooling/fixtures/release_foundation/source_surface.json"
      artifact_taxonomy = "tests/tooling/fixtures/release_foundation/artifact_taxonomy.json"
      distribution_trust_model = "tests/tooling/fixtures/release_foundation/distribution_trust_model.json"
      distribution_audit = "tests/tooling/fixtures/release_foundation/distribution_audit.json"
      reproducibility_policy = "tests/tooling/fixtures/release_foundation/reproducibility_policy.json"
      release_payload_policy = "tests/tooling/fixtures/release_foundation/release_payload_policy.json"
      provenance_policy = "tests/tooling/fixtures/release_foundation/provenance_policy.json"
      workflow_surface = "tests/tooling/fixtures/release_foundation/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/release_foundation/schema_surface.json"
      release_manifest_schema = "schemas/objc3c-release-manifest-v1.schema.json"
      release_sbom_schema = "schemas/objc3c-release-sbom-v1.schema.json"
      release_attestation_schema = "schemas/objc3c-release-attestation-v1.schema.json"
      source_roots = @(
        "scripts/check_release_foundation_source_surface.py",
        "scripts/check_release_foundation_schema_surface.py",
        "scripts/build_objc3c_release_manifest.py",
        "scripts/publish_objc3c_release_provenance.py",
        "scripts/check_objc3c_release_foundation_integration.py",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "scripts/check_release_evidence.py"
      )
      report_roots = @(
        "tmp/reports/release-foundation",
        "tmp/artifacts/release-foundation",
        "tmp/pkg/objc3c-release-foundation"
      )
      public_actions = @(
        "check-release-foundation-surface",
        "check-release-foundation-schema-surface",
        "build-release-manifest",
        "publish-release-provenance",
        "validate-release-foundation"
      )
    }
    packaging_channels_surface = [ordered]@{
      runbook = "docs/runbooks/objc3c_packaging_channels.md"
      source_surface_contract = "tests/tooling/fixtures/packaging_channels/source_surface.json"
      supported_platforms = "tests/tooling/fixtures/packaging_channels/supported_platforms.json"
      installer_policy = "tests/tooling/fixtures/packaging_channels/installer_policy.json"
      metadata_surface = "tests/tooling/fixtures/packaging_channels/metadata_surface.json"
      workflow_surface = "tests/tooling/fixtures/packaging_channels/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/packaging_channels/schema_surface.json"
      package_channels_manifest_schema = "schemas/objc3c-package-channels-manifest-v1.schema.json"
      install_receipt_schema = "schemas/objc3c-package-install-receipt-v1.schema.json"
      source_roots = @(
        "scripts/check_packaging_channels_source_surface.py",
        "scripts/check_packaging_channels_schema_surface.py",
        "scripts/build_objc3c_package_channels.py",
        "scripts/check_objc3c_packaging_channels_integration.py",
        "scripts/check_objc3c_packaging_channels_end_to_end.py",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "scripts/build_objc3c_release_manifest.py",
        "scripts/publish_objc3c_release_provenance.py"
      )
      report_roots = @(
        "tmp/reports/package-channels",
        "tmp/artifacts/package-channels",
        "tmp/pkg/objc3c-package-channels"
      )
      public_actions = @(
        "check-packaging-channels-surface",
        "check-packaging-channels-schema-surface",
        "build-package-channels",
        "validate-packaging-channels",
        "validate-packaging-channels-end-to-end"
      )
    }
    stress_validation_surface = [ordered]@{
      source_surface_contract = "tests/tooling/fixtures/stress/source_surface.json"
      artifact_surface_contract = "tests/tooling/fixtures/stress/artifact_surface.json"
      source_readme = "tests/tooling/fixtures/stress/README.md"
      safety_policy = "tests/tooling/fixtures/stress/safety_policy.json"
      runbook = "docs/runbooks/objc3c_stress_validation.md"
      source_check_script = "scripts/check_stress_source_surface.py"
      checked_in_roots = @(
        "tests/tooling/fixtures/stress",
        "tests/tooling/fixtures/native",
        "tests/tooling/fixtures/objc3c",
        "tests/tooling/fixtures/parser_conformance_corpus",
        "tests/conformance"
      )
      source_family_ids = @(
        "parser-sema-fuzz",
        "lowering-runtime-stress",
        "mixed-module-differential",
        "replay-backed-contracts"
      )
    }
    stdlib_foundation_surface = [ordered]@{
      workspace_contract = "stdlib/workspace.json"
      module_inventory = "stdlib/module_inventory.json"
      stability_policy = "stdlib/stability_policy.json"
      package_surface = "stdlib/package_surface.json"
      core_architecture = "stdlib/core_architecture.json"
      advanced_architecture = "stdlib/advanced_architecture.json"
      semantic_policy = "stdlib/semantic_policy.json"
      lowering_import_surface = "stdlib/lowering_import_surface.json"
      advanced_helper_package_surface = "stdlib/advanced_helper_package_surface.json"
      source_roots = @(
        "stdlib/README.md",
        "stdlib/advanced_helper_package_surface.json",
        "stdlib/modules/objc3.core/module.objc3",
        "stdlib/modules/objc3.errors/module.objc3",
        "stdlib/modules/objc3.concurrency/module.objc3",
        "stdlib/modules/objc3.keypath/module.objc3",
        "stdlib/modules/objc3.system/module.objc3"
      )
      report_roots = @(
        "tmp/artifacts/stdlib",
        "tmp/reports/stdlib",
        "tmp/pkg/objc3c-native-runnable-toolchain"
      )
      public_actions = @(
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-foundation",
        "validate-stdlib-advanced",
        "validate-runnable-stdlib-advanced",
        "validate-runnable-stdlib-foundation",
        "package-runnable-toolchain"
      )
    }
    stdlib_program_surface = [ordered]@{
      program_contract = "stdlib/program_surface.json"
      stdlib_readme = "stdlib/README.md"
      runbook = "docs/runbooks/objc3c_stdlib_program.md"
      site_entry = "site/src/index.body.md"
      publish_inputs = @(
        "stdlib/README.md",
        "docs/runbooks/objc3c_stdlib_program.md",
        "docs/tutorials/README.md",
        "docs/tutorials/getting_started.md",
        "docs/tutorials/objc2_swift_cpp_comparison.md",
        "showcase/README.md",
        "showcase/portfolio.json",
        "showcase/tutorial_walkthrough.json",
        "site/src/index.body.md"
      )
      report_roots = @(
        "tmp/artifacts/stdlib",
        "tmp/reports/stdlib",
        "tmp/pkg/objc3c-native-runnable-toolchain"
      )
      workflow_surface = [ordered]@{
        report_root = "tmp/reports/stdlib"
        showcase_report_root = "tmp/reports/showcase"
        tutorial_report_root = "tmp/reports/tutorials"
        integration_entrypoint = "validate-stdlib-program"
        packaged_validation_entrypoint = "validate-runnable-stdlib-program"
        integration_actions = @(
          "check-documentation-surface",
          "validate-getting-started",
          "validate-showcase",
          "validate-stdlib-foundation",
          "inspect-capability-explorer"
        )
        release_actions = @(
          "validate-runnable-showcase",
          "validate-runnable-stdlib-foundation",
          "package-runnable-toolchain"
        )
      }
      public_actions = @(
        "check-documentation-surface",
        "check-showcase-surface",
        "validate-getting-started",
        "validate-showcase",
        "validate-runnable-showcase",
        "validate-stdlib-foundation",
        "validate-runnable-stdlib-foundation",
        "validate-stdlib-program",
        "validate-runnable-stdlib-program",
        "inspect-capability-explorer",
        "package-runnable-toolchain"
      )
    }
    conformance_corpus_surface = [ordered]@{
      corpus_contract = "tests/conformance/corpus_surface.json"
      suite_readme = "tests/conformance/README.md"
      coverage_map = "tests/conformance/COVERAGE_MAP.md"
      runbook = "docs/runbooks/objc3c_conformance_corpus.md"
      longitudinal_manifest = "tests/conformance/longitudinal_suites.json"
      report_roots = @(
        "tmp/artifacts/conformance",
        "tmp/reports/conformance",
        "tmp/pkg/objc3c-native-runnable-toolchain"
      )
      workflow_surface = [ordered]@{
        report_root = "tmp/reports/conformance"
        artifact_root = "tmp/artifacts/conformance"
        package_stage_root = "tmp/pkg/objc3c-native-runnable-toolchain"
        surface_check_script = "scripts/check_conformance_corpus_surface.py"
        coverage_index_script = "scripts/generate_conformance_corpus_index.py"
        legacy_suite_gate_script = "scripts/check_conformance_suite.ps1"
        coverage_map = "tests/conformance/COVERAGE_MAP.md"
        longitudinal_suite_manifest = "tests/conformance/longitudinal_suites.json"
      }
    }
    frontend_contract_artifacts = @(
      $FrontendDefinitions | ForEach-Object {
        [ordered]@{
          name = [string]$_.Name
          family = [string]$_.Family
          artifact_path = Get-RepoRelativePath -RootPath $RepoRoot -TargetPath ([string]$_.OutputPath)
        }
      }
    )
    explicit_non_goals = @(
      "no milestone-coded command aliases",
      "no secondary source-of-truth directories",
      "no generated-output hand edits"
    )
  }

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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
    throw "frontend source graph module metadata is invalid"
    }
    $modulePayload.Add([ordered]@{
      name = $name
      source_count = $sources.Count
      sources = $sources
    })
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
    schema_version = 1
    module_count = $modulePayload.Count
    shared_source_count = $SharedSources.Count
    modules = $modulePayload.ToArray()
    shared_sources = $SharedSources
    binary_targets = $BinaryTargets
  }

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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
    throw "frontend source graph missing for invocation lock artifact: $FrontendScaffoldPath"
  }

  try {
    $scaffoldPayload = Get-Content -LiteralPath $FrontendScaffoldPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend source graph is not valid JSON for invocation lock artifact: $FrontendScaffoldPath"
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$scaffoldPayload.contract_id -ne $expectedScaffoldContractId) {
    throw "frontend source graph contract id mismatch for invocation lock artifact: $FrontendScaffoldPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
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

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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
    throw "frontend source graph missing for core feature expansion artifact: $FrontendScaffoldPath"
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
    throw "frontend source graph is not valid JSON for core feature expansion artifact: $FrontendScaffoldPath"
  }

  try {
    $invocationLockPayload = Get-Content -LiteralPath $FrontendInvocationLockPath -Raw | ConvertFrom-Json
  } catch {
    throw "frontend invocation lock is not valid JSON for core feature expansion artifact: $FrontendInvocationLockPath"
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-modular-scaffold-v1"
  if ([string]$scaffoldPayload.contract_id -ne $expectedScaffoldContractId) {
    throw "frontend source graph contract id mismatch for core feature expansion artifact: $FrontendScaffoldPath"
  }
  $expectedInvocationLockContractId = "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
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
    contract_id = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
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

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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

  $expectedCoreFeatureContractId = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
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
    contract_id = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCoreFeatureContractId
      "objc3c-frontend-build-invocation-manifest-guard/parser_build-manifest-guard-v1"
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

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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

  $expectedEdgeCompatContractId = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
  if ([string]$edgeCompatPayload.contract_id -ne $expectedEdgeCompatContractId) {
    throw "frontend edge compatibility contract id mismatch for edge robustness artifact: $FrontendEdgeCompatibilityPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeCompatContractId
      "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-core-feature-expansion-v1"
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

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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

  $expectedEdgeRobustnessContractId = "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
  if ([string]$edgeRobustnessPayload.contract_id -ne $expectedEdgeRobustnessContractId) {
    throw "frontend edge robustness contract id mismatch for diagnostics hardening artifact: $FrontendEdgeRobustnessPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedEdgeRobustnessContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
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

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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

  $expectedDiagnosticsContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-diagnostics-hardening-v1"
  if ([string]$diagnosticsPayload.contract_id -ne $expectedDiagnosticsContractId) {
    throw "frontend diagnostics hardening contract id mismatch for recovery determinism hardening artifact: $FrontendDiagnosticsHardeningPath"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedDiagnosticsContractId
      "objc3c-frontend-build-invocation-edge-robustness/parser_build-edge-robustness-v1"
    )
    cache_determinism = [ordered]@{
      fail_closed_exit_code = 2
      entry_contract_id = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
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

  Write-JsonArtifactFile -OutputPath $OutputPath -Payload $payload -Depth 8
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

  $expectedRecoveryContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
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
    contract_id = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedRecoveryContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
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

  $expectedMatrixContractId = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
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
    $compileArgs.Add("tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3")
    $compileArgs.Add("--out-dir=tmp/reports/parser_build/M226-D010/out")
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/M226-D010/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/M226-D010/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/M226-D010/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/M226-D010/out"
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
        "tests/tooling/fixtures/parser_conformance_corpus/accept_void_pointer_param.objc3"
        "--out-dir=tmp/reports/parser_build/M226-D010/out"
        "--llvm-capabilities-summary=tmp/artifacts/objc3c-native/llvm_capabilities_summary.json"
        "--objc3-route-backend-from-capabilities"
        "--objc3-route-backend-from-capabilities"
      )
    }
  )

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedMatrixContractId
      "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-edge-compat-completion-v1"
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

  $expectedCorpusContractId = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-conformance-corpus-v1"
  if ([string]$corpusPayload.contract_id -ne $expectedCorpusContractId) {
    throw "frontend conformance corpus contract id mismatch for integration closeout artifact: $FrontendConformanceCorpusPath"
  }

  $acceptanceCount = [int]$corpusPayload.acceptance_corpus_count
  $rejectionCount = [int]$corpusPayload.rejection_corpus_count
  if ($acceptanceCount -le 0 -or $rejectionCount -le 0) {
    throw "frontend conformance corpus must provide non-empty acceptance and rejection coverage for integration closeout"
  }

  $payload = [ordered]@{
    contract_id = "objc3c-frontend-build-invocation-integration-closeout/parser_build-integration-closeout-v1"
    schema_version = 1
    depends_on_contract_ids = @(
      $expectedCorpusContractId
      "objc3c-frontend-build-invocation-conformance-matrix/parser_build-conformance-matrix-v1"
      "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-recovery-determinism-hardening-v1"
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
$frontendScaffoldPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_source_graph.json"
$frontendInvocationLockPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
$frontendCoreFeatureExpansionPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
$frontendEdgeCompatPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
$frontendEdgeRobustnessPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
$frontendDiagnosticsHardeningPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
$frontendRecoveryDeterminismHardeningPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
$frontendConformanceMatrixPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
$frontendConformanceCorpusPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
$frontendIntegrationCloseoutPath = Join-Path $repoRoot "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
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

$frontendPacketDefinitions = @(Get-FrontendPacketDefinitions)

$buildFingerprint = Get-BuildFingerprint `
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
  Invoke-CMakeConfigure `
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

  Invoke-CMakeNativeBuild `
    -CmakeTool $cmakeTool `
    -BuildDir $tmpOutDir

  if (!(Test-Path -LiteralPath $outExe -PathType Leaf)) { throw "native binary missing after CMake/Ninja build: $outExe" }
  if (!(Test-Path -LiteralPath $outCapiExe -PathType Leaf)) { throw "c-api runner missing after CMake/Ninja build: $outCapiExe" }
  if (!(Test-Path -LiteralPath $outRuntimeLib -PathType Leaf)) { throw "runtime library missing after CMake/Ninja build: $outRuntimeLib" }
  if (!(Test-Path -LiteralPath $compileCommandsPath -PathType Leaf)) { throw "compile_commands.json missing after CMake/Ninja configure: $compileCommandsPath" }

  Write-BuildStep ("artifact_ready=objc3c-native -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
  Write-BuildStep ("artifact_ready=objc3c-frontend-c-api-runner -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
  Write-BuildStep ("artifact_ready=objc3_runtime -> " + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
  Write-BuildStep ("compile_commands=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $compileCommandsPath))
} else {
  Write-BuildStep "cmake_build_skip=native-binaries"
}

Invoke-FrontendPacketGeneration `
  -Mode $ExecutionMode `
  -PacketDefinitions $frontendPacketDefinitions `
  -RepoRoot $repoRoot `
  -NativeBinaryPath $outExe `
  -CapiBinaryPath $outCapiExe `
  -Modules $frontendModules `
  -SharedSources $sharedSources

Write-RepoSupercleanSourceOfTruthArtifact `
  -RepoRoot $repoRoot `
  -OutputPath $repoSupercleanSurfacePath `
  -ExecutionMode $ExecutionMode `
  -CompileCommandsPath $compileCommandsPath `
  -NativeExecutablePath $outExe `
  -FrontendCapiRunnerPath $outCapiExe `
  -RuntimeLibraryPath $outRuntimeLib `
  -FrontendDefinitions $frontendPacketDefinitions

if (Test-Path -LiteralPath $outExe -PathType Leaf) {
  Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outExe))
}
if (Test-Path -LiteralPath $outCapiExe -PathType Leaf) {
  Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outCapiExe))
}
if (Test-Path -LiteralPath $outRuntimeLib -PathType Leaf) {
  Write-Output ("built=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $outRuntimeLib))
}
foreach ($packetDefinition in $frontendPacketDefinitions) {
  if (Test-Path -LiteralPath $packetDefinition.OutputPath -PathType Leaf) {
    Write-Output ($packetDefinition.Name + "=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $packetDefinition.OutputPath))
  }
}
Write-Output ("repo_superclean_surface=" + (Get-RepoRelativePath -RootPath $repoRoot -TargetPath $repoSupercleanSurfacePath))
