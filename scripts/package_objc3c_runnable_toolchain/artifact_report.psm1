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
    [Parameter(Mandatory = $true)][string[]]$StagedRelativePaths,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  $surfacePayloads = Get-RunnableToolchainPackageSurfacePayloads -PackageRoot $PackageRoot
  $manifestPayload = [ordered]@{}

  Add-RunnableToolchainPackageManifestSection `
    -ManifestPayload $manifestPayload `
    -SectionPayload (New-RunnableToolchainPackageFoundationManifestSection `
      -RepoRoot $RepoRoot `
      -PackageRoot $PackageRoot `
      -ManifestPath $ManifestPath `
      -SanitizerVariant $SanitizerVariant)
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
    [Parameter(Mandatory = $true)][string[]]$StagedRelativePaths,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  $manifestPayload = New-RunnableToolchainPackageManifestPayload `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -ManifestPath $ManifestPath `
    -StagedRelativePaths $StagedRelativePaths `
    -SanitizerVariant $SanitizerVariant

  $manifestDir = Split-Path -Parent $ManifestPath
  New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null
  $manifestPayload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $ManifestPath -Encoding utf8
  Publish-RunnableToolchainPackagePlatformEvidence `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -ManifestPath $ManifestPath `
    -ManifestPayload $manifestPayload `
    -SanitizerVariant $SanitizerVariant
  return $manifestPayload
}

function Get-RunnableToolchainPackagePlatformEvidenceRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$TargetPlatformId
  )

  $platformId = [System.Environment]::GetEnvironmentVariable("OBJC3C_PLATFORM_ID")
  $evidenceRoot = [System.Environment]::GetEnvironmentVariable("OBJC3C_PLATFORM_EVIDENCE_ROOT")
  if ([string]::IsNullOrWhiteSpace($platformId) -or [string]::IsNullOrWhiteSpace($evidenceRoot)) {
    return $null
  }
  if ($platformId -ne $TargetPlatformId) {
    return $null
  }
  if ($platformId -notin @("linux-x64", "darwin-arm64")) {
    return $null
  }

  $resolvedRoot = [System.IO.Path]::GetFullPath((Join-Path $RepoRoot $evidenceRoot))
  $expectedRoot = [System.IO.Path]::GetFullPath(
    (Join-Path $RepoRoot (Join-Path "tmp/reports/platform-host-evidence" $platformId))
  )
  $trimSeparators = [char[]]@([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar)
  if ($resolvedRoot.TrimEnd($trimSeparators) -ne $expectedRoot.TrimEnd($trimSeparators)) {
    throw "runnable toolchain package FAIL: platform evidence root must be platform-scoped: $evidenceRoot"
  }

  return $resolvedRoot
}

function Get-RunnableToolchainPackagePlatformLoaderPolicy {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "linux-x64") {
    return "ELF rpath, RUNPATH, or package-root loader resolution must be proven before support"
  }
  if ($TargetPlatformId -eq "darwin-arm64") {
    return "@rpath, install_name, codesign, and package-root loader behavior must be proven before support"
  }
  return "platform loader behavior must be proven before support"
}

function Get-RunnableToolchainPackagePlatformLinkerFlags {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -in @("linux-x64", "darwin-arm64")) {
    return @("-lobjc3-runtime")
  }
  return @()
}

function New-RunnableToolchainPackagePlatformRuntimeManifest {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [Parameter(Mandatory = $true)][string]$EvidenceRoot,
    [Parameter(Mandatory = $true)]$ManifestPayload
  )

  $targetPlatformId = [string]$ManifestPayload["target_platform_id"]
  $runtimeLibrary = [string]$ManifestPayload["runtime_library"]
  $runtimeLibraryPath = Join-Path $PackageRoot ($runtimeLibrary -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  if (!(Test-Path -LiteralPath $runtimeLibraryPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: platform runtime library missing before hosted evidence publication: $runtimeLibrary"
  }
  $runtimeLibraryItem = Get-Item -LiteralPath $runtimeLibraryPath

  return [ordered]@{
    contract_id = "objc3c.platform.runtime-library-manifest.v1"
    schema_version = 1
    target_platform_id = $targetPlatformId
    target_triple = [string]$ManifestPayload["target_triple"]
    object_format = [string]$ManifestPayload["object_format"]
    debug_format = [string]$ManifestPayload["debug_format"]
    package_root = [string]$ManifestPayload["package_root"]
    source_package_manifest_path = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $ManifestPath
    platform_scoped_package_manifest_path = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath (Join-Path $EvidenceRoot "package/objc3c-runnable-toolchain-package.json")
    runtime_library_ids = @("objc3-runtime")
    runtime_library_names = @([string]$ManifestPayload["runtime_library_name"])
    runtime_library_kind = [string]$ManifestPayload["runtime_library_kind"]
    runtime_library_artifacts = @(
      [ordered]@{
        runtime_library_id = "objc3-runtime"
        artifact = $runtimeLibrary
        source_file_name = [string]$ManifestPayload["runtime_library_name"]
        size_bytes = [int64]$runtimeLibraryItem.Length
        sha256 = (Get-FileHash -LiteralPath $runtimeLibraryPath -Algorithm SHA256).Hash.ToLowerInvariant()
        install_required = $true
      }
    )
    package_root_layout = @($ManifestPayload["package_root_layout"])
    linker_flags = @(Get-RunnableToolchainPackagePlatformLinkerFlags -TargetPlatformId $targetPlatformId)
    loader_policy = Get-RunnableToolchainPackagePlatformLoaderPolicy -TargetPlatformId $targetPlatformId
    runtime_load_probe_required = $true
    runtime_load_failure_behavior = "fail-closed-before-native-execution-claim"
    support_truth = $false
    native_execution_claimed = $false
  }
}

function Publish-RunnableToolchainPackagePlatformEvidence {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [Parameter(Mandatory = $true)]$ManifestPayload,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  if ($SanitizerVariant -ne "release") {
    return
  }

  $evidenceRoot = Get-RunnableToolchainPackagePlatformEvidenceRoot `
    -RepoRoot $RepoRoot `
    -TargetPlatformId ([string]$ManifestPayload["target_platform_id"])
  if ($null -eq $evidenceRoot) {
    return
  }

  $packageEvidenceRoot = Join-Path $evidenceRoot "package"
  New-Item -ItemType Directory -Force -Path $packageEvidenceRoot | Out-Null
  Copy-Item -LiteralPath $ManifestPath -Destination (Join-Path $packageEvidenceRoot "objc3c-runnable-toolchain-package.json") -Force

  $runtimeManifest = New-RunnableToolchainPackagePlatformRuntimeManifest `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -ManifestPath $ManifestPath `
    -EvidenceRoot $evidenceRoot `
    -ManifestPayload $ManifestPayload
  $runtimeManifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $packageEvidenceRoot "runtime-library-manifest.json") -Encoding utf8
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageManifestPayload",
  "Write-RunnableToolchainPackageManifest"
)
