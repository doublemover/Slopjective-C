Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "artifact_report_io.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "artifact_report_foundation.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "artifact_report_application.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "artifact_report_operations.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "artifact_report_surfaces.psm1") -Force -DisableNameChecking

function Add-RunnableToolchainPackageManifestSection {
  param(
    [Parameter(Mandatory = $true)]$ManifestPayload,
    [Parameter(Mandatory = $true)]$SectionPayload
  )

  foreach ($key in $SectionPayload.Keys) {
    $ManifestPayload[$key] = $SectionPayload[$key]
  }
}

function New-RunnableToolchainPackageManifestPayload {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [Parameter(Mandatory = $true)][string[]]$StagedRelativePaths
  )

  $surfacePayloads = Get-RunnableToolchainPackageSurfacePayloads -PackageRoot $PackageRoot
  $manifestPayload = [ordered]@{}

  Add-RunnableToolchainPackageManifestSection `
    -ManifestPayload $manifestPayload `
    -SectionPayload (New-RunnableToolchainPackageFoundationManifestSection `
      -RepoRoot $RepoRoot `
      -PackageRoot $PackageRoot `
      -ManifestPath $ManifestPath)
  Add-RunnableToolchainPackageManifestSection `
    -ManifestPayload $manifestPayload `
    -SectionPayload (New-RunnableToolchainPackageApplicationManifestSection)
  Add-RunnableToolchainPackageManifestSection `
    -ManifestPayload $manifestPayload `
    -SectionPayload (New-RunnableToolchainPackageOperationsManifestSection)
  Add-RunnableToolchainPackageManifestSection `
    -ManifestPayload $manifestPayload `
    -SectionPayload (New-RunnableToolchainPackageSurfaceManifestSection `
      -SurfacePayloads $surfacePayloads `
      -StagedRelativePaths $StagedRelativePaths)

  return $manifestPayload
}

function Write-RunnableToolchainPackageManifest {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [Parameter(Mandatory = $true)][string[]]$StagedRelativePaths
  )

  $manifestPayload = New-RunnableToolchainPackageManifestPayload `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -ManifestPath $ManifestPath `
    -StagedRelativePaths $StagedRelativePaths

  $manifestDir = Split-Path -Parent $ManifestPath
  New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null
  $manifestPayload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $ManifestPath -Encoding utf8
  return $manifestPayload
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageManifestPayload",
  "Write-RunnableToolchainPackageManifest"
)
