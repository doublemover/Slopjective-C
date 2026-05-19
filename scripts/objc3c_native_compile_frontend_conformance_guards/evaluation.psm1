$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardConfigModule = Join-Path $PSScriptRoot "config.psm1"
$conformanceGuardNormalizationModule = Join-Path $PSScriptRoot "normalization.psm1"
$conformanceGuardReportingModule = Join-Path $PSScriptRoot "reporting.psm1"
$conformanceGuardCommonEvaluationModule = Join-Path $PSScriptRoot "evaluation_common.psm1"
$conformanceGuardMatrixEvaluationModule = Join-Path $PSScriptRoot "evaluation_matrix.psm1"
$conformanceGuardCorpusEvaluationModule = Join-Path $PSScriptRoot "evaluation_corpus.psm1"
$conformanceGuardCloseoutEvaluationModule = Join-Path $PSScriptRoot "evaluation_closeout.psm1"
foreach ($modulePath in @(
    $conformanceGuardConfigModule,
    $conformanceGuardNormalizationModule,
    $conformanceGuardReportingModule,
    $conformanceGuardCommonEvaluationModule,
    $conformanceGuardMatrixEvaluationModule,
    $conformanceGuardCorpusEvaluationModule,
    $conformanceGuardCloseoutEvaluationModule
  )) {
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

function Invoke-FrontendConformanceMatrixEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  Invoke-FrontendConformanceMatrixEvaluationImpl `
    -ArtifactPath $ArtifactPath `
    -Payload $Payload `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $EffectiveCompileArgs
}

function Invoke-FrontendConformanceCorpusEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload,
    [string]$InvocationProfileKey
  )

  Invoke-FrontendConformanceCorpusEvaluationImpl `
    -ArtifactPath $ArtifactPath `
    -Payload $Payload `
    -InvocationProfileKey $InvocationProfileKey
}

function Invoke-FrontendIntegrationCloseoutEvaluation {
  param(
    [string]$ArtifactPath,
    [object]$Payload
  )

  Invoke-FrontendIntegrationCloseoutEvaluationImpl `
    -ArtifactPath $ArtifactPath `
    -Payload $Payload
}

Export-ModuleMember -Function @(
  "Invoke-FrontendConformanceCorpusEvaluation",
  "Invoke-FrontendConformanceMatrixEvaluation",
  "Invoke-FrontendIntegrationCloseoutEvaluation"
)
