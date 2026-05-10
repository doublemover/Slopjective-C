function Get-SemaPassManagerPositiveRuntimeArtifacts {
  return @("module.manifest.json", "module.diagnostics.txt", "module.diagnostics.json", "module.ll", "module.obj", "module.object-backend.txt")
}

function Get-SemaPassManagerPositivePreObjectArtifacts {
  return @("module.manifest.json", "module.ll", "module.diagnostics.txt", "module.diagnostics.json")
}

function Get-SemaPassManagerPositiveForbiddenObjectArtifacts {
  return @("module.obj", "module.object-backend.txt")
}

function Get-SemaPassManagerPositiveLlvmDirectUnavailableMarkers {
  return @(
    "llvm-direct object emission failed: llc executable not found:",
    "llvm-direct object emission backend unavailable in this build",
    "llvm-direct object emission failed: llc exited with status "
  )
}

function Get-SemaPassManagerPositiveBackendArgs {
  param(
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$OutDir,
    [Parameter(Mandatory = $true)][string]$Backend,
    [string[]]$AdditionalArgs = @()
  )

  return @(
    $FixturePath,
    "--out-dir",
    $OutDir,
    "--emit-prefix",
    "module",
    "--objc3-ir-object-backend",
    $Backend
  ) + @($AdditionalArgs)
}
