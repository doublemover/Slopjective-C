Set-StrictMode -Version Latest

function Add-Objc3cNativePerfObjectBackendArgument {
  param(
    [string[]]$Arguments,
    [string]$Extension,
    [bool]$ForceClangObjectBackend
  )

  if ($ForceClangObjectBackend -or $Extension -eq ".objc3") {
    return @($Arguments + @("--objc3-ir-object-backend", "clang"))
  }
  return @($Arguments)
}

function New-Objc3cNativePerfDirectCompileArguments {
  param(
    [object]$Fixture,
    [string]$CaseDir
  )

  $arguments = @($Fixture.FullName, "--out-dir", $CaseDir, "--emit-prefix", "module")
  return (Add-Objc3cNativePerfObjectBackendArgument `
      -Arguments $arguments `
      -Extension $Fixture.Extension `
      -ForceClangObjectBackend $false)
}

function New-Objc3cNativePerfWrapperCompileArguments {
  param(
    [string]$SourcePath,
    [string]$OutputDirectory,
    [string]$Extension,
    [switch]$UseCache,
    [switch]$ForceClangObjectBackend
  )

  $arguments = @($SourcePath)
  if ($UseCache.IsPresent) {
    $arguments += "--use-cache"
  }
  $arguments += @("--out-dir", $OutputDirectory)

  return (Add-Objc3cNativePerfObjectBackendArgument `
      -Arguments $arguments `
      -Extension $Extension `
      -ForceClangObjectBackend $ForceClangObjectBackend.IsPresent)
}

function Get-Objc3cNativePerfCacheArtifactNames {
  param([string]$Extension)

  $artifactNames = @(
    "module.obj",
    "module.manifest.json",
    "module.diagnostics.txt"
  )
  if ($Extension -eq ".objc3") {
    $artifactNames += "module.ll"
  }
  return @($artifactNames)
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativePerfCacheArtifactNames",
  "New-Objc3cNativePerfDirectCompileArguments",
  "New-Objc3cNativePerfWrapperCompileArguments"
)
