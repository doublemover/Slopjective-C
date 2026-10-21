$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$nativeCmakeModule = Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) "objc3c_native_cmake.psm1"
Import-Module $nativeCmakeModule -Force -DisableNameChecking

function Resolve-NativeCompilerExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $configuredNativeExe = [string]$env:OBJC3C_NATIVE_EXECUTABLE
  if (-not [string]::IsNullOrWhiteSpace($configuredNativeExe)) {
    return [System.IO.Path]::GetFullPath($configuredNativeExe)
  }
  $coreArtifacts = Get-Objc3cNativePackageArtifactRelativePaths
  return (Join-Path $RepoRoot (Convert-NativeCompilerArtifactRelativePathForHost -RelativePath $coreArtifacts.NativeExecutable))
}

function Resolve-NativeCompilerRuntimeLibraryPath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $coreArtifacts = Get-Objc3cNativePackageArtifactRelativePaths
  return (Join-Path $RepoRoot (Convert-NativeCompilerArtifactRelativePathForHost -RelativePath $coreArtifacts.RuntimeLibrary))
}

function Convert-NativeCompilerArtifactRelativePathForHost {
  param([Parameter(Mandatory = $true)][string]$RelativePath)

  return $RelativePath.Replace('/', [System.IO.Path]::DirectorySeparatorChar)
}
