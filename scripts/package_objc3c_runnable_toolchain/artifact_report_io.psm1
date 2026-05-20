Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function Read-RunnableToolchainPackageJsonHashtable {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $payloadPath = Join-Path $PackageRoot ($RelativePath.Replace('/', '\'))
  if (!(Test-Path -LiteralPath $payloadPath -PathType Leaf)) {
    if ($RelativePath -eq "tmp/build-objc3c-native/repo_superclean_source_of_truth.json") {
      throw "runnable toolchain package FAIL: missing generated repo superclean surface $RelativePath; package staging must run scripts/build_objc3c_native.ps1 before manifest generation"
    }
    throw "runnable toolchain package FAIL: missing package JSON payload $RelativePath"
  }
  return Get-Content -LiteralPath $payloadPath -Raw | ConvertFrom-Json -AsHashtable
}

function Get-RunnableToolchainPackageSurfacePayloads {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  $repoSupercleanSurfaceRelativePath = "tmp/build-objc3c-native/repo_superclean_source_of_truth.json"
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
  $showcaseDemoPackagesRelativePath = "showcase/demo_packages.json"

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
    ShowcaseDemoPackagesRelativePath = $showcaseDemoPackagesRelativePath
    ShowcaseDemoPackagesPayload = Read-RunnableToolchainPackageJsonHashtable `
      -PackageRoot $PackageRoot `
      -RelativePath $showcaseDemoPackagesRelativePath
  }
}

Export-ModuleMember -Function @(
  "Get-RunnableToolchainPackageSurfacePayloads"
)
