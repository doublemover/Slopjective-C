$ErrorActionPreference = "Stop"

function Get-Objc3cNativeFrontendModuleEvidencePayload {
  param(
    [Parameter(Mandatory = $true)]
    [object[]]$Modules
  )

  $modulePayload = New-Object System.Collections.Generic.List[object]
  foreach ($module in $Modules) {
    $name = [string]$module.name
    $sources = @($module.sources)
    Assert-Objc3cNativeFrontendSourceGraphModuleMetadata -Name $name -Sources $sources

    $modulePayload.Add([ordered]@{
      name = $name
      source_count = $sources.Count
      sources = $sources
    })
  }

  return $modulePayload.ToArray()
}

function Get-Objc3cNativeFrontendHashedArtifactEvidencePayload {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$Name,
    [Parameter(Mandatory = $true)]
    [string]$Path
  )

  return [ordered]@{
    name = $Name
    path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $Path
    sha256 = Get-Objc3cNativeFileSha256Hex -Path $Path
  }
}

function Get-Objc3cNativeFrontendBinaryReferencePayload {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [string]$Name,
    [Parameter(Mandatory = $true)]
    [string]$Path
  )

  return [ordered]@{
    name = $Name
    path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $Path
  }
}

function Get-Objc3cNativeFrontendModuleNameEvidencePayload {
  param(
    [Parameter(Mandatory = $true)]
    [object[]]$Modules
  )

  $moduleNames = @($Modules | ForEach-Object { [string]$_.name })
  foreach ($moduleName in $moduleNames) {
    Assert-Objc3cNativeFrontendCoreFeatureModuleName -ModuleName $moduleName
  }

  return $moduleNames
}

function New-Objc3cNativeFrontendModuleScaffoldPayload {
  param(
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string[]]$BinaryTargets
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  $modulePayload = @(Get-Objc3cNativeFrontendModuleEvidencePayload -Modules $Modules)

  return [ordered]@{
    contract_id = $contracts.ModuleScaffold
    schema_version = 1
    module_count = $modulePayload.Count
    shared_source_count = $SharedSources.Count
    modules = $modulePayload
    shared_sources = $SharedSources
    binary_targets = $BinaryTargets
  }
}

function New-Objc3cNativeFrontendInvocationLockPayload {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    $ScaffoldPayload,
    [Parameter(Mandatory = $true)]
    [string]$FrontendScaffoldPath,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  $binaryNames = Get-Objc3cNativeFrontendBinaryNames

  return [ordered]@{
    contract_id = $contracts.InvocationLock
    schema_version = 1
    scaffold_contract_id = [string]$ScaffoldPayload.contract_id
    scaffold = [ordered]@{
      path = Get-Objc3cNativeRepoRelativePath -RootPath $RepoRoot -TargetPath $FrontendScaffoldPath
      sha256 = Get-Objc3cNativeFileSha256Hex -Path $FrontendScaffoldPath
    }
    binaries = @(
      (Get-Objc3cNativeFrontendHashedArtifactEvidencePayload `
        -RepoRoot $RepoRoot `
        -Name $binaryNames.Native `
        -Path $NativeBinaryPath),
      (Get-Objc3cNativeFrontendHashedArtifactEvidencePayload `
        -RepoRoot $RepoRoot `
        -Name $binaryNames.CapiRunner `
        -Path $CapiBinaryPath)
    )
  }
}

function New-Objc3cNativeFrontendCoreFeatureExpansionPayload {
  param(
    [Parameter(Mandatory = $true)]
    [string]$RepoRoot,
    [Parameter(Mandatory = $true)]
    [object[]]$Modules,
    [Parameter(Mandatory = $true)]
    [string[]]$SharedSources,
    [Parameter(Mandatory = $true)]
    [string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)]
    [string]$CapiBinaryPath
  )

  $contracts = Get-Objc3cNativeFrontendArtifactContractIds
  $binaryNames = Get-Objc3cNativeFrontendBinaryNames
  $pathConstants = Get-Objc3cNativeFrontendCoreFeaturePathConstants
  $backendRouting = Get-Objc3cNativeFrontendBackendRoutingConstants
  $moduleNames = @(Get-Objc3cNativeFrontendModuleNameEvidencePayload -Modules $Modules)

  return [ordered]@{
    contract_id = $contracts.CoreFeatureExpansion
    schema_version = 1
    depends_on_contract_ids = @(
      $contracts.ModuleScaffold
      $contracts.InvocationLock
    )
    module_names = $moduleNames
    shared_source_count = $SharedSources.Count
    binaries = @(
      (Get-Objc3cNativeFrontendBinaryReferencePayload `
        -RepoRoot $RepoRoot `
        -Name $binaryNames.Native `
        -Path $NativeBinaryPath),
      (Get-Objc3cNativeFrontendBinaryReferencePayload `
        -RepoRoot $RepoRoot `
        -Name $binaryNames.CapiRunner `
        -Path $CapiBinaryPath)
    )
    invocation = [ordered]@{
      default_out_dir = $pathConstants.DefaultOutDir
      cache_root = $pathConstants.CacheRoot
      supports_cache = $true
    }
    backend_routing = [ordered]@{
      allowed_ir_object_backends = $backendRouting.AllowedIrObjectBackends
      supports_capability_routing = $backendRouting.SupportsCapabilityRouting
      capability_summary_flag = $backendRouting.CapabilitySummaryFlag
      route_flag = $backendRouting.RouteFlag
    }
  }
}
