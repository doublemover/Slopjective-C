Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "..\objc3c_native_cmake.psm1") -Force -DisableNameChecking

function New-RunnableToolchainPackageNativeExecutionContract {
  param([switch]$IncludesTrapOrRecoverMode)

  $recordFields = @(
    "executable_path",
    "target_platform_id",
    "sanitizer",
    "runtime_library_ids",
    "runtime_library_artifacts",
    "environment",
    "exit_code",
    "diagnostic_records"
  )
  if ($IncludesTrapOrRecoverMode.IsPresent) {
    $recordFields += "trap_or_recover_mode"
  }

  return [ordered]@{
    native_execution_required_before_support = $true
    native_execution_record_required = $true
    native_execution_record_fields = $recordFields
    missing_native_execution_behavior = "fail-closed-before-support-promotion"
    native_execution_claimed = $false
  }
}

function Get-RunnableToolchainPackageSanitizerMetadataDigest {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$MetadataManifestPath
  )

  $metadataPath = Join-Path $PackageRoot ($MetadataManifestPath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  if (!(Test-Path -LiteralPath $metadataPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: sanitizer metadata missing before manifest publication: $MetadataManifestPath"
  }
  return "sha256:" + (Get-FileHash -LiteralPath $metadataPath -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-RunnableToolchainPackageSanitizerRuntimeArtifactPaths {
  param(
    [ValidateSet("address", "undefined")]
    [Parameter(Mandatory = $true)][string]$SanitizerVariant
  )

  if ($SanitizerVariant -eq "address") {
    return @(
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.dll",
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic-x86_64.lib",
      "artifacts/runtime/sanitizer/address/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    )
  }

  return @(
    "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone-x86_64.lib",
    "artifacts/runtime/sanitizer/undefined/clang_rt.ubsan_standalone_cxx-x86_64.lib"
  )
}

function Assert-RunnableToolchainPackageSanitizerRuntimeManifestSection {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryManifestPath,
    [ValidateSet("address", "undefined")]
    [Parameter(Mandatory = $true)][string]$SanitizerVariant,
    [Parameter(Mandatory = $true)]$Payload
  )

  $expectedRuntimeLibraryIds = if ($SanitizerVariant -eq "address") {
    @("objc3-runtime", "clang_rt.asan")
  } else {
    @("objc3-runtime", "clang_rt.ubsan")
  }
  if ([string]($Payload["contract_id"]) -ne "objc3c.sanitizer.runtime-library-manifest.v1" -or
      [string]($Payload["target_platform_id"]) -ne "windows-x64" -or
      [string]($Payload["sanitizer"]) -ne $SanitizerVariant -or
      [string]($Payload["runtime_library_root_kind"]) -ne "llvm-clang-runtime-windows-x64" -or
      [string]($Payload["missing_runtime_behavior"]) -ne "fail-closed-before-package-install" -or
      $Payload["support_truth"] -ne $false -or
      $Payload["native_execution_claimed"] -ne $false -or
      (@($Payload["runtime_library_ids"]) -join "`0") -ne ($expectedRuntimeLibraryIds -join "`0")) {
    throw "runnable toolchain package FAIL: sanitizer runtime library manifest identity drifted: $RuntimeLibraryManifestPath"
  }

  $runtimeArtifacts = @($Payload["runtime_library_artifacts"])
  $expectedArtifacts = @(Get-RunnableToolchainPackageSanitizerRuntimeArtifactPaths -SanitizerVariant $SanitizerVariant)
  if ($runtimeArtifacts.Count -ne $expectedArtifacts.Count) {
    throw "runnable toolchain package FAIL: sanitizer runtime library manifest artifact count drifted: $RuntimeLibraryManifestPath"
  }
  $expectedRuntimeLibraryId = if ($SanitizerVariant -eq "address") { "clang_rt.asan" } else { "clang_rt.ubsan" }
  foreach ($relativePath in $expectedArtifacts) {
    $artifactRecord = @($runtimeArtifacts | Where-Object { [string]($_["artifact"]) -eq [string]$relativePath })
    if ($artifactRecord.Count -ne 1) {
      throw "runnable toolchain package FAIL: sanitizer runtime library manifest artifact path drifted: $relativePath"
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
      throw "runnable toolchain package FAIL: sanitizer runtime library manifest artifact metadata drifted: $relativePath"
    }
    $actualDigest = (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ([string]($artifact["sha256"]) -ne $actualDigest) {
      throw "runnable toolchain package FAIL: sanitizer runtime library manifest artifact digest drifted: $relativePath"
    }
  }
}

function Get-RunnableToolchainPackageSanitizerRuntimeManifestSection {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RuntimeLibraryManifestPath,
    [ValidateSet("address", "undefined")]
    [Parameter(Mandatory = $true)][string]$SanitizerVariant
  )

  $manifestPath = Join-Path $PackageRoot ($RuntimeLibraryManifestPath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
  if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: sanitizer runtime library manifest missing before manifest publication: $RuntimeLibraryManifestPath"
  }
  $payload = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json -AsHashtable
  Assert-RunnableToolchainPackageSanitizerRuntimeManifestSection `
    -PackageRoot $PackageRoot `
    -RuntimeLibraryManifestPath $RuntimeLibraryManifestPath `
    -SanitizerVariant $SanitizerVariant `
    -Payload $payload

  return [ordered]@{
    runtime_library_manifest_path = $RuntimeLibraryManifestPath
    runtime_library_manifest_digest = "sha256:" + (Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash.ToLowerInvariant()
    runtime_library_artifacts = $payload["runtime_library_artifacts"]
    missing_runtime_behavior = $payload["missing_runtime_behavior"]
  }
}

function New-RunnableToolchainPackageFoundationManifestSection {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [ValidateSet("release", "address", "undefined")]
    [string]$SanitizerVariant = "release"
  )

  $coreArtifacts = Get-Objc3cNativePackageArtifactRelativePaths
  $targetPlatformId = Get-Objc3cNativeHostPlatformId
  $runtimeLibraryKind = Get-Objc3cNativeRuntimeLibraryKind
  $runtimeLibraryName = Get-Objc3cNativeRuntimeLibraryFileName
  $objectFormat = Get-Objc3cNativeObjectFormat
  $debugFormat = Get-Objc3cNativeDebugFormat
  $targetTriple = Get-Objc3cNativeTargetTriple

  $sanitizerPackageVariant = [ordered]@{
    selected_runtime_variant = "release"
    support_truth = $false
    native_execution_claimed = $false
  }
  if ($SanitizerVariant -eq "address") {
    $metadataManifestPath = "share/objc3c/sanitizer/asan-metadata.json"
    $runtimeLibraryManifestPath = "share/objc3c/sanitizer/asan-runtime-libraries.json"
    $runtimeLibraryManifestSection = Get-RunnableToolchainPackageSanitizerRuntimeManifestSection `
      -PackageRoot $PackageRoot `
      -RuntimeLibraryManifestPath $runtimeLibraryManifestPath `
      -SanitizerVariant "address"
    $sanitizerPackageVariant = [ordered]@{
      package_id = "org.objc3c.runtime:objc3c-runtime-asan"
      package_variant_row_id = "objc3c.package.sanitizer.asan.reserved"
      package_channel_id = "windows-x64-sanitizer-asan"
      target_platform_id = "windows-x64"
      sanitizer = "address"
      runtime_library_ids = @("objc3-runtime", "clang_rt.asan")
      metadata_manifest_path = $metadataManifestPath
      metadata_digest = (Get-RunnableToolchainPackageSanitizerMetadataDigest `
        -PackageRoot $PackageRoot `
        -MetadataManifestPath $metadataManifestPath)
      runtime_library_manifest_path = $runtimeLibraryManifestSection.runtime_library_manifest_path
      runtime_library_manifest_digest = $runtimeLibraryManifestSection.runtime_library_manifest_digest
      runtime_library_artifacts = $runtimeLibraryManifestSection.runtime_library_artifacts
      missing_runtime_behavior = $runtimeLibraryManifestSection.missing_runtime_behavior
      selected_runtime_variant = "sanitizer=address"
      install_selector = "sanitizer=address"
      compiler_flags = @("-fsanitize=address", "-fno-omit-frame-pointer")
      linker_flags = @("-fsanitize=address")
      environment = "ASAN_OPTIONS"
      native_execution_contract = (New-RunnableToolchainPackageNativeExecutionContract)
      default_release_channel_allowed = $false
      release_runtime_mixing_allowed = $false
      support_truth = $false
      native_execution_claimed = $false
    }
  } elseif ($SanitizerVariant -eq "undefined") {
    $metadataManifestPath = "share/objc3c/sanitizer/ubsan-metadata.json"
    $runtimeLibraryManifestPath = "share/objc3c/sanitizer/ubsan-runtime-libraries.json"
    $runtimeLibraryManifestSection = Get-RunnableToolchainPackageSanitizerRuntimeManifestSection `
      -PackageRoot $PackageRoot `
      -RuntimeLibraryManifestPath $runtimeLibraryManifestPath `
      -SanitizerVariant "undefined"
    $sanitizerPackageVariant = [ordered]@{
      package_id = "org.objc3c.runtime:objc3c-runtime-ubsan"
      package_variant_row_id = "objc3c.package.sanitizer.ubsan.reserved"
      package_channel_id = "windows-x64-sanitizer-ubsan"
      target_platform_id = "windows-x64"
      sanitizer = "undefined"
      runtime_library_ids = @("objc3-runtime", "clang_rt.ubsan")
      metadata_manifest_path = $metadataManifestPath
      metadata_digest = (Get-RunnableToolchainPackageSanitizerMetadataDigest `
        -PackageRoot $PackageRoot `
        -MetadataManifestPath $metadataManifestPath)
      runtime_library_manifest_path = $runtimeLibraryManifestSection.runtime_library_manifest_path
      runtime_library_manifest_digest = $runtimeLibraryManifestSection.runtime_library_manifest_digest
      runtime_library_artifacts = $runtimeLibraryManifestSection.runtime_library_artifacts
      missing_runtime_behavior = $runtimeLibraryManifestSection.missing_runtime_behavior
      selected_runtime_variant = "sanitizer=undefined"
      install_selector = "sanitizer=undefined"
      compiler_flags = @("-fsanitize=undefined", "-fno-omit-frame-pointer")
      linker_flags = @("-fsanitize=undefined")
      environment = "UBSAN_OPTIONS"
      trap_or_recover_mode = "trap"
      native_execution_contract = (New-RunnableToolchainPackageNativeExecutionContract -IncludesTrapOrRecoverMode)
      default_release_channel_allowed = $false
      release_runtime_mixing_allowed = $false
      support_truth = $false
      native_execution_claimed = $false
    }
  }

  return [ordered]@{
    contract_id = "objc3c-runnable-build-install-run-package/runnable_suite-packaged-end-to-end-v1"
    schema_version = 1
    package_model = "staged-runnable-toolchain-bundle-with-repo-relative-layout"
    install_model = "local-package-root-not-system-install"
    runtime_variant = $sanitizerPackageVariant.selected_runtime_variant
    sanitizer_package_variant = $sanitizerPackageVariant
    package_root = Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $PackageRoot
    manifest_artifact = Get-RepoRelativePathCompat -RootPath $PackageRoot -TargetPath $ManifestPath
    support_truth = $false
    native_execution_claimed = $false
    target_platform_id = $targetPlatformId
    target_triple = $targetTriple
    object_format = $objectFormat
    debug_format = $debugFormat
    runtime_library_kind = $runtimeLibraryKind
    runtime_library_name = $runtimeLibraryName
    package_root_layout = @(
      "artifacts/package/objc3c-runnable-toolchain-package.json",
      $coreArtifacts.NativeExecutable,
      $coreArtifacts.RuntimeLibrary,
      "stdlib/workspace.json",
      "stdlib/modules/objc3.core/module.json",
      "docs/runbooks/objc3c_packaging_channels.md"
    )
    native_executable = $coreArtifacts.NativeExecutable
    frontend_c_api_runner = $coreArtifacts.CapiRunnerExecutable
    runtime_library = $coreArtifacts.RuntimeLibrary
    compile_action = "compile-objc3c"
    compile_wrapper = "scripts/objc3c_native_compile.ps1"
    runtime_launch_contract_script = "scripts/objc3c_runtime_launch_contract.ps1"
    compile_proof_script = "scripts/run_objc3c_native_compile_proof.ps1"
    execution_smoke_script = "scripts/check_objc3c_native_execution_smoke.ps1"
    execution_replay_script = "scripts/check_objc3c_execution_replay_proof.ps1"
    showcase_root = "showcase"
    showcase_readme = "showcase/README.md"
    showcase_portfolio = "showcase/portfolio.json"
    showcase_examples = @(
      [ordered]@{
        example_id = "auroraBoard"
        source = "showcase/auroraBoard/main.objc3"
        workspace_manifest = "showcase/auroraBoard/workspace.json"
        expected_exit_code = 33
      },
      [ordered]@{
        example_id = "signalMesh"
        source = "showcase/signalMesh/main.objc3"
        workspace_manifest = "showcase/signalMesh/workspace.json"
        expected_exit_code = 13
      },
      [ordered]@{
        example_id = "patchKit"
        source = "showcase/patchKit/main.objc3"
        workspace_manifest = "showcase/patchKit/workspace.json"
        expected_exit_code = 7
      }
    )
    canonical_runnable_fixture = "tests/tooling/fixtures/native/canonical_runnable_sample_set.objc3"
    runtime_public_header = "native/objc3c/src/runtime/public/objc3_runtime_api.h"
    runtime_internal_header = "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h"
    release_evidence_dashboard_schema = "schemas/objc3-conformance-dashboard-status-v1.schema.json"
    release_evidence_gate_action = "check-release-evidence"
    release_evidence_gate_script = "scripts/check_release_evidence.py"
    release_evidence_runbook = "spec/conformance/release_evidence_gate_maintenance.md"
    release_candidate_fixture = "tests/tooling/fixtures/native/hello.objc3"
    release_candidate_claim_probe = "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp"
    release_candidate_evidence_probe = "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp"
    developer_tooling_runbook = "docs/runbooks/objc3c_developer_tooling.md"
    developer_tooling_boundary_inventory = "tests/tooling/fixtures/developer_tooling/boundary_inventory.json"
    developer_tooling_editor_surface_schema = "schemas/objc3c-developer-tooling-editor-surface-v1.schema.json"
    developer_tooling_navigation_contract = "tests/tooling/fixtures/developer_tooling/language_server_navigation_implementation_contract.json"
    developer_tooling_workspace_semantic_navigation_contract = "tests/tooling/fixtures/developer_tooling/workspace_semantic_navigation_contract.json"
    developer_tooling_formatter_debug_contract = "tests/tooling/fixtures/developer_tooling/formatter_debug_implementation_contract.json"
    developer_tooling_formatter_rewrite_contract = "tests/tooling/fixtures/developer_tooling/formatter_rewrite_contract.json"
    developer_tooling_diagnostic_quality_contract = "tests/tooling/fixtures/developer_tooling/diagnostic_quality_contract.json"
    developer_tooling_workspace_contract = "tests/tooling/fixtures/developer_tooling/workspace_editor_debug_integration_contract.json"
    developer_tooling_packaged_contract = "tests/tooling/fixtures/developer_tooling/packaged_cli_to_editor_contract.json"
    developer_tooling_runtime_debug_trace_script = "scripts/build_objc3c_runtime_debug_trace.py"
    developer_tooling_runtime_debug_trace_schema = "schemas/objc3c-runtime-debug-trace-v1.schema.json"
    developer_tooling_runtime_debug_trace_path = "tmp/reports/objc3c-public-workflow/runtime-debug-trace.json"
    developer_tooling_runtime_debug_trace_model = "deterministic-runtime-inspector-and-editor-debug-artifact-trace"
    developer_tooling_example_source = "tests/tooling/fixtures/native/hello.objc3"
    developer_tooling_negative_source = "tests/tooling/fixtures/native/negative_undefined_symbol.objc3"
    developer_tooling_formatter_source = "tests/tooling/fixtures/developer_tooling/messy_hello.objc3"
    developer_tooling_expected_formatted_source = "tests/tooling/fixtures/developer_tooling/formatted_hello.objc3"
    developer_tooling_scripts = [ordered]@{
      editor_surface = "scripts/build_objc3c_editor_tooling_surface.py"
      formatter = "scripts/format_objc3c_source.py"
      source_rewrite = "scripts/rewrite_objc3c_source.py"
      language_server_navigation_validation = "scripts/check_developer_tooling_language_server_navigation.py"
      formatter_debug_validation = "scripts/check_developer_tooling_formatter_debug_surface.py"
      formatter_rewrite_validation = "scripts/check_developer_tooling_formatter_rewrite_surface.py"
      diagnostic_quality_validation = "scripts/check_developer_tooling_diagnostic_quality.py"
      workspace_validation = "scripts/check_developer_tooling_workspace_integration.py"
      runtime_debug_trace = "scripts/build_objc3c_runtime_debug_trace.py"
      integration_validation = "scripts/check_objc3c_developer_tooling_integration.py"
      runnable_end_to_end_validation = "scripts/check_objc3c_runnable_developer_tooling_end_to_end.py"
    }
    developer_tooling_public_actions = @(
      "inspect-editor-tooling",
      "format-objc3c",
      "materialize-playground-workspace",
      "trace-runtime-debug",
      "validate-developer-tooling",
      "validate-runnable-developer-tooling"
    )
    package_bridge = "objc3c"
  }
}

Export-ModuleMember -Function @(
  "New-RunnableToolchainPackageFoundationManifestSection"
)
