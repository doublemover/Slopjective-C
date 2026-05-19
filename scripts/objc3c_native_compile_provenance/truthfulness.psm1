$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "text_analysis.psm1") -Force -DisableNameChecking

function Get-CompileOutputTruthfulness {
  param(
    [string]$CompileDir,
    [string]$EmitPrefix
  )

  if ([string]::IsNullOrWhiteSpace($CompileDir) -or !(Test-Path -LiteralPath $CompileDir -PathType Container)) {
    throw "compile output truthfulness check requires an existing compile directory"
  }
  if ([string]::IsNullOrWhiteSpace($EmitPrefix)) {
    $EmitPrefix = "module"
  }

  $manifestPath = Join-Path $CompileDir ($EmitPrefix + ".manifest.json")
  $registrationManifestPath = Join-Path $CompileDir ($EmitPrefix + ".runtime-registration-manifest.json")
  $llvmIrPath = Join-Path $CompileDir ($EmitPrefix + ".ll")
  foreach ($requiredPath in @($manifestPath, $registrationManifestPath, $llvmIrPath)) {
    if (!(Test-Path -LiteralPath $requiredPath -PathType Leaf)) {
      throw "compile output truthfulness check missing required artifact '$requiredPath'"
    }
  }

  $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json -AsHashtable
  $registrationManifest = Get-Content -LiteralPath $registrationManifestPath -Raw | ConvertFrom-Json -AsHashtable
  $llvmIrText = Get-Content -LiteralPath $llvmIrPath -Raw

  $lowering = $manifest["lowering"]
  $propertySynthesis = $manifest["lowering_property_synthesis_ivar_binding"]
  $runtimeDispatchSymbol = ""
  if ($lowering -is [System.Collections.IDictionary]) {
    $runtimeDispatchSymbol = [string]$lowering["runtime_dispatch_symbol"]
  }
  if ([string]::IsNullOrWhiteSpace($runtimeDispatchSymbol)) {
    $runtimeDispatchSymbol = [string]$manifest["runtime_support_library_link_wiring_runtime_dispatch_symbol"]
  }
  if ([string]::IsNullOrWhiteSpace($runtimeDispatchSymbol)) {
    $runtimeDispatchSymbol = [string]$manifest["runtime_link_host_link_runtime_dispatch_symbol"]
  }
  if ([string]::IsNullOrWhiteSpace($runtimeDispatchSymbol)) {
    throw "compile output truthfulness check could not resolve the runtime dispatch symbol from the compile manifest"
  }

  $propertyDescriptorCountExpected = [int]$registrationManifest["property_descriptor_count"]
  $ivarDescriptorCountExpected = [int]$registrationManifest["ivar_descriptor_count"]
  $propertySynthesisSitesExpected = 0
  if ($propertySynthesis -is [System.Collections.IDictionary]) {
    $propertySynthesisSitesExpected = Get-ReplayKeyCounter -ReplayKey ([string]$propertySynthesis["replay_key"]) -CounterName "property_synthesis_sites"
  }

  $dispatchDeclarationCount = Get-RegexMatchCount -Text $llvmIrText -Pattern ("(?m)declare i32 @" + [regex]::Escape($runtimeDispatchSymbol) + "\(")
  $dispatchCallCount = Get-RegexMatchCount -Text $llvmIrText -Pattern ("(?m)call i32 @" + [regex]::Escape($runtimeDispatchSymbol) + "\(")
  $propertyDescriptorDefinitionCount = Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_meta_property_[0-9]+ = "
  $ivarDescriptorDefinitionCount = Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_meta_ivar_[0-9]+ = "
  $propertyDescriptorSectionPresent = (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_sec_property_descriptors = ") -ge 1
  $ivarDescriptorSectionPresent = (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_sec_ivar_descriptors = ") -ge 1
  $currentPropertyHelperCallCount =
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)call i32 @objc3_runtime_read_current_property_i32\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)call void @objc3_runtime_write_current_property_i32\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)call i32 @objc3_runtime_exchange_current_property_i32\(")
  $synthesizedAccessorDefinitionCount =
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^define i32 @objc3_method_.*_instance_.*\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^define i1 @objc3_method_.*_instance_.*\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^define void @objc3_method_.*_instance_.*\(")

  $propertyDescriptorCountsMatch = ($propertyDescriptorDefinitionCount -eq $propertyDescriptorCountExpected)
  $ivarDescriptorCountsMatch = ($ivarDescriptorDefinitionCount -eq $ivarDescriptorCountExpected)
  $synthesizedPropertySurfaceMatches = ($propertySynthesisSitesExpected -eq 0) -or (
    $propertyDescriptorCountExpected -gt 0 -and (
      (
        $currentPropertyHelperCallCount -gt 0 -and
        $synthesizedAccessorDefinitionCount -ge $propertySynthesisSitesExpected
      ) -or (
        $currentPropertyHelperCallCount -eq 0
      )
    )
  )
  $truthful = $dispatchDeclarationCount -ge 1 -and
    $propertyDescriptorSectionPresent -and
    $ivarDescriptorSectionPresent -and
    $propertyDescriptorCountsMatch -and
    $ivarDescriptorCountsMatch -and
    $synthesizedPropertySurfaceMatches

  $failures = New-Object System.Collections.Generic.List[string]
  if ($dispatchDeclarationCount -lt 1) {
    $failures.Add("missing LLVM declaration for runtime dispatch symbol '$runtimeDispatchSymbol'")
  }
  if (-not $propertyDescriptorSectionPresent) {
    $failures.Add("missing property descriptor aggregate section in emitted LLVM IR")
  }
  if (-not $ivarDescriptorSectionPresent) {
    $failures.Add("missing ivar descriptor aggregate section in emitted LLVM IR")
  }
  if (-not $propertyDescriptorCountsMatch) {
    $failures.Add("property descriptor count mismatch: registration manifest=$propertyDescriptorCountExpected emitted LLVM IR=$propertyDescriptorDefinitionCount")
  }
  if (-not $ivarDescriptorCountsMatch) {
    $failures.Add("ivar descriptor count mismatch: registration manifest=$ivarDescriptorCountExpected emitted LLVM IR=$ivarDescriptorDefinitionCount")
  }
  if (-not $synthesizedPropertySurfaceMatches) {
    $failures.Add("synthesized property lowering replay claims do not match emitted runtime-backed accessor/helper surface")
  }

  return [ordered]@{
    contract_id = "objc3c.native.compile.output.truthfulness.v1"
    llvm_ir_artifact = ($EmitPrefix + ".ll")
    manifest_artifact = ($EmitPrefix + ".manifest.json")
    registration_manifest_artifact = ($EmitPrefix + ".runtime-registration-manifest.json")
    verification_model = "compile-wrapper-cross-checks-manifest-and-runtime-registration-claims-against-emitted-llvm-ir"
    runtime_dispatch_symbol = $runtimeDispatchSymbol
    runtime_dispatch_declaration_count = $dispatchDeclarationCount
    runtime_dispatch_call_count = $dispatchCallCount
    property_descriptor_count_expected = $propertyDescriptorCountExpected
    property_descriptor_definition_count = $propertyDescriptorDefinitionCount
    property_descriptor_section_present = $propertyDescriptorSectionPresent
    ivar_descriptor_count_expected = $ivarDescriptorCountExpected
    ivar_descriptor_definition_count = $ivarDescriptorDefinitionCount
    ivar_descriptor_section_present = $ivarDescriptorSectionPresent
    property_synthesis_sites_expected = $propertySynthesisSitesExpected
    synthesized_accessor_definition_count = $synthesizedAccessorDefinitionCount
    current_property_helper_call_count = $currentPropertyHelperCallCount
    property_descriptor_counts_match = $propertyDescriptorCountsMatch
    ivar_descriptor_counts_match = $ivarDescriptorCountsMatch
    synthesized_property_surface_matches = $synthesizedPropertySurfaceMatches
    truthful = $truthful
    failures = @($failures.ToArray())
  }
}

Export-ModuleMember -Function "Get-CompileOutputTruthfulness"
