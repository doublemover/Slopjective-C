$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
foreach ($modulePath in @($conformanceGuardNormalizationModule, $conformanceGuardReportingModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard evaluation common dependency module missing at $modulePath"
    exit 2
  }
  $moduleRootLiteral = (Split-Path -Parent $modulePath).Replace("'", "''")
  $moduleScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$moduleRootLiteral'`n" +
    (Get-Content -LiteralPath $modulePath -Raw)
  )
  . $moduleScript
}

function Assert-FrontendConformanceDependencyContracts {
  param(
    [object[]]$PresentContracts,
    [string[]]$RequiredContracts,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  $dependencySet = New-FrontendConformanceStringSet -Values @($PresentContracts)
  foreach ($requiredContractId in $RequiredContracts) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Stop-FrontendConformanceGuard "$ArtifactName missing dependency contract '$requiredContractId' in $ArtifactPath"
    }
  }
}

function Assert-FrontendConformanceRequiredDiagnostics {
  param(
    [hashtable]$PresentDiagnostics,
    [string[]]$RequiredDiagnostics,
    [string]$ArtifactName,
    [string]$ArtifactPath
  )

  foreach ($requiredDiagnostic in $RequiredDiagnostics) {
    if (-not $PresentDiagnostics.ContainsKey($requiredDiagnostic)) {
      Stop-FrontendConformanceGuard "$ArtifactName missing rejection diagnostic '$requiredDiagnostic' in $ArtifactPath"
    }
  }
}

Export-ModuleMember -Function @(
  "Assert-FrontendConformanceDependencyContracts",
  "Assert-FrontendConformanceRequiredDiagnostics"
)
