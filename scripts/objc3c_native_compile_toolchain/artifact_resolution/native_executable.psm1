$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Resolve-NativeCompilerExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $configuredNativeExe = [string]$env:OBJC3C_NATIVE_EXECUTABLE
  if (-not [string]::IsNullOrWhiteSpace($configuredNativeExe)) {
    return [System.IO.Path]::GetFullPath($configuredNativeExe)
  }
  return (Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe")
}
