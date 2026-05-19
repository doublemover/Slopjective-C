function Get-Objc3cNativeFrontendContractModuleNames {
  return @(
    "artifact_paths.psm1",
    "packet_definitions.psm1",
    "packet_selection.psm1",
    "packet_prerequisites.psm1",
    "module_catalog.psm1",
    "shared_sources.psm1"
  )
}

function Get-Objc3cNativeFrontendContractExportedFunctionNames {
  return @(
    "Get-Objc3cNativeFrontendArtifactPaths",
    "Get-Objc3cNativeFrontendPacketDefinitions",
    "Get-Objc3cNativeSelectedFrontendPacketDefinitions",
    "Assert-Objc3cNativeFrontendPacketPrerequisites",
    "Get-Objc3cNativeFrontendModules",
    "Get-Objc3cNativeFrontendSharedSources"
  )
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeFrontendContractModuleNames",
  "Get-Objc3cNativeFrontendContractExportedFunctionNames"
)
