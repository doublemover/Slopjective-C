$script:Objc3cRuntimeLaunchIntegrationContractId = "objc3c.runtime.launch.integration.v1"
$script:Objc3cRuntimeLaunchCompileWrapperCommandSurface = "scripts/objc3c_native_compile.ps1"
$script:Objc3cRuntimeLaunchCompileProofCommandSurface = "scripts/run_objc3c_native_compile_proof.ps1"
$script:Objc3cRuntimeLaunchExecutionSmokeCommandSurface = "scripts/check_objc3c_native_execution_smoke.ps1"
$script:Objc3cRuntimeLaunchArchiveResolutionModel = "registration-manifest-runtime-archive-path-is-authoritative"
$script:Objc3cRuntimeLaunchDriverLinkerFlagConsumptionModel = "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands"

function Get-Objc3cLaunchContractRepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath,
    [Parameter(Mandatory = $true)][string]$Label
  )

  if ([string]::IsNullOrWhiteSpace($RelativePath)) {
    throw "runtime launch contract FAIL: missing $Label relative path"
  }

  $normalized = $RelativePath.Replace('\\', '/').Trim()
  foreach ($segment in $normalized.Split('/')) {
    if ($segment -eq '..') {
      throw "runtime launch contract FAIL: $Label escapes repo root: $RelativePath"
    }
  }

  $repoRootResolved = (Resolve-Path -LiteralPath $RepoRoot).Path
  return [System.IO.Path]::GetFullPath((Join-Path $repoRootResolved $normalized.Replace('/', '\\')))
}

function Read-Objc3cJsonArtifact {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Label
  )

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "runtime launch contract FAIL: missing $Label at $Path"
  }

  try {
    return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  }
  catch {
    throw "runtime launch contract FAIL: invalid json in $Label at $Path"
  }
}

function Get-Objc3cRuntimeLaunchContract {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EmitPrefix = "module",
    [string]$DefaultRuntimeLibraryRelativePath = "artifacts/lib/objc3_runtime.lib"
  )

  if (!(Test-Path -LiteralPath $CompileDir -PathType Container)) {
    throw "runtime launch contract FAIL: compile directory missing at $CompileDir"
  }

  $repoRootResolved = (Resolve-Path -LiteralPath $RepoRoot).Path
  $compileDirResolved = (Resolve-Path -LiteralPath $CompileDir).Path
  $registrationManifestPath = Join-Path $compileDirResolved ($EmitPrefix + ".runtime-registration-manifest.json")
  $mainManifestPath = Join-Path $compileDirResolved ($EmitPrefix + ".manifest.json")

  $registrationManifest = Read-Objc3cJsonArtifact -Path $registrationManifestPath -Label "runtime registration manifest"
  $mainManifest = Read-Objc3cJsonArtifact -Path $mainManifestPath -Label "main manifest"
  $semanticRegistrationManifest = $mainManifest.frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_manifest

  $launchContractId = "$($registrationManifest.launch_integration_contract_id)".Trim()
  if ($launchContractId -ne $script:Objc3cRuntimeLaunchIntegrationContractId) {
    throw "runtime launch contract FAIL: launch_integration_contract_id mismatch in $registrationManifestPath"
  }

  $expectedCommandSurfaces = [ordered]@{
    compile_wrapper_command_surface = $script:Objc3cRuntimeLaunchCompileWrapperCommandSurface
    compile_proof_command_surface = $script:Objc3cRuntimeLaunchCompileProofCommandSurface
    execution_smoke_command_surface = $script:Objc3cRuntimeLaunchExecutionSmokeCommandSurface
  }
  foreach ($propertyName in $expectedCommandSurfaces.Keys) {
    $actualValue = "$($registrationManifest.$propertyName)".Trim()
    if ($actualValue -ne [string]$expectedCommandSurfaces[$propertyName]) {
      throw "runtime launch contract FAIL: $propertyName mismatch in $registrationManifestPath"
    }
  }

  if ("$($registrationManifest.runtime_library_resolution_model)".Trim() -ne $script:Objc3cRuntimeLaunchArchiveResolutionModel) {
    throw "runtime launch contract FAIL: runtime_library_resolution_model mismatch in $registrationManifestPath"
  }
  if ("$($registrationManifest.driver_linker_flag_consumption_model)".Trim() -ne $script:Objc3cRuntimeLaunchDriverLinkerFlagConsumptionModel) {
    throw "runtime launch contract FAIL: driver_linker_flag_consumption_model mismatch in $registrationManifestPath"
  }
  if (-not [bool]$registrationManifest.launch_integration_ready) {
    throw "runtime launch contract FAIL: launch_integration_ready is false in $registrationManifestPath"
  }

  $manifestArtifact = "$($registrationManifest.manifest_artifact)".Trim()
  if ($manifestArtifact -ne ($EmitPrefix + ".runtime-registration-manifest.json")) {
    throw "runtime launch contract FAIL: manifest_artifact mismatch in $registrationManifestPath"
  }

  $archiveRelativePath = "$($registrationManifest.runtime_support_library_archive_relative_path)".Trim()
  $bootstrapArchiveRelativePath = "$($registrationManifest.bootstrap_runtime_api_archive_relative_path)".Trim()
  if ([string]::IsNullOrWhiteSpace($archiveRelativePath) -or [string]::IsNullOrWhiteSpace($bootstrapArchiveRelativePath)) {
    throw "runtime launch contract FAIL: runtime archive paths are incomplete in $registrationManifestPath"
  }
  if ($archiveRelativePath.Replace('\\', '/') -ne $bootstrapArchiveRelativePath.Replace('\\', '/')) {
    throw "runtime launch contract FAIL: runtime archive path drift between manifest archive fields in $registrationManifestPath"
  }

  $runtimeLibraryPath = Get-Objc3cLaunchContractRepoRelativePath -RepoRoot $repoRootResolved -RelativePath $archiveRelativePath -Label "runtime archive"
  if (!(Test-Path -LiteralPath $runtimeLibraryPath -PathType Leaf)) {
    throw "runtime launch contract FAIL: runtime library missing at $runtimeLibraryPath"
  }

  $defaultRuntimeLibraryPath = Get-Objc3cLaunchContractRepoRelativePath -RepoRoot $repoRootResolved -RelativePath $DefaultRuntimeLibraryRelativePath -Label "default runtime archive"
  $resolvedRuntimeLibrary = [System.IO.Path]::GetFullPath($runtimeLibraryPath)
  $resolvedDefaultRuntimeLibrary = [System.IO.Path]::GetFullPath($defaultRuntimeLibraryPath)

  $semanticRegistrationPath = "$($semanticRegistrationManifest.manifest_artifact_relative_path)".Trim()
  if ($semanticRegistrationPath -ne ($EmitPrefix + ".runtime-registration-manifest.json")) {
    throw "runtime launch contract FAIL: main manifest registration manifest path mismatch in $mainManifestPath"
  }

  $mainLaunchContractId = "$($semanticRegistrationManifest.launch_integration_contract_id)".Trim()
  if ($mainLaunchContractId -ne $script:Objc3cRuntimeLaunchIntegrationContractId) {
    throw "runtime launch contract FAIL: main manifest launch integration contract id mismatch in $mainManifestPath"
  }
  if ("$($semanticRegistrationManifest.runtime_library_resolution_model)".Trim() -ne $script:Objc3cRuntimeLaunchArchiveResolutionModel) {
    throw "runtime launch contract FAIL: main manifest runtime library resolution model mismatch in $mainManifestPath"
  }
  if ("$($semanticRegistrationManifest.driver_linker_flag_consumption_model)".Trim() -ne $script:Objc3cRuntimeLaunchDriverLinkerFlagConsumptionModel) {
    throw "runtime launch contract FAIL: main manifest driver linker flag model mismatch in $mainManifestPath"
  }
  if ("$($semanticRegistrationManifest.compile_wrapper_command_surface)".Trim() -ne $script:Objc3cRuntimeLaunchCompileWrapperCommandSurface) {
    throw "runtime launch contract FAIL: main manifest compile wrapper command surface mismatch in $mainManifestPath"
  }
  if ("$($semanticRegistrationManifest.compile_proof_command_surface)".Trim() -ne $script:Objc3cRuntimeLaunchCompileProofCommandSurface) {
    throw "runtime launch contract FAIL: main manifest compile proof command surface mismatch in $mainManifestPath"
  }
  if ("$($semanticRegistrationManifest.execution_smoke_command_surface)".Trim() -ne $script:Objc3cRuntimeLaunchExecutionSmokeCommandSurface) {
    throw "runtime launch contract FAIL: main manifest execution smoke command surface mismatch in $mainManifestPath"
  }
  if (-not [bool]$semanticRegistrationManifest.launch_integration_ready) {
    throw "runtime launch contract FAIL: main manifest launch integration ready flag is false in $mainManifestPath"
  }

  $driverLinkerFlags = @($registrationManifest.driver_linker_flags)
  if ($driverLinkerFlags.Count -eq 0) {
    throw "runtime launch contract FAIL: driver_linker_flags missing from $registrationManifestPath"
  }

  return [pscustomobject]@{
    registration_manifest_path = $registrationManifestPath
    registration_manifest_relative_path = ($EmitPrefix + ".runtime-registration-manifest.json")
    main_manifest_path = $mainManifestPath
    main_manifest_relative_path = ($EmitPrefix + ".manifest.json")
    launch_integration_contract_id = $launchContractId
    runtime_library_path = $resolvedRuntimeLibrary
    runtime_library_relative_path = $archiveRelativePath.Replace('\\', '/')
    runtime_library_source = "registration-manifest.runtime_support_library_archive_relative_path"
    runtime_library_matches_default_archive = ($resolvedRuntimeLibrary -eq $resolvedDefaultRuntimeLibrary)
    translation_unit_identity_key = "$($registrationManifest.translation_unit_identity_key)"
    registration_entrypoint_symbol = "$($registrationManifest.registration_entrypoint_symbol)"
    bootstrap_runtime_api_registration_entrypoint_symbol = "$($registrationManifest.bootstrap_runtime_api_registration_entrypoint_symbol)"
    driver_linker_flags = @($driverLinkerFlags)
    runtime_library_resolution_model = "$($registrationManifest.runtime_library_resolution_model)"
    driver_linker_flag_consumption_model = "$($registrationManifest.driver_linker_flag_consumption_model)"
    compile_wrapper_command_surface = "$($registrationManifest.compile_wrapper_command_surface)"
    compile_proof_command_surface = "$($registrationManifest.compile_proof_command_surface)"
    execution_smoke_command_surface = "$($registrationManifest.execution_smoke_command_surface)"
  }
}

function Assert-Objc3cRuntimeLaunchContract {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$EmitPrefix = "module",
    [string]$DefaultRuntimeLibraryRelativePath = "artifacts/lib/objc3_runtime.lib"
  )

  $null = Get-Objc3cRuntimeLaunchContract `
    -CompileDir $CompileDir `
    -RepoRoot $RepoRoot `
    -EmitPrefix $EmitPrefix `
    -DefaultRuntimeLibraryRelativePath $DefaultRuntimeLibraryRelativePath
}
