"""PowerShell script rendering for package channels."""

from __future__ import annotations

from .model import (
    DEFAULT_TARGET_PLATFORM_ID,
    RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM,
    release_package_channel_id_for_platform,
    release_package_id_for_platform,
    required_payload_entries_for_platform,
)
from .sanitizer_contracts import runtime_package_variant_contract


def powershell_string_array(entries: list[str], *, indent: str = "  ") -> str:
    return "\n".join(f'{indent}"{entry}"' for entry in entries)


def install_script_text(
    sanitizer_variant: str = "release",
    *,
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> str:
    runtime_package_variant_contract(sanitizer_variant)
    if sanitizer_variant != "release" and target_platform_id != DEFAULT_TARGET_PLATFORM_ID:
        raise RuntimeError("sanitizer package channels are currently windows-x64 only")
    payload_required_entries = required_payload_entries_for_platform(
        sanitizer_variant="release",
        target_platform_id=target_platform_id,
    )
    runtime_library_names = RELEASE_RUNTIME_LIBRARY_NAMES_BY_PLATFORM[target_platform_id]
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot,
  [switch]$Force,
  [ValidateSet("local-installer", "offline-bundle")][string]$ChannelId = "local-installer",
  [ValidateSet("release", "address", "undefined")][string]$SanitizerVariant = "__SANITIZER_VARIANT__"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$sourceRoot = Join-Path $PSScriptRoot "payload"
$resolvedInstallRoot = [System.IO.Path]::GetFullPath($InstallRoot)
$installHome = Join-Path $resolvedInstallRoot "objc3c"
$receiptPath = Join-Path $resolvedInstallRoot "objc3c-install-receipt.json"
$bootstrapSource = Join-Path $PSScriptRoot "Bootstrap-objc3cEnvironment.ps1"
$bootstrapTarget = Join-Path $resolvedInstallRoot "Bootstrap-objc3cEnvironment.ps1"
$payloadManifest = "artifacts/package/objc3c-runnable-toolchain-package.json"
$expectedSanitizerVariant = "__SANITIZER_VARIANT__"
$targetPlatformId = "__TARGET_PLATFORM_ID__"
if ($SanitizerVariant -ne $expectedSanitizerVariant) {
  throw "installer sanitizer selector does not match packaged runtime variant: expected $expectedSanitizerVariant, got $SanitizerVariant"
}
$payloadRequiredEntries = @(
__PAYLOAD_REQUIRED_ENTRIES__
)
if ($SanitizerVariant -eq "address") {
  $payloadRequiredEntries += "share/objc3c/sanitizer/asan-metadata.json"
  $payloadRequiredEntries += "share/objc3c/sanitizer/asan-runtime-libraries.json"
  $payloadRequiredEntries += "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll"
  $payloadRequiredEntries += "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib"
  $payloadRequiredEntries += "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
} elseif ($SanitizerVariant -eq "undefined") {
  $payloadRequiredEntries += "share/objc3c/sanitizer/ubsan-metadata.json"
  $payloadRequiredEntries += "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
  $payloadRequiredEntries += "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib"
  $payloadRequiredEntries += "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib"
}
$allowedReceiptChannels = @("local-installer", "offline-bundle")

function Resolve-PackageRuntimeModel {
  $packageId = "__PACKAGE_ID__"
  $packageChannelId = "__PACKAGE_CHANNEL_ID__"
  $runtimeVariant = "release"
  $runtimeLibraryIds = @("objc3-runtime")
  $runtimeLibraryNames = @(
__RUNTIME_LIBRARY_NAMES__
  )
  $missingRuntimeBehavior = "fail-closed-before-native-execution-claim"

  if ($SanitizerVariant -eq "address") {
    $packageId = "org.objc3c.runtime:objc3c-runtime-asan"
    $packageChannelId = "windows-x64-sanitizer-asan"
    $runtimeVariant = "sanitizer=address"
    $runtimeLibraryIds = @("objc3-runtime", "clang_rt.asan")
    $runtimeLibraryNames = @(
      "objc3_runtime.lib",
      "clang_rt.asan_dynamic-x86_64.dll",
      "clang_rt.asan_dynamic-x86_64.lib",
      "clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    )
    $missingRuntimeBehavior = "fail-closed-before-package-install"
  } elseif ($SanitizerVariant -eq "undefined") {
    $packageId = "org.objc3c.runtime:objc3c-runtime-ubsan"
    $packageChannelId = "windows-x64-sanitizer-ubsan"
    $runtimeVariant = "sanitizer=undefined"
    $runtimeLibraryIds = @("objc3-runtime", "clang_rt.ubsan")
    $runtimeLibraryNames = @(
      "objc3_runtime.lib",
      "clang_rt.ubsan_standalone-x86_64.lib",
      "clang_rt.ubsan_standalone_cxx-x86_64.lib"
    )
    $missingRuntimeBehavior = "fail-closed-before-package-install"
  }

  return [ordered]@{
    target_platform_id = $targetPlatformId
    package_id = $packageId
    package_channel_id = $packageChannelId
    sanitizer_variant = $SanitizerVariant
    runtime_variant = $runtimeVariant
    runtime_library_ids = $runtimeLibraryIds
    runtime_library_names = $runtimeLibraryNames
    package_root_layout = @($payloadRequiredEntries)
    missing_runtime_behavior = $missingRuntimeBehavior
    unsupported_behavior = "fail-closed"
    support_truth = $false
    native_execution_claimed = $false
  }
}

function Assert-PayloadEntriesMatch {
  param(
    [Parameter(Mandatory = $true)]$ActualEntries,
    [Parameter(Mandatory = $true)]$ExpectedEntries,
    [Parameter(Mandatory = $true)][string]$Context
  )

  $actual = @($ActualEntries | ForEach-Object { [string]$_ })
  $expected = @($ExpectedEntries | ForEach-Object { [string]$_ })
  if ($actual.Count -ne $expected.Count) {
    throw "$Context payload entry count drifted"
  }
  for ($index = 0; $index -lt $expected.Count; $index++) {
    if ($actual[$index] -ne $expected[$index]) {
      throw "$Context payload entry drifted at index $index: expected $($expected[$index]), got $($actual[$index])"
    }
  }
}

function Resolve-SanitizerPackageVariant {
  if ($SanitizerVariant -eq "release") {
    return $null
  }

  $runtimeManifestPath = ""
  $expectedRuntimeEntries = @()
  if ($SanitizerVariant -eq "address") {
    $runtimeManifestPath = "share/objc3c/sanitizer/asan-runtime-libraries.json"
    $expectedRuntimeEntries = @(
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    )
  } else {
    $runtimeManifestPath = "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
    $expectedRuntimeEntries = @(
      "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
      "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib"
    )
  }

  $runtimeManifestInstalledPath = Join-Path $installHome ($runtimeManifestPath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  if (!(Test-Path -LiteralPath $runtimeManifestInstalledPath -PathType Leaf)) {
    throw "sanitizer runtime library manifest missing before receipt emission: $runtimeManifestPath"
  }
  $runtimeManifest = Get-Content -LiteralPath $runtimeManifestInstalledPath -Raw | ConvertFrom-Json
  if ([string]$runtimeManifest.contract_id -ne "objc3c.sanitizer.runtime-library-manifest.v1" -or
      [string]$runtimeManifest.sanitizer -ne $SanitizerVariant -or
      [string]$runtimeManifest.missing_runtime_behavior -ne "fail-closed-before-package-install" -or
      $runtimeManifest.support_truth -ne $false -or
      $runtimeManifest.native_execution_claimed -ne $false) {
    throw "sanitizer runtime library manifest identity drifted before receipt emission: $runtimeManifestPath"
  }
  $runtimeArtifacts = @($runtimeManifest.runtime_library_artifacts)
  Assert-PayloadEntriesMatch `
    -ActualEntries @($runtimeArtifacts | ForEach-Object { [string]$_.artifact }) `
    -ExpectedEntries $expectedRuntimeEntries `
    -Context "sanitizer runtime library manifest"
  foreach ($runtimeArtifact in $runtimeArtifacts) {
    $artifactRelativePath = [string]$runtimeArtifact.artifact
    $artifactPath = Join-Path $installHome ($artifactRelativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "sanitizer runtime library missing before receipt emission: $artifactRelativePath"
    }
    $artifactDigest = (Get-FileHash -LiteralPath $artifactPath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ([string]$runtimeArtifact.sha256 -ne $artifactDigest -or $runtimeArtifact.install_required -ne $true) {
      throw "sanitizer runtime library digest drifted before receipt emission: $artifactRelativePath"
    }
  }
  $runtimeManifestDigest = "sha256:" + (Get-FileHash -LiteralPath $runtimeManifestInstalledPath -Algorithm SHA256).Hash.ToLowerInvariant()

  if ($SanitizerVariant -eq "address") {
    $metadataManifestPath = "share/objc3c/sanitizer/asan-metadata.json"
    $metadataPath = Join-Path $installHome ($metadataManifestPath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
    if (!(Test-Path -LiteralPath $metadataPath -PathType Leaf)) {
      throw "sanitizer install metadata missing before receipt emission: $metadataManifestPath"
    }
    return [ordered]@{
      package_id = "org.objc3c.runtime:objc3c-runtime-asan"
      package_variant_row_id = "objc3c.package.sanitizer.asan.reserved"
      package_channel_id = "windows-x64-sanitizer-asan"
      target_platform_id = "windows-x64"
      sanitizer = "address"
      runtime_library_ids = @("objc3-runtime", "clang_rt.asan")
      metadata_manifest_path = $metadataManifestPath
      metadata_digest = "sha256:" + (Get-FileHash -LiteralPath $metadataPath -Algorithm SHA256).Hash.ToLowerInvariant()
      runtime_library_manifest_path = $runtimeManifestPath
      runtime_library_manifest_digest = $runtimeManifestDigest
      runtime_library_artifacts = $runtimeArtifacts
      missing_runtime_behavior = "fail-closed-before-package-install"
      selected_runtime_variant = "sanitizer=address"
      install_selector = "sanitizer=address"
      native_execution_contract = [ordered]@{
        native_execution_required_before_support = $true
        native_execution_record_required = $true
        native_execution_record_fields = @("executable_path", "target_platform_id", "sanitizer", "runtime_library_ids", "runtime_library_artifacts", "environment", "exit_code", "diagnostic_records")
        missing_native_execution_behavior = "fail-closed-before-support-promotion"
        native_execution_claimed = $false
      }
      support_truth = $false
      native_execution_claimed = $false
    }
  }

  $metadataManifestPath = "share/objc3c/sanitizer/ubsan-metadata.json"
  $metadataPath = Join-Path $installHome ($metadataManifestPath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  if (!(Test-Path -LiteralPath $metadataPath -PathType Leaf)) {
    throw "sanitizer install metadata missing before receipt emission: $metadataManifestPath"
  }
  return [ordered]@{
    package_id = "org.objc3c.runtime:objc3c-runtime-ubsan"
    package_variant_row_id = "objc3c.package.sanitizer.ubsan.reserved"
    package_channel_id = "windows-x64-sanitizer-ubsan"
    target_platform_id = "windows-x64"
    sanitizer = "undefined"
    runtime_library_ids = @("objc3-runtime", "clang_rt.ubsan")
    metadata_manifest_path = $metadataManifestPath
    metadata_digest = "sha256:" + (Get-FileHash -LiteralPath $metadataPath -Algorithm SHA256).Hash.ToLowerInvariant()
    runtime_library_manifest_path = $runtimeManifestPath
    runtime_library_manifest_digest = $runtimeManifestDigest
    runtime_library_artifacts = $runtimeArtifacts
    missing_runtime_behavior = "fail-closed-before-package-install"
    selected_runtime_variant = "sanitizer=undefined"
    install_selector = "sanitizer=undefined"
    trap_or_recover_mode = "trap"
    native_execution_contract = [ordered]@{
      native_execution_required_before_support = $true
      native_execution_record_required = $true
      native_execution_record_fields = @("executable_path", "target_platform_id", "sanitizer", "runtime_library_ids", "runtime_library_artifacts", "environment", "exit_code", "diagnostic_records", "trap_or_recover_mode")
      missing_native_execution_behavior = "fail-closed-before-support-promotion"
      native_execution_claimed = $false
    }
    support_truth = $false
    native_execution_claimed = $false
  }
}

function Assert-ReceiptSanitizerVariant {
  param([Parameter(Mandatory = $true)]$Receipt)

  if ($SanitizerVariant -eq "release") {
    if ($null -ne $Receipt.sanitizer_package_variant) {
      throw "installer target receipt uses sanitizer runtime for release install: $installHome"
    }
    return
  }

  $expectedPackageId = ""
  $expectedPackageVariantRowId = ""
  $expectedPackageChannelId = ""
  $expectedRuntimeLibraries = @()
  $expectedMetadataPath = ""
  $expectedRuntimeManifestPath = ""
  $expectedRuntimeEntries = @()
  $expectedSelector = "sanitizer=" + $SanitizerVariant
  if ($SanitizerVariant -eq "address") {
    $expectedPackageId = "org.objc3c.runtime:objc3c-runtime-asan"
    $expectedPackageVariantRowId = "objc3c.package.sanitizer.asan.reserved"
    $expectedPackageChannelId = "windows-x64-sanitizer-asan"
    $expectedRuntimeLibraries = @("objc3-runtime", "clang_rt.asan")
    $expectedMetadataPath = "share/objc3c/sanitizer/asan-metadata.json"
    $expectedRuntimeManifestPath = "share/objc3c/sanitizer/asan-runtime-libraries.json"
    $expectedRuntimeEntries = @(
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    )
  } else {
    $expectedPackageId = "org.objc3c.runtime:objc3c-runtime-ubsan"
    $expectedPackageVariantRowId = "objc3c.package.sanitizer.ubsan.reserved"
    $expectedPackageChannelId = "windows-x64-sanitizer-ubsan"
    $expectedRuntimeLibraries = @("objc3-runtime", "clang_rt.ubsan")
    $expectedMetadataPath = "share/objc3c/sanitizer/ubsan-metadata.json"
    $expectedRuntimeManifestPath = "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
    $expectedRuntimeEntries = @(
      "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
      "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib"
    )
  }

  if ($null -eq $Receipt.sanitizer_package_variant) {
    throw "installer target receipt missing sanitizer package variant: $installHome"
  }
  if ([string]$Receipt.sanitizer_package_variant.sanitizer -ne $SanitizerVariant) {
    throw "installer target receipt sanitizer variant drifted: $installHome"
  }
  if ([string]$Receipt.sanitizer_package_variant.package_id -ne $expectedPackageId -or
      [string]$Receipt.sanitizer_package_variant.package_variant_row_id -ne $expectedPackageVariantRowId -or
      [string]$Receipt.sanitizer_package_variant.package_channel_id -ne $expectedPackageChannelId -or
      [string]$Receipt.sanitizer_package_variant.target_platform_id -ne "windows-x64" -or
      [string]$Receipt.sanitizer_package_variant.metadata_manifest_path -ne $expectedMetadataPath -or
      [string]$Receipt.sanitizer_package_variant.runtime_library_manifest_path -ne $expectedRuntimeManifestPath -or
      [string]$Receipt.sanitizer_package_variant.missing_runtime_behavior -ne "fail-closed-before-package-install" -or
      [string]$Receipt.sanitizer_package_variant.selected_runtime_variant -ne $expectedSelector -or
      [string]$Receipt.sanitizer_package_variant.install_selector -ne $expectedSelector) {
    throw "installer target receipt sanitizer package identity drifted: $installHome"
  }
  Assert-PayloadEntriesMatch `
    -ActualEntries @($Receipt.sanitizer_package_variant.runtime_library_ids) `
    -ExpectedEntries $expectedRuntimeLibraries `
    -Context "installer target receipt sanitizer runtime libraries"
  if ([string]$Receipt.sanitizer_package_variant.metadata_digest -notmatch '^sha256:[0-9a-f]{64}$') {
    throw "installer target receipt sanitizer metadata digest drifted: $installHome"
  }
  if ([string]$Receipt.sanitizer_package_variant.runtime_library_manifest_digest -notmatch '^sha256:[0-9a-f]{64}$') {
    throw "installer target receipt sanitizer runtime library manifest digest drifted: $installHome"
  }
  Assert-PayloadEntriesMatch `
    -ActualEntries @($Receipt.sanitizer_package_variant.runtime_library_artifacts | ForEach-Object { [string]$_.artifact }) `
    -ExpectedEntries $expectedRuntimeEntries `
    -Context "installer target receipt sanitizer runtime library artifacts"
  foreach ($runtimeArtifact in @($Receipt.sanitizer_package_variant.runtime_library_artifacts)) {
    if ([string]$runtimeArtifact.sha256 -notmatch '^[0-9a-f]{64}$' -or $runtimeArtifact.install_required -ne $true) {
      throw "installer target receipt sanitizer runtime library artifact digest drifted: $installHome"
    }
  }
  if ($SanitizerVariant -eq "undefined" -and [string]$Receipt.sanitizer_package_variant.trap_or_recover_mode -ne "trap") {
    throw "installer target receipt UBSan trap-or-recover mode drifted: $installHome"
  }
  if ($Receipt.sanitizer_package_variant.native_execution_contract.native_execution_required_before_support -ne $true -or
      $Receipt.sanitizer_package_variant.native_execution_contract.native_execution_record_required -ne $true -or
      [string]$Receipt.sanitizer_package_variant.native_execution_contract.missing_native_execution_behavior -ne "fail-closed-before-support-promotion" -or
      $Receipt.sanitizer_package_variant.native_execution_contract.native_execution_claimed -ne $false) {
    throw "installer target receipt sanitizer native execution contract drifted: $installHome"
  }
  if ($Receipt.sanitizer_package_variant.support_truth -ne $false -or
      $Receipt.sanitizer_package_variant.native_execution_claimed -ne $false) {
    throw "installer target receipt promoted sanitizer support without native execution: $installHome"
  }
}

function Assert-NoReparsePointInExistingPath {
  param([Parameter(Mandatory = $true)][string]$Path)

  $fullPath = [System.IO.Path]::GetFullPath($Path)
  $pathRoot = [System.IO.Path]::GetPathRoot($fullPath)
  $currentPath = $pathRoot
  foreach ($segment in @($fullPath.Substring($pathRoot.Length) -split '[\\/]+')) {
    if ([string]::IsNullOrWhiteSpace($segment)) {
      continue
    }
    $currentPath = Join-Path $currentPath $segment
    if (Test-Path -LiteralPath $currentPath) {
      $item = Get-Item -LiteralPath $currentPath -Force
      if (($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "installer refuses to operate through reparse point: $currentPath"
      }
    }
  }
}

function Assert-NoReparsePointInExistingTree {
  param([Parameter(Mandatory = $true)][string]$Path)

  Assert-NoReparsePointInExistingPath -Path $Path
  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return
  }
  $reparseChild = Get-ChildItem -LiteralPath $Path -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "installer refuses to recursively remove tree containing reparse point: $($reparseChild.FullName)"
  }
}

function Assert-ReceiptOwnsInstallHome {
  if (!(Test-Path -LiteralPath $receiptPath -PathType Leaf)) {
    throw "installer target exists without an objc3c install receipt: $installHome"
  }
  $receipt = Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
  $receiptPayloadEntries = @($receipt.payload_required_entries)
  if ($receipt.contract_id -ne "objc3c.packaging.channels.install-receipt.v1" -or
      [System.IO.Path]::GetFullPath([string]$receipt.install_home) -ne $installHome -or
      $allowedReceiptChannels -notcontains [string]$receipt.channel_id -or
      [string]$receipt.bootstrap_entrypoint -ne "Bootstrap-objc3cEnvironment.ps1" -or
      [string]$receipt.package_bridge -ne "objc3c" -or
      [string]$receipt.install_command -ne "npm run objc3c -- build-package-channels" -or
      [string]$receipt.payload_manifest -ne $payloadManifest -or
      [string]::IsNullOrWhiteSpace([string]$receipt.payload_manifest_sha256)) {
    throw "installer target receipt does not own install home: $installHome"
  }
  Assert-PayloadEntriesMatch `
    -ActualEntries $receiptPayloadEntries `
    -ExpectedEntries $payloadRequiredEntries `
    -Context "installer target receipt"
  Assert-ReceiptSanitizerVariant -Receipt $receipt
}

function Resolve-InstalledPayloadPath {
  param([Parameter(Mandatory = $true)][string]$RelativePath)

  return Join-Path $installHome ($RelativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
}

function Assert-InstalledPayloadContract {
  $manifestPath = Resolve-InstalledPayloadPath -RelativePath $payloadManifest
  if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    throw "installer payload missing runnable manifest: $payloadManifest"
  }
  foreach ($relativePath in $payloadRequiredEntries) {
    $payloadPath = Resolve-InstalledPayloadPath -RelativePath $relativePath
    if (!(Test-Path -LiteralPath $payloadPath -PathType Leaf)) {
      throw "installer payload missing required entry: $relativePath"
    }
  }
  return (Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash.ToLowerInvariant()
}

if ((Test-Path -LiteralPath $installHome) -and -not $Force.IsPresent) {
  throw "installer target already exists: $installHome"
}

Assert-NoReparsePointInExistingPath -Path $resolvedInstallRoot
if (Test-Path -LiteralPath $installHome) {
  Assert-ReceiptOwnsInstallHome
  Assert-NoReparsePointInExistingTree -Path $installHome
  Remove-Item -LiteralPath $installHome -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $resolvedInstallRoot | Out-Null
Copy-Item -LiteralPath $sourceRoot -Destination $installHome -Recurse -Force
Copy-Item -LiteralPath $bootstrapSource -Destination $bootstrapTarget -Force
$payloadManifestSha256 = Assert-InstalledPayloadContract
$packageRuntimeModel = Resolve-PackageRuntimeModel
$sanitizerPackageVariant = Resolve-SanitizerPackageVariant

$receipt = [ordered]@{
  contract_id = "objc3c.packaging.channels.install-receipt.v1"
  install_root = $resolvedInstallRoot
  install_home = $installHome
  channel_id = $ChannelId
  bootstrap_entrypoint = "Bootstrap-objc3cEnvironment.ps1"
  package_bridge = "objc3c"
  install_command = "npm run objc3c -- build-package-channels"
  payload_manifest = $payloadManifest
  payload_manifest_sha256 = $payloadManifestSha256
  payload_required_entries = $payloadRequiredEntries
  target_platform_id = [string]$packageRuntimeModel.target_platform_id
  package_id = [string]$packageRuntimeModel.package_id
  package_channel_id = [string]$packageRuntimeModel.package_channel_id
  sanitizer_variant = $SanitizerVariant
  package_runtime_model = $packageRuntimeModel
  support_truth = $false
  native_execution_claimed = $false
  installed_at_utc = [DateTime]::UtcNow.ToString("o")
}
if ($null -ne $sanitizerPackageVariant) {
  $receipt["sanitizer_package_variant"] = $sanitizerPackageVariant
}
$receipt | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $receiptPath -Encoding utf8

Write-Output ("install_root: " + $resolvedInstallRoot)
Write-Output ("install_home: " + $installHome)
Write-Output ("receipt_path: " + $receiptPath)
Write-Output ("bootstrap_entrypoint: " + $bootstrapTarget)
""".replace("__SANITIZER_VARIANT__", sanitizer_variant).replace(
        "__TARGET_PLATFORM_ID__",
        target_platform_id,
    ).replace(
        "__PACKAGE_ID__",
        release_package_id_for_platform(target_platform_id),
    ).replace(
        "__PACKAGE_CHANNEL_ID__",
        release_package_channel_id_for_platform(target_platform_id),
    ).replace(
        "__PAYLOAD_REQUIRED_ENTRIES__",
        powershell_string_array(payload_required_entries),
    ).replace(
        "__RUNTIME_LIBRARY_NAMES__",
        powershell_string_array(runtime_library_names, indent="    "),
    )


def uninstall_script_text(
    *,
    target_platform_id: str = DEFAULT_TARGET_PLATFORM_ID,
) -> str:
    payload_required_entries = required_payload_entries_for_platform(
        sanitizer_variant="release",
        target_platform_id=target_platform_id,
    )
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$resolvedInstallRoot = [System.IO.Path]::GetFullPath($InstallRoot)
$installHome = Join-Path $resolvedInstallRoot "objc3c"
$receiptPath = Join-Path $resolvedInstallRoot "objc3c-install-receipt.json"
$bootstrapTarget = Join-Path $resolvedInstallRoot "Bootstrap-objc3cEnvironment.ps1"
$payloadManifest = "artifacts/package/objc3c-runnable-toolchain-package.json"
$targetPlatformId = "__TARGET_PLATFORM_ID__"
$payloadRequiredEntries = @(
__PAYLOAD_REQUIRED_ENTRIES__
)
$allowedReceiptChannels = @("local-installer", "offline-bundle")

function Assert-PayloadEntriesMatch {
  param(
    [Parameter(Mandatory = $true)]$ActualEntries,
    [Parameter(Mandatory = $true)]$ExpectedEntries,
    [Parameter(Mandatory = $true)][string]$Context
  )

  $actual = @($ActualEntries | ForEach-Object { [string]$_ })
  $expected = @($ExpectedEntries | ForEach-Object { [string]$_ })
  if ($actual.Count -ne $expected.Count) {
    throw "$Context payload entry count drifted"
  }
  for ($index = 0; $index -lt $expected.Count; $index++) {
    if ($actual[$index] -ne $expected[$index]) {
      throw "$Context payload entry drifted at index $index: expected $($expected[$index]), got $($actual[$index])"
    }
  }
}

function Assert-NoReparsePointInExistingPath {
  param([Parameter(Mandatory = $true)][string]$Path)

  $fullPath = [System.IO.Path]::GetFullPath($Path)
  $pathRoot = [System.IO.Path]::GetPathRoot($fullPath)
  $currentPath = $pathRoot
  foreach ($segment in @($fullPath.Substring($pathRoot.Length) -split '[\\/]+')) {
    if ([string]::IsNullOrWhiteSpace($segment)) {
      continue
    }
    $currentPath = Join-Path $currentPath $segment
    if (Test-Path -LiteralPath $currentPath) {
      $item = Get-Item -LiteralPath $currentPath -Force
      if (($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "uninstaller refuses to operate through reparse point: $currentPath"
      }
    }
  }
}

function Assert-NoReparsePointInExistingTree {
  param([Parameter(Mandatory = $true)][string]$Path)

  Assert-NoReparsePointInExistingPath -Path $Path
  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return
  }
  $reparseChild = Get-ChildItem -LiteralPath $Path -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "uninstaller refuses to recursively remove tree containing reparse point: $($reparseChild.FullName)"
  }
}

function Assert-ReceiptOwnsInstallHome {
  if (!(Test-Path -LiteralPath $receiptPath -PathType Leaf)) {
    throw "uninstaller target exists without an objc3c install receipt: $installHome"
  }
  $receipt = Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
  $receiptPayloadEntries = @($receipt.payload_required_entries)
  $expectedPayloadEntries = @($payloadRequiredEntries)
  if ($null -ne $receipt.sanitizer_package_variant) {
    if ([string]$receipt.sanitizer_package_variant.sanitizer -eq "address") {
      $expectedPayloadEntries += "share/objc3c/sanitizer/asan-metadata.json"
      $expectedPayloadEntries += "share/objc3c/sanitizer/asan-runtime-libraries.json"
      $expectedPayloadEntries += "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll"
      $expectedPayloadEntries += "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib"
      $expectedPayloadEntries += "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    } elseif ([string]$receipt.sanitizer_package_variant.sanitizer -eq "undefined") {
      $expectedPayloadEntries += "share/objc3c/sanitizer/ubsan-metadata.json"
      $expectedPayloadEntries += "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
      $expectedPayloadEntries += "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib"
      $expectedPayloadEntries += "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib"
    } else {
      throw "uninstaller target receipt has unknown sanitizer package variant: $installHome"
    }
  }
  if ($receipt.contract_id -ne "objc3c.packaging.channels.install-receipt.v1" -or
      [System.IO.Path]::GetFullPath([string]$receipt.install_home) -ne $installHome -or
      $allowedReceiptChannels -notcontains [string]$receipt.channel_id -or
      [string]$receipt.bootstrap_entrypoint -ne "Bootstrap-objc3cEnvironment.ps1" -or
      [string]$receipt.package_bridge -ne "objc3c" -or
      [string]$receipt.install_command -ne "npm run objc3c -- build-package-channels" -or
      [string]$receipt.payload_manifest -ne $payloadManifest -or
      [string]$receipt.target_platform_id -ne $targetPlatformId -or
      [string]::IsNullOrWhiteSpace([string]$receipt.payload_manifest_sha256)) {
    throw "uninstaller target receipt does not own install home: $installHome"
  }
  Assert-PayloadEntriesMatch `
    -ActualEntries $receiptPayloadEntries `
    -ExpectedEntries $expectedPayloadEntries `
    -Context "uninstaller target receipt"
}

if (Test-Path -LiteralPath $installHome) {
  Assert-ReceiptOwnsInstallHome
  Assert-NoReparsePointInExistingTree -Path $installHome
  Remove-Item -LiteralPath $installHome -Recurse -Force
}
if (Test-Path -LiteralPath $bootstrapTarget) {
  Remove-Item -LiteralPath $bootstrapTarget -Force
}
if (Test-Path -LiteralPath $receiptPath) {
  Remove-Item -LiteralPath $receiptPath -Force
}

Write-Output ("rollback_root: " + $resolvedInstallRoot)
""".replace(
        "__TARGET_PLATFORM_ID__",
        target_platform_id,
    ).replace(
        "__PAYLOAD_REQUIRED_ENTRIES__",
        powershell_string_array(payload_required_entries),
    )


def bootstrap_script_text() -> str:
    return """param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$installRoot = $PSScriptRoot
$toolchainHome = Join-Path $installRoot "objc3c"
$binPath = Join-Path $toolchainHome "artifacts/bin"

$env:OBJC3C_HOME = $toolchainHome
if ($env:PATH -notmatch [regex]::Escape($binPath)) {
  $env:PATH = $binPath + [System.IO.Path]::PathSeparator + $env:PATH
}

Write-Output ("objc3c_home: " + $env:OBJC3C_HOME)
Write-Output ("objc3c_bin: " + $binPath)
"""


def offline_bootstrap_script_text(
    sanitizer_variant: str = "release",
    installer_archive_name: str = "objc3c-windows-x64-installer.zip",
) -> str:
    runtime_package_variant_contract(sanitizer_variant)
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$bundleRoot = $PSScriptRoot
$stagingRoot = Join-Path $bundleRoot "staging"
$installerArchive = Join-Path $bundleRoot "channels/__INSTALLER_ARCHIVE_NAME__"
$installerImageRoot = Join-Path $stagingRoot ("installer-image-" + [Guid]::NewGuid().ToString("N"))
$installerScript = Join-Path $installerImageRoot "Install-objc3c.ps1"

function Assert-NoReparsePointInExistingTree {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return
  }
  $root = Get-Item -LiteralPath $Path -Force
  if (($root.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw "offline bootstrap refuses to remove reparse-point staging root: $Path"
  }
  $reparseChild = Get-ChildItem -LiteralPath $Path -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "offline bootstrap refuses to recursively remove staging tree containing reparse point: $($reparseChild.FullName)"
  }
}

try {
  New-Item -ItemType Directory -Force -Path $stagingRoot | Out-Null
  Expand-Archive -LiteralPath $installerArchive -DestinationPath $installerImageRoot -Force

  & $installerScript -InstallRoot $InstallRoot -Force -ChannelId "offline-bundle" -SanitizerVariant "__SANITIZER_VARIANT__"
  if (-not $?) {
    exit $LASTEXITCODE
  }
}
finally {
  if (Test-Path -LiteralPath $installerImageRoot) {
    Assert-NoReparsePointInExistingTree -Path $installerImageRoot
    Remove-Item -LiteralPath $installerImageRoot -Recurse -Force
  }
}

Write-Output ("offline_bundle_root: " + $bundleRoot)
Write-Output ("installer_archive: " + $installerArchive)
""".replace("__SANITIZER_VARIANT__", sanitizer_variant).replace(
        "__INSTALLER_ARCHIVE_NAME__",
        installer_archive_name,
    )
