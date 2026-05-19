$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-FrontendHardeningEdgeRobustnessDefinition {
  return [pscustomobject]@{
    RulesFactory = { Get-FrontendEdgeRobustnessGuardRules }
    PathResolver = { param($RepoRoot, $BuildResult) Resolve-FrontendEdgeRobustnessPath -RepoRoot $RepoRoot -BuildResult $BuildResult }
    Assertion = { param($Payload, $Rules, $ArtifactPath) Assert-FrontendEdgeRobustnessPayload -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath }
  }
}

function Get-FrontendHardeningDiagnosticsDefinition {
  return [pscustomobject]@{
    RulesFactory = { Get-FrontendDiagnosticsHardeningGuardRules }
    PathResolver = { param($RepoRoot, $BuildResult) Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult }
    Assertion = { param($Payload, $Rules, $ArtifactPath) Assert-FrontendDiagnosticsHardeningPayload -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath }
  }
}

function Get-FrontendHardeningRecoveryDeterminismDefinition {
  return [pscustomobject]@{
    RulesFactory = { Get-FrontendRecoveryDeterminismHardeningGuardRules }
    PathResolver = { param($RepoRoot, $BuildResult) Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult }
    Assertion = { param($Payload, $Rules, $ArtifactPath) Assert-FrontendRecoveryDeterminismHardeningPayload -Payload $Payload -Rules $Rules -ArtifactPath $ArtifactPath }
  }
}
