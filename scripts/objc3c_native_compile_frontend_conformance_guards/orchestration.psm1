$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$conformanceOrchestrationRoot = Join-Path $PSScriptRoot "orchestration"
foreach ($conformanceOrchestrationModule in @("dependencies.psm1", "guard_runner.psm1")) {
  $conformanceOrchestrationModulePath = Join-Path $conformanceOrchestrationRoot $conformanceOrchestrationModule
  if (!(Test-Path -LiteralPath $conformanceOrchestrationModulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance orchestration helper missing at $conformanceOrchestrationModulePath"
    exit 2
  }
  $conformanceOrchestrationRootLiteral = (Split-Path -Parent $conformanceOrchestrationModulePath).Replace("'", "''")
  $conformanceOrchestrationScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$conformanceOrchestrationRootLiteral'`n" +
    (Get-Content -LiteralPath $conformanceOrchestrationModulePath -Raw)
  )
  . $conformanceOrchestrationScript
}

$scriptRoot = Split-Path $PSScriptRoot -Parent
$conformanceGuardDependencyModules = Get-FrontendConformanceGuardDependencyModules -ConformanceGuardRoot $PSScriptRoot

foreach ($modulePath in $conformanceGuardDependencyModules) {
  if (!(Test-Path -LiteralPath $modulePath -PathType Leaf)) {
    Write-Error "native compile frontend conformance guard dependency module missing at $modulePath"
    exit 2
  }
  if ($modulePath -eq (Join-Path $scriptRoot "objc3c_native_compile_toolchain.psm1")) {
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
