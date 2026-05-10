$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardRoot = $PSScriptRoot
$scriptRoot = Split-Path $conformanceGuardRoot -Parent
$compileToolchainModule = Join-Path $scriptRoot "objc3c_native_compile_toolchain.psm1"
$conformanceGuardConfigModule = Join-Path $conformanceGuardRoot "config.psm1"
$conformanceGuardEvaluationModule = Join-Path $conformanceGuardRoot "evaluation.psm1"
$conformanceGuardNormalizationModule = Join-Path $conformanceGuardRoot "normalization.psm1"

foreach ($modulePath in @($compileToolchainModule, $conformanceGuardConfigModule, $conformanceGuardEvaluationModule, $conformanceGuardNormalizationModule)) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard dependency module missing at $modulePath"
    exit 2
  }
  Import-Module $modulePath -Force -DisableNameChecking
}

function Invoke-FrontendConformanceMatrixGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  $matrixPath = Resolve-FrontendConformanceMatrixPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $config = Get-FrontendConformanceMatrixGuardConfig
  $payload = Read-FrontendConformanceJsonArtifact -Path $matrixPath -ArtifactName $config.artifact_name
  Invoke-FrontendConformanceMatrixEvaluation `
    -ArtifactPath $matrixPath `
    -Payload $payload `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $EffectiveCompileArgs
}

function Invoke-FrontendConformanceCorpusGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [string]$InvocationProfileKey
  )

  $corpusPath = Resolve-FrontendConformanceCorpusPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $config = Get-FrontendConformanceCorpusGuardConfig
  $payload = Read-FrontendConformanceJsonArtifact -Path $corpusPath -ArtifactName $config.artifact_name
  Invoke-FrontendConformanceCorpusEvaluation `
    -ArtifactPath $corpusPath `
    -Payload $payload `
    -InvocationProfileKey $InvocationProfileKey
}

function Invoke-FrontendIntegrationCloseoutGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $closeoutPath = Resolve-FrontendIntegrationCloseoutPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  $config = Get-FrontendIntegrationCloseoutGuardConfig
  $payload = Read-FrontendConformanceJsonArtifact -Path $closeoutPath -ArtifactName $config.artifact_name
  Invoke-FrontendIntegrationCloseoutEvaluation `
    -ArtifactPath $closeoutPath `
    -Payload $payload
}

Export-ModuleMember -Function @(
  "Invoke-FrontendConformanceCorpusGuard",
  "Invoke-FrontendConformanceMatrixGuard",
  "Invoke-FrontendIntegrationCloseoutGuard"
)
