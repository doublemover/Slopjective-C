Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function Resolve-RunnableToolchainPackageEnvironment {
  param(
    [Parameter(Mandatory = $true)][string]$ScriptRoot,
    [string]$PackageRoot = "",
    [Parameter(Mandatory = $true)][string]$ManifestRelativePath
  )

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"

  if (!(Test-Path -LiteralPath $buildScript -PathType Leaf)) {
    throw "runnable toolchain package FAIL: missing build script $buildScript"
  }

  $resolvedPackageRoot = Resolve-PackageRoot -RepoRoot $repoRoot -RequestedRoot $PackageRoot
  $manifestPath = Join-Path $resolvedPackageRoot ($ManifestRelativePath.Replace('/', '\\'))
  $manifestDir = Split-Path -Parent $manifestPath
  New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null

  return [ordered]@{
    RepoRoot = $repoRoot
    BuildScript = $buildScript
    PackageRoot = $resolvedPackageRoot
    ManifestPath = $manifestPath
  }
}

Export-ModuleMember -Function @("Resolve-RunnableToolchainPackageEnvironment")
