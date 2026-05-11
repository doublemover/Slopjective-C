$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceOrchestrationRoot = Join-Path $PSScriptRoot "orchestration"
. (Join-Path $conformanceOrchestrationRoot "dependencies.psm1")
. (Join-Path $conformanceOrchestrationRoot "guard_runner.psm1")

Import-FrontendConformanceGuardDependencies -ConformanceGuardRoot $PSScriptRoot

function Invoke-FrontendConformanceMatrixGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  Invoke-FrontendConformanceArtifactGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -Config (Get-FrontendConformanceMatrixGuardConfig) `
    -PathResolver { param($RepoRoot, $BuildResult) Resolve-FrontendConformanceMatrixPath -RepoRoot $RepoRoot -BuildResult $BuildResult } `
    -Evaluation {
      param($ArtifactPath, $Payload)
      Invoke-FrontendConformanceMatrixEvaluation `
        -ArtifactPath $ArtifactPath `
        -Payload $Payload `
        -ParsedArgs $ParsedArgs `
        -EffectiveCompileArgs $EffectiveCompileArgs
    }
}

function Invoke-FrontendConformanceCorpusGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [string]$InvocationProfileKey
  )

  Invoke-FrontendConformanceArtifactGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -Config (Get-FrontendConformanceCorpusGuardConfig) `
    -PathResolver { param($RepoRoot, $BuildResult) Resolve-FrontendConformanceCorpusPath -RepoRoot $RepoRoot -BuildResult $BuildResult } `
    -Evaluation {
      param($ArtifactPath, $Payload)
      Invoke-FrontendConformanceCorpusEvaluation `
        -ArtifactPath $ArtifactPath `
        -Payload $Payload `
        -InvocationProfileKey $InvocationProfileKey
    }
}

function Invoke-FrontendIntegrationCloseoutGuard {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  Invoke-FrontendConformanceArtifactGuard `
    -RepoRoot $RepoRoot `
    -BuildResult $BuildResult `
    -Config (Get-FrontendIntegrationCloseoutGuardConfig) `
    -PathResolver { param($RepoRoot, $BuildResult) Resolve-FrontendIntegrationCloseoutPath -RepoRoot $RepoRoot -BuildResult $BuildResult } `
    -Evaluation {
      param($ArtifactPath, $Payload)
      Invoke-FrontendIntegrationCloseoutEvaluation `
        -ArtifactPath $ArtifactPath `
        -Payload $Payload
    }
}

Export-ModuleMember -Function @(
  "Invoke-FrontendConformanceCorpusGuard",
  "Invoke-FrontendConformanceMatrixGuard",
  "Invoke-FrontendIntegrationCloseoutGuard"
)
