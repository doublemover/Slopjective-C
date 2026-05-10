$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-Objc3cDiagnosticsUnexpectedFailClosedArtifactsCore {
  param(
    [string]$OutDir,
    [string[]]$ArtifactNames = @("module.manifest.json", "module.ll", "module.obj")
  )

  $unexpected = New-Object 'System.Collections.Generic.List[string]'
  foreach ($artifact in $ArtifactNames) {
    if (Test-Path -LiteralPath (Join-Path $OutDir $artifact) -PathType Leaf) {
      $null = $unexpected.Add($artifact)
    }
  }

  return @($unexpected.ToArray())
}

Export-ModuleMember -Function @(
  "Get-Objc3cDiagnosticsUnexpectedFailClosedArtifactsCore"
)
