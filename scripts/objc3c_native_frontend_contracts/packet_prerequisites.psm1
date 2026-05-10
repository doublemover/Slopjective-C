function Assert-Objc3cNativeFrontendPacketPrerequisites {
  param(
    [Parameter(Mandatory = $true)][object[]]$PacketDefinitions,
    [Parameter(Mandatory = $true)][object[]]$SelectedPacketDefinitions,
    [Parameter(Mandatory = $true)][string]$NativeBinaryPath,
    [Parameter(Mandatory = $true)][string]$CapiBinaryPath
  )

  $selectedNames = @{}
  foreach ($definition in $SelectedPacketDefinitions) {
    $selectedNames[$definition.Name] = $true
  }

  foreach ($definition in $SelectedPacketDefinitions) {
    if ($definition.RequiresNativeBinaries) {
      foreach ($binaryPath in @($NativeBinaryPath, $CapiBinaryPath)) {
        if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
          throw ("contract artifact family '{0}' requires existing native binaries: {1}" -f $definition.Family, $binaryPath)
        }
      }
    }
    foreach ($dependencyName in $definition.Dependencies) {
      if ($selectedNames.ContainsKey($dependencyName)) {
        continue
      }
      $dependency = $PacketDefinitions | Where-Object { $_.Name -eq $dependencyName } | Select-Object -First 1
      if ($null -eq $dependency) {
        throw ("frontend contract dependency not declared: " + $dependencyName)
      }
      if (!(Test-Path -LiteralPath $dependency.OutputPath -PathType Leaf)) {
        throw ("packet '{0}' requires existing dependency output '{1}' at {2}" -f $definition.Name, $dependencyName, $dependency.OutputPath)
      }
    }
  }
}
