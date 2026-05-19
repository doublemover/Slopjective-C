$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($conformanceGuardConfigModule, $conformanceGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard helper module missing at $modulePath"
    exit 2
  }
  $moduleRootLiteral = (Split-Path -Parent $modulePath).Replace("'", "''")
  $moduleScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$moduleRootLiteral'`n" +
    (Get-Content -LiteralPath $modulePath -Raw)
  )
  . $moduleScript
}

$normalizationRoot = Join-Path $PSScriptRoot "normalization"
$normalizationModules = @(
  "artifact_io.psm1",
  "backend_keys.psm1",
  "sets.psm1",
  "profile_keys.psm1"
)

foreach ($normalizationModule in $normalizationModules) {
  $normalizationModulePath = Join-Path $normalizationRoot $normalizationModule
  if (!(Test-Path -LiteralPath $normalizationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance normalization helper missing at $normalizationModulePath"
    exit 2
  }
  $normalizationRootLiteral = (Split-Path -Parent $normalizationModulePath).Replace("'", "''")
  $normalizationScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$normalizationRootLiteral'`n" +
    (Get-Content -LiteralPath $normalizationModulePath -Raw)
  )
  . $normalizationScript
}

Export-ModuleMember -Function @(
  "Get-FrontendConformanceInvocationProfileKey",
  "New-FrontendConformanceExpectedProfileSet",
  "New-FrontendConformanceStringSet",
  "Normalize-FrontendConformanceBackendKey",
  "Read-FrontendConformanceJsonArtifact"
)
