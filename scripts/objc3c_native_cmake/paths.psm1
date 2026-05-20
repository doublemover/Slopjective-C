$ErrorActionPreference = "Stop"

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

Export-ModuleMember -Function @(
  "Get-Objc3cNativeCMakeBuildPaths"
)
