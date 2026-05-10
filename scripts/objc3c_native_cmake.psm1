$ErrorActionPreference = "Stop"

function Write-Objc3cNativeBuildStep {
  param([Parameter(Mandatory = $true)][string]$Message)

  Write-Host ("[objc3c-native] " + $Message)
}

function Resolve-Objc3cNativeToolchain {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

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
  $nativeSourceRoot = Join-Path $RepoRoot "native/objc3c/src"

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

  return [pscustomobject]@{
    LlvmRoot = $llvmRoot
    Clangxx = $clangxx
    LlvmLibTool = $llvmLibTool
    CmakeTool = $cmakeTool
    NinjaTool = $ninjaTool
    Libclang = $libclang
    IncludeDir = $includeDir
    NativeSourceRoot = $nativeSourceRoot
  }
}

function Get-Objc3cNativeCMakeBuildPaths {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $outDir = Join-Path $RepoRoot "artifacts/bin"
  $outLibDir = Join-Path $RepoRoot "artifacts/lib"
  $tmpOutDir = Join-Path $RepoRoot "tmp/build-objc3c-native"

  return [pscustomobject]@{
    RuntimeOutputDir = $outDir
    LibraryOutputDir = $outLibDir
    NativeExecutable = Join-Path $outDir "objc3c-native.exe"
    CapiRunnerExecutable = Join-Path $outDir "objc3c-frontend-c-api-runner.exe"
    RuntimeLibrary = Join-Path $outLibDir "objc3_runtime.lib"
    BuildDir = $tmpOutDir
    CmakeSourceDir = Join-Path $RepoRoot "native/objc3c"
    CompileCommands = Join-Path $tmpOutDir "compile_commands.json"
    BuildFingerprint = Join-Path $tmpOutDir "native_build_backend_fingerprint.json"
  }
}

function Get-Objc3cNativeBuildFingerprint {
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

function Test-Objc3cNativeBuildFingerprintMatch {
  param(
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$ExpectedFingerprint,
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

function Invoke-Objc3cNativeCMakeConfigure {
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
    [Parameter(Mandatory = $true)][System.Collections.IDictionary]$Fingerprint,
    [Parameter(Mandatory = $true)][bool]$ForceReconfigure
  )

  $cachePath = Join-Path $BuildDir "CMakeCache.txt"
  $needsConfigure = $ForceReconfigure -or !(Test-Path -LiteralPath $cachePath -PathType Leaf) -or !(Test-Objc3cNativeBuildFingerprintMatch -ExpectedFingerprint $Fingerprint -FingerprintPath $FingerprintPath)
  if ($needsConfigure) {
    if ($ForceReconfigure) {
      Write-Objc3cNativeBuildStep "cmake_configure=force-reconfigure"
    } elseif (Test-Path -LiteralPath $cachePath -PathType Leaf) {
      Write-Objc3cNativeBuildStep "cmake_configure=refresh-fingerprint"
    } else {
      Write-Objc3cNativeBuildStep "cmake_configure=cold"
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
    Write-Objc3cNativeBuildStep "cmake_configure=reuse"
  }
}

function Invoke-Objc3cNativeCMakeBuild {
  param(
    [Parameter(Mandatory = $true)][string]$CmakeTool,
    [Parameter(Mandatory = $true)][string]$BuildDir
  )

  Write-Objc3cNativeBuildStep "cmake_build_start=native-binaries"
  & $CmakeTool --build $BuildDir --parallel --target objc3c-native objc3c_tools_frontend_c_api_runner objc3_runtime
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
  Write-Objc3cNativeBuildStep "cmake_build_done=native-binaries"
}

Export-ModuleMember -Function @(
  "Resolve-Objc3cNativeToolchain",
  "Get-Objc3cNativeCMakeBuildPaths",
  "Get-Objc3cNativeBuildFingerprint",
  "Test-Objc3cNativeBuildFingerprintMatch",
  "Invoke-Objc3cNativeCMakeConfigure",
  "Invoke-Objc3cNativeCMakeBuild"
)
