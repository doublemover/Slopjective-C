Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function Read-RunnableToolchainPackageJsonHashtable {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $payloadPath = Join-Path $PackageRoot ($RelativePath.Replace('/', '\'))
  return Get-Content -LiteralPath $payloadPath -Raw | ConvertFrom-Json -AsHashtable
}

function Get-RunnableToolchainPackageSurfacePayloads {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  $repoSupercleanSurfaceRelativePath = "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json"
  $repoSupercleanSurfacePayload = Read-RunnableToolchainPackageJsonHashtable `
    -PackageRoot $PackageRoot `
    -RelativePath $repoSupercleanSurfaceRelativePath
  Assert-RequiredPackageSurfaceKeys `
    -Payload $repoSupercleanSurfacePayload `
    -RelativePath $repoSupercleanSurfaceRelativePath `
    -RequiredKeys @(
      "bonus_experience_surfaces",
      "performance_benchmark_surface",
      "runtime_performance_surface",
      "compiler_throughput_surface",
      "conformance_corpus_surface",
      "stdlib_foundation_surface",
      "stdlib_program_surface"
    )

  $stdlibLoweringImportSurfaceRelativePath = "stdlib/lowering_import_surface.json"
  $stdlibAdvancedHelperPackageSurfaceRelativePath = "stdlib/advanced_helper_package_surface.json"
  $stdlibProgramSurfaceRelativePath = "stdlib/program_surface.json"

  return [ordered]@{
    RepoSupercleanSurfaceRelativePath = $repoSupercleanSurfaceRelativePath
    RepoSupercleanSurfacePayload = $repoSupercleanSurfacePayload
    StdlibLoweringImportSurfaceRelativePath = $stdlibLoweringImportSurfaceRelativePath
    StdlibLoweringImportSurfacePayload = Read-RunnableToolchainPackageJsonHashtable `
      -PackageRoot $PackageRoot `
      -RelativePath $stdlibLoweringImportSurfaceRelativePath
    StdlibAdvancedHelperPackageSurfaceRelativePath = $stdlibAdvancedHelperPackageSurfaceRelativePath
    StdlibAdvancedHelperPackageSurfacePayload = Read-RunnableToolchainPackageJsonHashtable `
      -PackageRoot $PackageRoot `
      -RelativePath $stdlibAdvancedHelperPackageSurfaceRelativePath
    StdlibProgramSurfaceRelativePath = $stdlibProgramSurfaceRelativePath
    StdlibProgramSurfacePayload = Read-RunnableToolchainPackageJsonHashtable `
      -PackageRoot $PackageRoot `
      -RelativePath $stdlibProgramSurfaceRelativePath
  }
}

Export-ModuleMember -Function @(
  "Get-RunnableToolchainPackageSurfacePayloads"
)
