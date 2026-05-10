$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$objc3cNativeSupercleanSurfaceCatalogModuleRoot = Join-Path $PSScriptRoot "objc3c_native_superclean_surface_catalog"
$objc3cNativeSupercleanSurfaceCatalogModules = @(
  "normalization.psm1",
  "report_rendering.psm1",
  "base_surface.psm1",
  "bonus_surfaces.psm1",
  "performance_surfaces.psm1",
  "release_surfaces.psm1",
  "program_surfaces.psm1"
)

foreach ($moduleName in $objc3cNativeSupercleanSurfaceCatalogModules) {
  Import-Module (Join-Path $objc3cNativeSupercleanSurfaceCatalogModuleRoot $moduleName) -Force -DisableNameChecking
}

Export-ModuleMember -Function @(
  "New-Objc3cNativeRepoSupercleanBaseSurface",
  "New-Objc3cNativeRepoSupercleanBonusSurfaces",
  "New-Objc3cNativeRepoSupercleanPerformanceSurfaces",
  "New-Objc3cNativeRepoSupercleanReleaseSurfaces",
  "New-Objc3cNativeRepoSupercleanProgramSurfaces",
  "Get-Objc3cNativeRepoSupercleanExplicitNonGoals"
)
