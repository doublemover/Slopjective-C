$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-FrontendHardeningArtifactGuard {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult,
    [Parameter(Mandatory = $true)]$Definition
  )

  $rules = & $Definition.RulesFactory
  $artifactPath = & $Definition.PathResolver $RepoRoot $BuildResult
  $payload = Read-FrontendHardeningGuardJsonArtifact `
    -Path $artifactPath `
    -ArtifactName $rules.artifact_name

  & $Definition.Assertion $payload $rules $artifactPath

  return New-FrontendHardeningGuardReport `
    -PropertyName $rules.report_key `
    -ArtifactPath $artifactPath
}
