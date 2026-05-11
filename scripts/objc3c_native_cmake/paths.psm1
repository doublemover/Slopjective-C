$ErrorActionPreference = "Stop"

function Get-Objc3cNativeCMakeBuildPaths {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot
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
