$ErrorActionPreference = "Stop"

function Test-Objc3cNativeHostIsWindows {
  return [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::Windows
  )
}

function Test-Objc3cNativeHostIsDarwin {
  return [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::OSX
  )
}

function Test-Objc3cNativeHostIsLinux {
  return [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::Linux
  )
}

function Get-Objc3cNativeHostArchitecture {
  $architecture = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString().ToLowerInvariant()
  if ($architecture -in @("x64", "x86_64", "amd64")) {
    return "x64"
  }
  if ($architecture -in @("arm64", "aarch64")) {
    return "arm64"
  }
  return $architecture
}

function Get-Objc3cNativeHostPlatformId {
  $architecture = Get-Objc3cNativeHostArchitecture
  if (Test-Objc3cNativeHostIsWindows) {
    return "windows-$architecture"
  }
  if (Test-Objc3cNativeHostIsDarwin) {
    return "darwin-$architecture"
  }
  if (Test-Objc3cNativeHostIsLinux) {
    return "linux-$architecture"
  }
  return "unknown-$architecture"
}

function Get-Objc3cNativeExecutableExtension {
  if (Test-Objc3cNativeHostIsWindows) {
    return ".exe"
  }
  return ""
}

function Get-Objc3cNativeExecutableFileName {
  param([Parameter(Mandatory = $true)][string]$BaseName)

  return $BaseName + (Get-Objc3cNativeExecutableExtension)
}

function Get-Objc3cNativeRuntimeLibraryFileName {
  if (Test-Objc3cNativeHostIsWindows) {
    return "objc3_runtime.lib"
  }
  if (Test-Objc3cNativeHostIsDarwin) {
    return "libobjc3-runtime.dylib"
  }
  return "libobjc3-runtime.so"
}

function Get-Objc3cNativeRuntimeLibraryKind {
  if (Test-Objc3cNativeHostIsWindows) {
    return "static-archive"
  }
  return "shared-library"
}

function Get-Objc3cNativeObjectFormat {
  if (Test-Objc3cNativeHostIsWindows) {
    return "COFF"
  }
  if (Test-Objc3cNativeHostIsDarwin) {
    return "Mach-O"
  }
  return "ELF"
}

function Get-Objc3cNativeDebugFormat {
  if (Test-Objc3cNativeHostIsWindows) {
    return "CodeView/PDB"
  }
  if (Test-Objc3cNativeHostIsDarwin) {
    return "DWARF/dSYM"
  }
  return "DWARF"
}

function Get-Objc3cNativeTargetTriple {
  $platformId = Get-Objc3cNativeHostPlatformId
  if ($platformId -eq "windows-x64") {
    return "x86_64-pc-windows-msvc"
  }
  if ($platformId -eq "linux-x64") {
    return "x86_64-unknown-linux-gnu"
  }
  if ($platformId -eq "darwin-arm64") {
    return "aarch64-apple-darwin"
  }
  if ($platformId -eq "darwin-x64") {
    return "x86_64-apple-darwin"
  }
  return $platformId
}

function Get-Objc3cNativePackageArtifactRelativePaths {
  return [pscustomobject]@{
    NativeExecutable = "artifacts/bin/" + (Get-Objc3cNativeExecutableFileName -BaseName "objc3c-native")
    CapiRunnerExecutable = "artifacts/bin/" + (Get-Objc3cNativeExecutableFileName -BaseName "objc3c-frontend-c-api-runner")
    RuntimeLibrary = "artifacts/lib/" + (Get-Objc3cNativeRuntimeLibraryFileName)
    CompileCommands = "tmp/build-objc3c-native/compile_commands.json"
  }
}

function Get-Objc3cNativeCMakeBuildPaths {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [string]$CleanRoomRoot = "",
    [string]$BuildDir = "",
    [string]$RuntimeOutputDir = "",
    [string]$LibraryOutputDir = ""
  )

  if ($CleanRoomRoot) {
    $cleanRoomFullPath = [System.IO.Path]::GetFullPath($CleanRoomRoot)
    if (!$RuntimeOutputDir) {
      $RuntimeOutputDir = Join-Path $cleanRoomFullPath "artifacts/bin"
    }
    if (!$LibraryOutputDir) {
      $LibraryOutputDir = Join-Path $cleanRoomFullPath "artifacts/lib"
    }
    if (!$BuildDir) {
      $BuildDir = Join-Path $cleanRoomFullPath "build/native"
    }
  }

  $outDir = if ($RuntimeOutputDir) { $RuntimeOutputDir } else { Join-Path $RepoRoot "artifacts/bin" }
  $outLibDir = if ($LibraryOutputDir) { $LibraryOutputDir } else { Join-Path $RepoRoot "artifacts/lib" }
  $tmpOutDir = if ($BuildDir) { $BuildDir } else { Join-Path $RepoRoot "tmp/build-objc3c-native" }
  $nativeExecutableName = Get-Objc3cNativeExecutableFileName -BaseName "objc3c-native"
  $capiRunnerName = Get-Objc3cNativeExecutableFileName -BaseName "objc3c-frontend-c-api-runner"
  $runtimeLibraryName = Get-Objc3cNativeRuntimeLibraryFileName

  return [pscustomobject]@{
    RuntimeOutputDir = $outDir
    LibraryOutputDir = $outLibDir
    NativeExecutable = Join-Path $outDir $nativeExecutableName
    CapiRunnerExecutable = Join-Path $outDir $capiRunnerName
    RuntimeLibrary = Join-Path $outLibDir $runtimeLibraryName
    TargetPlatformId = Get-Objc3cNativeHostPlatformId
    TargetTriple = Get-Objc3cNativeTargetTriple
    ExecutableExtension = Get-Objc3cNativeExecutableExtension
    RuntimeLibraryKind = Get-Objc3cNativeRuntimeLibraryKind
    RuntimeLibraryFileName = $runtimeLibraryName
    ObjectFormat = Get-Objc3cNativeObjectFormat
    DebugFormat = Get-Objc3cNativeDebugFormat
    BuildDir = $tmpOutDir
    CmakeSourceDir = Join-Path $RepoRoot "native/objc3c"
    CompileCommands = Join-Path $tmpOutDir "compile_commands.json"
    BuildFingerprint = Join-Path $tmpOutDir "native_build_backend_fingerprint.json"
  }
}

Export-ModuleMember -Function @(
  "Test-Objc3cNativeHostIsWindows",
  "Test-Objc3cNativeHostIsDarwin",
  "Test-Objc3cNativeHostIsLinux",
  "Get-Objc3cNativeHostArchitecture",
  "Get-Objc3cNativeHostPlatformId",
  "Get-Objc3cNativeExecutableExtension",
  "Get-Objc3cNativeExecutableFileName",
  "Get-Objc3cNativeRuntimeLibraryFileName",
  "Get-Objc3cNativeRuntimeLibraryKind",
  "Get-Objc3cNativeObjectFormat",
  "Get-Objc3cNativeDebugFormat",
  "Get-Objc3cNativeTargetTriple",
  "Get-Objc3cNativePackageArtifactRelativePaths",
  "Get-Objc3cNativeCMakeBuildPaths"
)
