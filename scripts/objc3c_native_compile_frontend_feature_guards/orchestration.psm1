$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$featureGuardRoot = $PSScriptRoot
$scriptRoot = Split-Path $featureGuardRoot -Parent
$compileToolchainModule = Join-Path $scriptRoot "objc3c_native_compile_toolchain.psm1"
$featureGuardConfigModule = Join-Path $featureGuardRoot "config.psm1"
$featureGuardEvaluationModule = Join-Path $featureGuardRoot "evaluation.psm1"
$featureGuardNormalizationModule = Join-Path $featureGuardRoot "normalization.psm1"

foreach ($modulePath in @($compileToolchainModule, $featureGuardConfigModule, $featureGuardEvaluationModule, $featureGuardNormalizationModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend feature guard dependency module missing at $modulePath"
    exit 2
  }
  if ($modulePath -eq $compileToolchainModule) {
    Import-Module $modulePath -Force -DisableNameChecking
  } else {
    $moduleRootLiteral = (Split-Path -Parent $modulePath).Replace("'", "''")
    $moduleScript = [scriptblock]::Create(
      "`$PSScriptRoot = '$moduleRootLiteral'`n" +
      (Get-Content -LiteralPath $modulePath -Raw)
    )
    . $moduleScript
  }
}

function Invoke-FrontendCoreFeatureExpansionGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs
  )

  $featurePath = Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $config = Get-FrontendCoreFeatureGuardConfig
  $payload = Read-FrontendFeatureGuardJsonArtifact -Path $featurePath -ArtifactName $config.artifact_name
  Invoke-FrontendCoreFeatureExpansionEvaluation `
    -ArtifactPath $featurePath `
    -Payload $payload `
    -ParsedArgs $ParsedArgs
}

function Invoke-FrontendEdgeCompatibilityGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $compatPath = Resolve-FrontendEdgeCompatibilityPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $config = Get-FrontendEdgeCompatibilityGuardConfig
  $payload = Read-FrontendFeatureGuardJsonArtifact -Path $compatPath -ArtifactName $config.artifact_name
  Invoke-FrontendEdgeCompatibilityEvaluation `
    -ArtifactPath $compatPath `
    -Payload $payload `
    -ParsedArgs $ParsedArgs `
    -CoreFeatureGuard $CoreFeatureGuard
}

Export-ModuleMember -Function @(
  "Invoke-FrontendCoreFeatureExpansionGuard",
  "Invoke-FrontendEdgeCompatibilityGuard"
)
