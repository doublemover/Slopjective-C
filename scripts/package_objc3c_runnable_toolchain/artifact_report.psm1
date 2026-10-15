Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking
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

function Get-RunnableToolchainPackagePlatformIssueRef {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "linux-x64") {
    return 8228
  }
  if ($TargetPlatformId -eq "darwin-arm64") {
    return 8229
  }
  return 0
}

function Get-RunnableToolchainPackagePlatformExpectedLayout {
  param(
    [Parameter(Mandatory = $true)][string]$TargetPlatformId
  )

  $nativeExecutable = if ($TargetPlatformId -eq "windows-x64") {
    "artifacts/bin/objc3c-native.exe"
  } else {
    "artifacts/bin/objc3c-native"
  }
  $runtimeLibrary = if ($TargetPlatformId -eq "darwin-arm64") {
    "artifacts/lib/libobjc3-runtime.dylib"
  } elseif ($TargetPlatformId -eq "linux-x64") {
    "artifacts/lib/libobjc3-runtime.so"
  } else {
    "artifacts/lib/objc3_runtime.lib"
  }
  return @(
    "artifacts/package/objc3c-runnable-toolchain-package.json",
    $nativeExecutable,
    $runtimeLibrary,
    "stdlib/workspace.json",
    "stdlib/modules/objc3.core/module.json",
    "docs/runbooks/objc3c_packaging_channels.md"
  )
}

function Get-RunnableToolchainPackagePlatformRuntimeLibrary {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "darwin-arm64") {
    return "artifacts/lib/libobjc3-runtime.dylib"
  }
  if ($TargetPlatformId -eq "linux-x64") {
    return "artifacts/lib/libobjc3-runtime.so"
  }
  return "artifacts/lib/objc3_runtime.lib"
}

function Get-RunnableToolchainPackagePlatformRuntimeLibraryKind {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "windows-x64") {
    return "static-archive"
  }
  return "shared-library"
}

function Get-RunnableToolchainPackagePlatformTargetTriple {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "linux-x64") {
    return "x86_64-unknown-linux-gnu"
  }
  if ($TargetPlatformId -eq "darwin-arm64") {
    return "aarch64-apple-darwin"
  }
  if ($TargetPlatformId -eq "windows-x64") {
    return "x86_64-pc-windows-msvc"
  }
  return $TargetPlatformId
}

function Get-RunnableToolchainPackagePlatformObjectFormat {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "linux-x64") {
    return "ELF"
  }
  if ($TargetPlatformId -eq "darwin-arm64") {
    return "Mach-O"
  }
  return "COFF"
}

function Get-RunnableToolchainPackagePlatformDebugFormat {
  param([Parameter(Mandatory = $true)][string]$TargetPlatformId)

  if ($TargetPlatformId -eq "darwin-arm64") {
    return "DWARF/dSYM"
  }
  if ($TargetPlatformId -eq "linux-x64") {
    return "DWARF"
  }
  return "CodeView/PDB"
}

function Get-RunnableToolchainPackagePlatformManifestStatus {
  param(
    [Parameter(Mandatory = $true)][bool]$PackageManifestExists,
    [Parameter(Mandatory = $true)][bool]$RuntimeLibraryExists,
    [Parameter(Mandatory = $true)][string]$SourcePackageTargetPlatformId,
    [Parameter(Mandatory = $true)][string]$TargetPlatformId
  )

  if (-not $PackageManifestExists -or -not $RuntimeLibraryExists) {
    return "missing-source-generated-fail-closed"
  }
  if (-not [string]::IsNullOrWhiteSpace($SourcePackageTargetPlatformId) -and $SourcePackageTargetPlatformId -ne $TargetPlatformId) {
    return "package-target-mismatch-generated-fail-closed"
  }
  return "generated-host-artifact-present"
}

function New-RunnableToolchainPackageLogicalFileArtifact {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$LogicalPath
  )

  $artifact = [ordered]@{
    path = $LogicalPath
    exists = $false
  }
  if (Test-Path -LiteralPath $Path -PathType Leaf) {
    $item = Get-Item -LiteralPath $Path
    $artifact["exists"] = $true
    $artifact["size_bytes"] = [int64]$item.Length
    $artifact["sha256"] = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
  }
  return $artifact
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
  $runtimeLibrary = Get-RunnableToolchainPackagePlatformRuntimeLibrary -TargetPlatformId $targetPlatformId
  $runtimeLibraryName = Split-Path -Leaf $runtimeLibrary
  $runtimeLibraryPath = Join-Path $PackageRoot ($runtimeLibrary -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  $runtimeLibraryArtifact = [ordered]@{
    path = Get-RepoRelativePathCompat -RootPath $PackageRoot -TargetPath $runtimeLibraryPath
    exists = $false
  }
  if (Test-Path -LiteralPath $runtimeLibraryPath -PathType Leaf) {
    $runtimeLibraryItem = Get-Item -LiteralPath $runtimeLibraryPath
    $runtimeLibraryArtifact["exists"] = $true
    $runtimeLibraryArtifact["size_bytes"] = [int64]$runtimeLibraryItem.Length
    $runtimeLibraryArtifact["sha256"] = (Get-FileHash -LiteralPath $runtimeLibraryPath -Algorithm SHA256).Hash.ToLowerInvariant()
  }
  $packageManifestArtifact = New-RunnableToolchainPackageLogicalFileArtifact `
    -Path $ManifestPath `
    -LogicalPath "artifacts/package/objc3c-runnable-toolchain-package.json"
  $expectedLayout = Get-RunnableToolchainPackagePlatformExpectedLayout `
    -TargetPlatformId $targetPlatformId

  return [ordered]@{
    contract_id = "objc3c.platform.hosted-runtime-library-manifest.generated.v1"
    schema_version = 1
    platform_id = $targetPlatformId
    issue_ref = Get-RunnableToolchainPackagePlatformIssueRef -TargetPlatformId $targetPlatformId
    generated_report_path = Get-RepoRelativePathCompat `
      -RootPath $RepoRoot `
      -TargetPath (Join-Path $EvidenceRoot "package/runtime-library-manifest.json")
    source_package_manifest_path = "artifacts/package/objc3c-runnable-toolchain-package.json"
    support_truth = $false
    native_execution_claimed = $false
    promotion_allowed_from_generated_evidence = $false
    status = Get-RunnableToolchainPackagePlatformManifestStatus `
      -PackageManifestExists ([bool]$packageManifestArtifact.exists) `
      -RuntimeLibraryExists ([bool]$runtimeLibraryArtifact.exists) `
      -SourcePackageTargetPlatformId $targetPlatformId `
      -TargetPlatformId $targetPlatformId
    target_platform_id = $targetPlatformId
    source_package_target_platform_id = $targetPlatformId
    target_triple = Get-RunnableToolchainPackagePlatformTargetTriple -TargetPlatformId $targetPlatformId
    object_format = Get-RunnableToolchainPackagePlatformObjectFormat -TargetPlatformId $targetPlatformId
    debug_format = Get-RunnableToolchainPackagePlatformDebugFormat -TargetPlatformId $targetPlatformId
    package_root = [string]$ManifestPayload["package_root"]
    platform_scoped_package_manifest_path = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath (Join-Path $EvidenceRoot "package/objc3c-runnable-toolchain-package.json")
    runtime_library_ids = @("objc3-runtime")
    runtime_library_names = @($runtimeLibraryName)
    runtime_library_kind = Get-RunnableToolchainPackagePlatformRuntimeLibraryKind -TargetPlatformId $targetPlatformId
    runtime_library_artifacts = @($runtimeLibraryArtifact)
    package_root_layout = $expectedLayout
    package_manifest_artifact = $packageManifestArtifact
    linker_flags = @(Get-RunnableToolchainPackagePlatformLinkerFlags -TargetPlatformId $targetPlatformId)
    loader_policy = Get-RunnableToolchainPackagePlatformLoaderPolicy -TargetPlatformId $targetPlatformId
    runtime_load_probe_required = $true
    runtime_load_failure_behavior = "fail-closed-before-native-execution-claim"
    producer_evidence = [ordered]@{
      contract_id = "objc3c.platform.package.runtime-library-manifest.seed.v1"
      status = if ([bool]$runtimeLibraryArtifact.exists) { "PACKAGE_RUNTIME_LIBRARY_ARTIFACT_PRESENT" } else { "PACKAGE_RUNTIME_LIBRARY_ARTIFACT_MISSING" }
      runtime_library_artifacts = @(
        [ordered]@{
          runtime_library_id = "objc3-runtime"
          artifact = $runtimeLibrary
          source_file_name = $runtimeLibraryName
          install_required = $true
        }
      )
      missing_runtime_behavior = "fail-closed-before-package-install"
    }
    source_artifacts = @($packageManifestArtifact)
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
