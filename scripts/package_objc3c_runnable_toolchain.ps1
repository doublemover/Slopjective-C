param(
  [string]$PackageRoot = "",
  [string]$ManifestRelativePath = "artifacts/package/objc3c-runnable-toolchain-package.json"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$packageScriptModuleRoot = Join-Path $PSScriptRoot "package_objc3c_runnable_toolchain"
Import-Module (Join-Path $packageScriptModuleRoot "cli_request.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "environment_paths.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "staging_orchestration.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "artifact_report.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $packageScriptModuleRoot "status_rendering.psm1") -Force -DisableNameChecking

$request = New-RunnableToolchainPackageRequest `
  -PackageRoot $PackageRoot `
  -ManifestRelativePath $ManifestRelativePath

$environment = Resolve-RunnableToolchainPackageEnvironment `
  -ScriptRoot $PSScriptRoot `
  -PackageRoot $request.PackageRoot `
  -ManifestRelativePath $request.ManifestRelativePath

$staging = Invoke-RunnableToolchainPackageStaging `
  -RepoRoot $environment.RepoRoot `
  -PackageRoot $environment.PackageRoot `
  -ManifestPath $environment.ManifestPath `
  -BuildScript $environment.BuildScript

$manifestPayload = Write-RunnableToolchainPackageManifest `
  -RepoRoot $environment.RepoRoot `
  -PackageRoot $environment.PackageRoot `
  -ManifestPath $environment.ManifestPath `
  -StagedRelativePaths $staging.StagedRelativePaths

Write-RunnableToolchainPackageStatus -ManifestPayload $manifestPayload
