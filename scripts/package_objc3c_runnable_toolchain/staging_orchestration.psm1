Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\objc3c_native_cmake.psm1") -Force -DisableNameChecking

function Get-RunnableToolchainPackagePrivateBuildRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $normalizedPackageRoot = [System.IO.Path]::GetFullPath($PackageRoot).ToLowerInvariant()
  $packageRootBytes = [System.Text.Encoding]::UTF8.GetBytes($normalizedPackageRoot)
  $hashBytes = [System.Security.Cryptography.SHA256]::HashData($packageRootBytes)
  $hashPrefix = -join (
    $hashBytes[0..5] |
      ForEach-Object { $_.ToString("x2") }
  )
  return Join-Path (Join-Path $RepoRoot "tmp/b/pkg") $hashPrefix
}

function Get-RunnableToolchainPackageGeneratedArtifactPaths {
  return @(
    "artifacts/bin/objc3c-native.exe",
    "artifacts/bin/objc3c-frontend-c-api-runner.exe",
    "artifacts/lib/objc3_runtime.lib",
    "tmp/artifacts/objc3c-native/frontend_source_graph.json",
    "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
    "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
    "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
    "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
    "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
    "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
    "share/objc3c/sanitizer/asan-metadata.json",
    "share/objc3c/sanitizer/ubsan-metadata.json",
    "share/objc3c/sanitizer/asan-runtime-libraries.json",
    "share/objc3c/sanitizer/ubsan-runtime-libraries.json",
    "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
    "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
    "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib",
    "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
    "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib",
    "tmp/build-objc3c-native/repo_superclean_source_of_truth.json"
  )
}

function Get-RunnableToolchainPackageSanitizerVariantMetadata {
  param(
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  if ($SanitizerVariant -eq "release") {
    return $null
  }

  if ($SanitizerVariant -eq "address") {
    return [ordered]@{
      package_id = "org.objc3c.runtime:objc3c-runtime-asan"
      package_variant_row_id = "objc3c.package.sanitizer.asan.reserved"
      package_channel_id = "windows-x64-sanitizer-asan"
      target_platform_id = "windows-x64"
      sanitizer = "address"
      selected_runtime_variant = "sanitizer=address"
      install_selector = "sanitizer=address"
      metadata_manifest_path = "share/objc3c/sanitizer/asan-metadata.json"
      runtime_library_manifest_path = "share/objc3c/sanitizer/asan-runtime-libraries.json"
      runtime_library_payload_entries = @(
        "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
        "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
        "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
      )
      runtime_library_ids = @("objc3-runtime", "clang_rt.asan")
      compiler_flags = @("-fsanitize=address", "-fno-omit-frame-pointer")
      linker_flags = @("-fsanitize=address")
      environment = "ASAN_OPTIONS"
      missing_runtime_behavior = "fail-closed-before-package-install"
    }
  }

  return [ordered]@{
    package_id = "org.objc3c.runtime:objc3c-runtime-ubsan"
    package_variant_row_id = "objc3c.package.sanitizer.ubsan.reserved"
    package_channel_id = "windows-x64-sanitizer-ubsan"
    target_platform_id = "windows-x64"
    sanitizer = "undefined"
    selected_runtime_variant = "sanitizer=undefined"
    install_selector = "sanitizer=undefined"
    metadata_manifest_path = "share/objc3c/sanitizer/ubsan-metadata.json"
    runtime_library_manifest_path = "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
    runtime_library_payload_entries = @(
      "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
      "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib"
    )
    runtime_library_ids = @("objc3-runtime", "clang_rt.ubsan")
    compiler_flags = @("-fsanitize=undefined", "-fno-omit-frame-pointer")
    linker_flags = @("-fsanitize=undefined")
    environment = "UBSAN_OPTIONS"
    trap_or_recover_mode = "trap"
    missing_runtime_behavior = "fail-closed-before-package-install"
  }
}

function Get-RunnableToolchainPackageSanitizerRuntimeRoot {
  param(
    [Parameter(Mandatory = $true)][string]$LlvmRoot,
    [Parameter(Mandatory = $true)][string]$SanitizerVariant
  )

  $clangRoot = Join-Path $LlvmRoot "lib\clang"
  if (!(Test-Path -LiteralPath $clangRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: sanitizer runtime clang root missing before package install: $clangRoot"
  }

  $versionDirs = @(
    Get-ChildItem -LiteralPath $clangRoot -Directory -ErrorAction Stop |
      Sort-Object -Property Name -Descending
  )
  foreach ($versionDir in $versionDirs) {
    $candidate = Join-Path $versionDir.FullName "lib\windows"
    if (Test-Path -LiteralPath $candidate -PathType Container) {
      return $candidate
    }
  }

  throw "runnable toolchain package FAIL: sanitizer runtime library root missing before package install for $SanitizerVariant under $clangRoot"
}

function Assert-RunnableToolchainPackageSanitizerRuntimePreflight {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  $metadata = Get-RunnableToolchainPackageSanitizerVariantMetadata -SanitizerVariant $SanitizerVariant
  if ($null -eq $metadata) {
    return
  }

  $toolchain = Resolve-Objc3cNativeToolchain -RepoRoot $RepoRoot
  $runtimeRoot = Get-RunnableToolchainPackageSanitizerRuntimeRoot `
    -LlvmRoot $toolchain.LlvmRoot `
    -SanitizerVariant $SanitizerVariant

  foreach ($relativePath in @($metadata["runtime_library_payload_entries"])) {
    $fileName = Split-Path -Leaf $relativePath
    $sourcePath = Join-Path $runtimeRoot $fileName
    if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
      throw "runnable toolchain package FAIL: sanitizer runtime library missing before native build for ${SanitizerVariant}: $fileName in $runtimeRoot"
    }
  }
}

function Copy-RunnableToolchainPackageSanitizerRuntimeLibraries {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  $metadata = Get-RunnableToolchainPackageSanitizerVariantMetadata -SanitizerVariant $SanitizerVariant
  if ($null -eq $metadata) {
    return
  }

  $toolchain = Resolve-Objc3cNativeToolchain -RepoRoot $RepoRoot
  $runtimeRoot = Get-RunnableToolchainPackageSanitizerRuntimeRoot `
    -LlvmRoot $toolchain.LlvmRoot `
    -SanitizerVariant $SanitizerVariant

  $runtimeArtifacts = @()
  foreach ($relativePath in @($metadata["runtime_library_payload_entries"])) {
    $fileName = Split-Path -Leaf $relativePath
    $sourcePath = Join-Path $runtimeRoot $fileName
    if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
      throw "runnable toolchain package FAIL: sanitizer runtime library missing before package install for ${SanitizerVariant}: $fileName in $runtimeRoot"
    }

    $targetPath = Join-Path $PackageRoot ($relativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
    $targetDir = Split-Path -Parent $targetPath
    New-Item -ItemType Directory -Force -Path $targetDir | Out-Null
    Copy-Item -LiteralPath $sourcePath -Destination $targetPath -Force
    $targetItem = Get-Item -LiteralPath $targetPath
    $runtimeArtifacts += [ordered]@{
      runtime_library_id = if ($SanitizerVariant -eq "address") { "clang_rt.asan" } else { "clang_rt.ubsan" }
      artifact = $relativePath
      source_file_name = $fileName
      size_bytes = [int64]$targetItem.Length
      sha256 = (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash.ToLowerInvariant()
      install_required = $true
    }
  }

  $manifestPath = Join-Path $PackageRoot ($metadata["runtime_library_manifest_path"] -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  $manifestDir = Split-Path -Parent $manifestPath
  New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null
  $manifest = [ordered]@{
    contract_id = "objc3c.sanitizer.runtime-library-manifest.v1"
    target_platform_id = $metadata["target_platform_id"]
    sanitizer = $SanitizerVariant
    runtime_library_ids = $metadata["runtime_library_ids"]
    runtime_library_artifacts = $runtimeArtifacts
    runtime_library_root_kind = "llvm-clang-runtime-windows-x64"
    missing_runtime_behavior = $metadata["missing_runtime_behavior"]
    support_truth = $false
    native_execution_claimed = $false
  }
  $manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $manifestPath -Encoding utf8
}

function Assert-RunnableToolchainPackageSanitizerRuntimeManifestPayload {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryManifestPath,
    [Parameter(Mandatory = $true)]$Payload,
    [ValidateSet("address", "undefined")]
    [Parameter(Mandatory = $true)][string]$SanitizerVariant
  )

  $metadata = Get-RunnableToolchainPackageSanitizerVariantMetadata -SanitizerVariant $SanitizerVariant
  if ([string]($Payload["contract_id"]) -ne "objc3c.sanitizer.runtime-library-manifest.v1" -or
      [string]($Payload["target_platform_id"]) -ne [string]($metadata["target_platform_id"]) -or
      [string]($Payload["sanitizer"]) -ne $SanitizerVariant -or
      [string]($Payload["runtime_library_root_kind"]) -ne "llvm-clang-runtime-windows-x64" -or
      [string]($Payload["missing_runtime_behavior"]) -ne [string]($metadata["missing_runtime_behavior"]) -or
      $Payload["support_truth"] -ne $false -or
      $Payload["native_execution_claimed"] -ne $false) {
    throw "runnable toolchain package FAIL: sanitizer runtime library manifest identity drifted: $RuntimeLibraryManifestPath"
  }

  $expectedRuntimeLibraryIds = @($metadata["runtime_library_ids"])
  $actualRuntimeLibraryIds = @($Payload["runtime_library_ids"])
  if (($actualRuntimeLibraryIds -join "`0") -ne ($expectedRuntimeLibraryIds -join "`0")) {
    throw "runnable toolchain package FAIL: sanitizer runtime library ids drifted: $RuntimeLibraryManifestPath"
  }

  $runtimeArtifacts = @($Payload["runtime_library_artifacts"])
  $expectedArtifacts = @($metadata["runtime_library_payload_entries"])
  if ($runtimeArtifacts.Count -ne $expectedArtifacts.Count) {
    throw "runnable toolchain package FAIL: sanitizer runtime library artifact count drifted: $RuntimeLibraryManifestPath"
  }

  $expectedRuntimeLibraryId = if ($SanitizerVariant -eq "address") { "clang_rt.asan" } else { "clang_rt.ubsan" }
  foreach ($relativePath in $expectedArtifacts) {
    $artifactRecord = @($runtimeArtifacts | Where-Object { [string]($_["artifact"]) -eq [string]$relativePath })
    if ($artifactRecord.Count -ne 1) {
      throw "runnable toolchain package FAIL: sanitizer runtime library artifact path drifted: $relativePath"
    }
    $artifact = $artifactRecord[0]
    $fileName = Split-Path -Leaf $relativePath
    $targetPath = Join-Path $PackageRoot ($relativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
    if ([string]($artifact["runtime_library_id"]) -ne $expectedRuntimeLibraryId -or
        [string]($artifact["source_file_name"]) -ne $fileName -or
        [int64]($artifact["size_bytes"]) -lt 1 -or
        $artifact["install_required"] -ne $true -or
        [string]($artifact["sha256"]) -notmatch '^[0-9a-f]{64}$' -or
        !(Test-Path -LiteralPath $targetPath -PathType Leaf)) {
      throw "runnable toolchain package FAIL: sanitizer runtime library artifact metadata drifted: $relativePath"
    }
    $expectedHash = (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ([string]($artifact["sha256"]) -ne $expectedHash) {
      throw "runnable toolchain package FAIL: sanitizer runtime library artifact digest drifted: $relativePath"
    }
  }
}

function Get-RunnableToolchainPackageSanitizerRuntimeManifestPayload {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryManifestPath,
    [ValidateSet("address", "undefined")]
    [Parameter(Mandatory = $true)][string]$SanitizerVariant
  )

  $manifestPath = Join-Path $PackageRoot ($RuntimeLibraryManifestPath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: sanitizer runtime library manifest missing before metadata publication: $RuntimeLibraryManifestPath"
  }
  $payload = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json -AsHashtable
  Assert-RunnableToolchainPackageSanitizerRuntimeManifestPayload `
    -PackageRoot $PackageRoot `
    -RuntimeLibraryManifestPath $RuntimeLibraryManifestPath `
    -Payload $payload `
    -SanitizerVariant $SanitizerVariant
  return $payload
}

function Write-RunnableToolchainPackageSanitizerMetadata {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  $metadata = Get-RunnableToolchainPackageSanitizerVariantMetadata -SanitizerVariant $SanitizerVariant
  if ($null -eq $metadata) {
    return
  }

  $metadata["contract_id"] = "objc3c.sanitizer.runtime-package-metadata.v1"
  $runtimeManifest = Get-RunnableToolchainPackageSanitizerRuntimeManifestPayload `
    -PackageRoot $PackageRoot `
    -RuntimeLibraryManifestPath $metadata["runtime_library_manifest_path"] `
    -SanitizerVariant $SanitizerVariant
  $runtimeManifestPath = Join-Path $PackageRoot ($metadata["runtime_library_manifest_path"] -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  $metadata["runtime_library_manifest_digest"] = "sha256:" + (Get-FileHash -LiteralPath $runtimeManifestPath -Algorithm SHA256).Hash.ToLowerInvariant()
  $metadata["runtime_library_artifacts"] = $runtimeManifest["runtime_library_artifacts"]
  $metadata["support_truth"] = $false
  $metadata["native_execution_claimed"] = $false
  $nativeExecutionContract = [ordered]@{
    native_execution_required_before_support = $true
    native_execution_record_required = $true
    native_execution_record_fields = @("executable_path", "target_platform_id", "sanitizer", "runtime_library_ids", "runtime_library_artifacts", "environment", "exit_code", "diagnostic_records")
    missing_native_execution_behavior = "fail-closed-before-support-promotion"
    native_execution_claimed = $false
  }
  if ($SanitizerVariant -eq "undefined") {
    $nativeExecutionContract["native_execution_record_fields"] += "trap_or_recover_mode"
  }
  $metadata["native_execution_contract"] = $nativeExecutionContract
  $metadata["default_release_channel_allowed"] = $false
  $metadata["release_runtime_mixing_allowed"] = $false

  $metadataPath = Join-Path $PackageRoot ($metadata["metadata_manifest_path"] -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  $metadataDir = Split-Path -Parent $metadataPath
  New-Item -ItemType Directory -Force -Path $metadataDir | Out-Null
  $metadata | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $metadataPath -Encoding utf8
}

function Test-RunnableToolchainPackageGeneratedArtifactPath {
  param([Parameter(Mandatory = $true)][string]$RelativePath)

  $normalized = $RelativePath.Replace('\', '/')
  return @(Get-RunnableToolchainPackageGeneratedArtifactPaths) -contains $normalized
}

function Convert-RunnableToolchainPackageGeneratedPathString {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$Value
  )

  $normalized = $Value.Replace('\', '/')
  $packageRelativeRoot = (Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $PackageRoot).Replace('\', '/')
  $packageFullRoot = [System.IO.Path]::GetFullPath($PackageRoot).Replace('\', '/').TrimEnd('/')
  foreach ($prefix in @($packageRelativeRoot, $packageFullRoot)) {
    if ([string]::IsNullOrWhiteSpace($prefix)) {
      continue
    }
    $prefixWithSlash = $prefix + "/"
    if ($normalized.StartsWith($prefixWithSlash, [System.StringComparison]::OrdinalIgnoreCase)) {
      return $normalized.Substring($prefixWithSlash.Length)
    }
  }
  return $Value
}

function Convert-RunnableToolchainPackageGeneratedPayloadPaths {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)]$Value
  )

  if ($Value -is [string]) {
    return Convert-RunnableToolchainPackageGeneratedPathString `
      -RepoRoot $RepoRoot `
      -PackageRoot $PackageRoot `
      -Value $Value
  }

  if ($Value -is [System.Collections.IDictionary]) {
    foreach ($key in @($Value.Keys)) {
      $Value[$key] = Convert-RunnableToolchainPackageGeneratedPayloadPaths `
        -RepoRoot $RepoRoot `
        -PackageRoot $PackageRoot `
        -Value $Value[$key]
    }
    return $Value
  }

  if ($Value -is [array]) {
    for ($index = 0; $index -lt $Value.Count; $index++) {
      $Value[$index] = Convert-RunnableToolchainPackageGeneratedPayloadPaths `
        -RepoRoot $RepoRoot `
        -PackageRoot $PackageRoot `
        -Value $Value[$index]
    }
    return $Value
  }

  return $Value
}

function Write-RunnableToolchainPackageJsonPayload {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)]$Payload
  )

  $Payload | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $Path -Encoding utf8
}

function Normalize-RunnableToolchainPackageFrontendArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $artifactRoot = Join-Path $PackageRoot "tmp/artifacts/objc3c-native"
  if (!(Test-Path -LiteralPath $artifactRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: isolated build did not publish frontend artifacts: $artifactRoot"
  }

  foreach ($artifactPath in @(Get-ChildItem -LiteralPath $artifactRoot -Filter "*.json" -File)) {
    $payload = Get-Content -LiteralPath $artifactPath.FullName -Raw | ConvertFrom-Json -AsHashtable
    $payload = Convert-RunnableToolchainPackageGeneratedPayloadPaths `
      -RepoRoot $RepoRoot `
      -PackageRoot $PackageRoot `
      -Value $payload
    Write-RunnableToolchainPackageJsonPayload -Path $artifactPath.FullName -Payload $payload
  }

  $sourceGraphPath = Join-Path $artifactRoot "frontend_source_graph.json"
  $invocationLockPath = Join-Path $artifactRoot "frontend_invocation_lock.json"
  if ((Test-Path -LiteralPath $sourceGraphPath -PathType Leaf) -and (Test-Path -LiteralPath $invocationLockPath -PathType Leaf)) {
    $invocationLock = Get-Content -LiteralPath $invocationLockPath -Raw | ConvertFrom-Json -AsHashtable
    $invocationLock["scaffold"]["sha256"] = (Get-FileHash -LiteralPath $sourceGraphPath -Algorithm SHA256).Hash.ToLowerInvariant()
    Write-RunnableToolchainPackageJsonPayload -Path $invocationLockPath -Payload $invocationLock
  }
}

function Copy-RunnableToolchainPackageRepoSupercleanSurface {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$BuildDir
  )

  $sourcePath = Join-Path $BuildDir "repo_superclean_source_of_truth.json"
  if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: isolated build did not publish repo superclean surface: $sourcePath"
  }

  $payload = Get-Content -LiteralPath $sourcePath -Raw | ConvertFrom-Json -AsHashtable
  $payload["native_build_outputs"]["native_executable"] = "artifacts/bin/objc3c-native.exe"
  $payload["native_build_outputs"]["frontend_c_api_runner"] = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  $payload["native_build_outputs"]["runtime_library"] = "artifacts/lib/objc3_runtime.lib"
  $payload["native_build_outputs"]["compile_commands"] = "tmp/build-objc3c-native/compile_commands.json"

  foreach ($entry in @($payload["frontend_contract_artifacts"])) {
    $artifactPath = [string]$entry["artifact_path"]
    $packageRelativeRoot = (Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $PackageRoot).Replace('\', '/')
    $prefix = $packageRelativeRoot + "/"
    if ($artifactPath.StartsWith($prefix, [System.StringComparison]::OrdinalIgnoreCase)) {
      $entry["artifact_path"] = $artifactPath.Substring($prefix.Length)
    }
  }

  $destinationPath = Join-Path $PackageRoot "tmp/build-objc3c-native/repo_superclean_source_of_truth.json"
  $destinationDir = Split-Path -Parent $destinationPath
  New-Item -ItemType Directory -Force -Path $destinationDir | Out-Null
  $payload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $destinationPath -Encoding utf8
}

function Remove-RunnableToolchainPackagePrivateBuildRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $privateBuildRoot = Get-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  if (!(Test-Path -LiteralPath $privateBuildRoot -PathType Container)) {
    return
  }
  Assert-RunnableToolchainPackageTreeHasNoReparsePoints -PackageRoot $privateBuildRoot
  Remove-Item -LiteralPath $privateBuildRoot -Recurse -Force
}

function Invoke-RunnableToolchainPackageBuild {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$BuildScript,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release",
    [int]$Parallelism = 0
  )

  $privateBuildRoot = Get-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  Remove-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  $buildDir = Join-Path $privateBuildRoot "b"
  $summaryPath = Join-Path $privateBuildRoot "native_build_summary.json"
  $runtimeOutputDir = Join-Path $PackageRoot "artifacts/bin"
  $libraryOutputDir = Join-Path $PackageRoot "artifacts/lib"
  $frontendArtifactRoot = Join-Path $PackageRoot "tmp/artifacts/objc3c-native"

  Assert-RunnableToolchainPackageSanitizerRuntimePreflight `
    -RepoRoot $RepoRoot `
    -SanitizerVariant $SanitizerVariant

  & $BuildScript `
    -ExecutionMode full `
    -BuildDir $buildDir `
    -RuntimeOutputDir $runtimeOutputDir `
    -LibraryOutputDir $libraryOutputDir `
    -FrontendArtifactRoot $frontendArtifactRoot `
    -SummaryPath $summaryPath `
    -SanitizerVariant $SanitizerVariant `
    -Parallelism $Parallelism |
    ForEach-Object { Write-Host $_ }
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }

  Normalize-RunnableToolchainPackageFrontendArtifacts `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  Copy-RunnableToolchainPackageRepoSupercleanSurface `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -BuildDir $buildDir
  Copy-RunnableToolchainPackageSanitizerRuntimeLibraries `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -SanitizerVariant $SanitizerVariant
  Write-RunnableToolchainPackageSanitizerMetadata `
    -PackageRoot $PackageRoot `
    -SanitizerVariant $SanitizerVariant
  Remove-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
}

function Get-RunnableToolchainPackageOwnedRunRoot {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return [System.IO.Path]::GetFullPath(
    (Join-Path $RepoRoot "tmp/pkg/objc3c-native-runnable-toolchain")
  ).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
}

function Test-RunnableToolchainPackagePathIsUnderOwnedRunRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $ownedRoot = Get-RunnableToolchainPackageOwnedRunRoot -RepoRoot $RepoRoot
  $packageRootFullPath = [System.IO.Path]::GetFullPath($PackageRoot).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))

  return $packageRootFullPath.StartsWith(
    $ownedRoot + [System.IO.Path]::DirectorySeparatorChar,
    [System.StringComparison]::OrdinalIgnoreCase
  )
}

function Test-RunnableToolchainPackageRootIsReparsePoint {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  $packageRootItem = Get-Item -LiteralPath $PackageRoot -Force
  return ($packageRootItem.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0
}

function Assert-RunnableToolchainPackagePathHasNoReparseAncestor {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $ownedRoot = Get-RunnableToolchainPackageOwnedRunRoot -RepoRoot $RepoRoot
  $packageRootFullPath = [System.IO.Path]::GetFullPath($PackageRoot).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
  $relativePath = [System.IO.Path]::GetRelativePath($ownedRoot, $packageRootFullPath)
  $currentPath = $ownedRoot

  if ((Test-Path -LiteralPath $currentPath) -and (Test-RunnableToolchainPackageRootIsReparsePoint -PackageRoot $currentPath)) {
    throw "runnable toolchain package FAIL: refusing to stage through reparse-point owned root: $currentPath"
  }

  foreach ($segment in @($relativePath -split '[\\/]+')) {
    if ([string]::IsNullOrWhiteSpace($segment) -or $segment -eq ".") {
      continue
    }
    if ($segment -eq "..") {
      throw "runnable toolchain package FAIL: package root escaped owned run root: $PackageRoot"
    }
    $currentPath = Join-Path $currentPath $segment
    if ((Test-Path -LiteralPath $currentPath) -and (Test-RunnableToolchainPackageRootIsReparsePoint -PackageRoot $currentPath)) {
      throw "runnable toolchain package FAIL: refusing to stage through reparse-point package ancestor: $currentPath"
    }
  }
}

function Assert-RunnableToolchainPackageTreeHasNoReparsePoints {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  if (!(Test-Path -LiteralPath $PackageRoot -PathType Container)) {
    return
  }
  if (Test-RunnableToolchainPackageRootIsReparsePoint -PackageRoot $PackageRoot) {
    throw "runnable toolchain package FAIL: refusing to clean reparse-point package root: $PackageRoot"
  }

  $reparseChild = Get-ChildItem `
    -LiteralPath $PackageRoot `
    -Force `
    -Recurse `
    -Attributes ReparsePoint `
    -ErrorAction Stop |
    Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "runnable toolchain package FAIL: refusing to recursively clean package tree containing reparse point: $($reparseChild.FullName)"
  }
}

function Initialize-RunnableToolchainPackageRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $isUnderOwnedRunRoot = Test-RunnableToolchainPackagePathIsUnderOwnedRunRoot -RepoRoot $RepoRoot -PackageRoot $PackageRoot
  if ($isUnderOwnedRunRoot) {
    Assert-RunnableToolchainPackagePathHasNoReparseAncestor -RepoRoot $RepoRoot -PackageRoot $PackageRoot
  }

  if (!(Test-Path -LiteralPath $PackageRoot -PathType Container)) {
    New-Item -ItemType Directory -Force -Path $PackageRoot | Out-Null
    return
  }

  if ($isUnderOwnedRunRoot) {
    Assert-RunnableToolchainPackageTreeHasNoReparsePoints -PackageRoot $PackageRoot
    Remove-Item -LiteralPath $PackageRoot -Recurse -Force
    New-Item -ItemType Directory -Force -Path $PackageRoot | Out-Null
    return
  }

  $existingEntry = Get-ChildItem -LiteralPath $PackageRoot -Force | Select-Object -First 1
  if ($null -ne $existingEntry) {
    throw "runnable toolchain package FAIL: package root must be empty for clean-room staging: $PackageRoot"
  }
}

function Get-RunnableToolchainPackageInputFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return @(
    @(Get-RequiredRunnableToolchainPackageFiles) +
    @(Get-RepoRelativeNativeCompileSupportFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeExecutionSupportFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeFixtureFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeExecutionFixtureFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeDocsFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeRuntimeSourceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePythonSharedFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePythonToolingFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeConformanceSurfacePythonFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeWorkflowPythonFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePerformanceBenchmarkFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRuntimeAcceptanceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRuntimeProbeFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRecoveryPositiveFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeStdlibFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeConformanceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePackagedPythonScriptFiles -RepoRoot $RepoRoot)
  ) |
    Where-Object { !(Test-RunnableToolchainPackageGeneratedArtifactPath -RelativePath $_) } |
    Sort-Object -Unique
}

function Copy-RunnableToolchainPackageInputs {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string[]]$RelativePaths
  )

  $copiedRelativePaths = New-Object System.Collections.Generic.List[string]
  foreach ($relativePath in $RelativePaths) {
    Copy-RepoRelativeFile -RepoRoot $RepoRoot -PackageRoot $PackageRoot -RelativePath $relativePath | Out-Null
    $copiedRelativePaths.Add($relativePath.Replace('\', '/')) | Out-Null
  }

  return @($copiedRelativePaths)
}

function Get-RunnableToolchainStagedRelativePaths {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath
  )

  return @(
    Get-ChildItem -LiteralPath $PackageRoot -Recurse -File |
      Where-Object { $_.FullName -ne $ManifestPath } |
      Sort-Object -Property FullName |
      ForEach-Object { (Get-RepoRelativePathCompat -RootPath $PackageRoot -TargetPath $_.FullName).Replace('\', '/') }
  )
}

function Set-RunnableToolchainPackagedOutputTimestamps {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  $packagedNativeExecutablePath = Join-Path $PackageRoot "artifacts\bin\objc3c-native.exe"
  $packagedFrontendRunnerPath = Join-Path $PackageRoot "artifacts\bin\objc3c-frontend-c-api-runner.exe"
  $packagedRuntimeLibraryPath = Join-Path $PackageRoot "artifacts\lib\objc3_runtime.lib"
  $normalizedOutputTimestamp = [datetime]::UtcNow
  foreach ($outputPath in @($packagedNativeExecutablePath, $packagedFrontendRunnerPath, $packagedRuntimeLibraryPath)) {
    if (Test-Path -LiteralPath $outputPath -PathType Leaf) {
      $item = Get-Item -LiteralPath $outputPath
      $item.LastWriteTimeUtc = $normalizedOutputTimestamp
    }
  }
}

function Invoke-RunnableToolchainPackageStaging {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [Parameter(Mandatory = $true)][string]$BuildScript,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release",
    [int]$Parallelism = 0
  )

  Initialize-RunnableToolchainPackageRoot -RepoRoot $RepoRoot -PackageRoot $PackageRoot
  Invoke-RunnableToolchainPackageBuild `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -BuildScript $BuildScript `
    -SanitizerVariant $SanitizerVariant `
    -Parallelism $Parallelism
  $inputFiles = @(Get-RunnableToolchainPackageInputFiles -RepoRoot $RepoRoot)
  $copiedRelativePaths = @(Copy-RunnableToolchainPackageInputs `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -RelativePaths $inputFiles)
  $stagedRelativePaths = @(Get-RunnableToolchainStagedRelativePaths `
    -PackageRoot $PackageRoot `
    -ManifestPath $ManifestPath)
  Set-RunnableToolchainPackagedOutputTimestamps -PackageRoot $PackageRoot

  return [pscustomobject]@{
    CopiedRelativePaths = $copiedRelativePaths
    StagedRelativePaths = $stagedRelativePaths
  }
}

Export-ModuleMember -Function @(
  "Invoke-RunnableToolchainPackageBuild",
  "Invoke-RunnableToolchainPackageStaging"
)
