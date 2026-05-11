$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-FrontendConformanceArtifactGuard {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult,
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)][scriptblock]$PathResolver,
    [Parameter(Mandatory = $true)][scriptblock]$Evaluation
  )

  $artifactPath = & $PathResolver $RepoRoot $BuildResult
  $payload = Read-FrontendConformanceJsonArtifact `
    -Path $artifactPath `
    -ArtifactName $Config.artifact_name
  & $Evaluation $artifactPath $payload
}
