$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceGuardModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_frontend_conformance_guards"
$conformanceGuardOrchestrationModule = Join-Path $conformanceGuardModuleRoot "orchestration.psm1"
if (!(Test-Path -LiteralPath $conformanceGuardOrchestrationModule -PathType Leaf)) {
  Write-Error "native compile frontend conformance guard orchestration module missing at $conformanceGuardOrchestrationModule"
  exit 2
}
$conformanceGuardOrchestrationRootLiteral = (Split-Path -Parent $conformanceGuardOrchestrationModule).Replace("'", "''")
$conformanceGuardOrchestrationScript = [scriptblock]::Create(
  "`$PSScriptRoot = '$conformanceGuardOrchestrationRootLiteral'`n" +
  (Get-Content -LiteralPath $conformanceGuardOrchestrationModule -Raw)
)
. $conformanceGuardOrchestrationScript

function Assert-FrontendConformanceMatrix {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  Invoke-FrontendConformanceMatrixGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -ParsedArgs $ParsedArgs `
    -EffectiveCompileArgs $EffectiveCompileArgs
}

function Assert-FrontendConformanceCorpus {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [string]$InvocationProfileKey
  )

  Invoke-FrontendConformanceCorpusGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -InvocationProfileKey $InvocationProfileKey
}

function Assert-FrontendIntegrationCloseout {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendIntegrationCloseoutGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult
}

Export-ModuleMember -Function @("Assert-FrontendConformanceMatrix", "Assert-FrontendConformanceCorpus", "Assert-FrontendIntegrationCloseout")
