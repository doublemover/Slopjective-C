$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "hash_io.psm1") -Force -DisableNameChecking

function Get-CompileOutputArtifactEntries {
  param(
    [string]$CompileDir,
    [string]$EmitPrefix,
    [string]$ProvenanceFileName
  )

  $artifactFiles = @(
    Get-ChildItem -LiteralPath $CompileDir -File |
      Where-Object {
        $_.Name -ne $ProvenanceFileName -and
        $_.Name -ne ($EmitPrefix + ".runtime-registration-manifest.json") -and
        (
          $_.Name.Equals($EmitPrefix, [System.StringComparison]::OrdinalIgnoreCase) -or
          $_.Name.StartsWith($EmitPrefix + ".", [System.StringComparison]::OrdinalIgnoreCase) -or
          $_.Name.StartsWith($EmitPrefix + "-", [System.StringComparison]::OrdinalIgnoreCase)
        )
      } |
      Sort-Object Name
  )

  $artifactEntries = New-Object System.Collections.Generic.List[object]
  foreach ($artifact in $artifactFiles) {
    $artifactEntries.Add([ordered]@{
      path = $artifact.Name
      byte_count = [long]$artifact.Length
      sha256 = Get-FileSha256Hex -Path $artifact.FullName
    })
  }

  return @($artifactEntries.ToArray())
}

function Get-CompileOutputArtifactSetDigest {
  param([object[]]$ArtifactEntries)

  $digestLines = New-Object System.Collections.Generic.List[string]
  foreach ($entry in @($ArtifactEntries)) {
    $digestLines.Add(("{0}|{1}|{2}" -f [string]$entry.path, [string]$entry.byte_count, [string]$entry.sha256))
  }
  return Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($digestLines -join "`n")))
}

Export-ModuleMember -Function @(
  "Get-CompileOutputArtifactEntries",
  "Get-CompileOutputArtifactSetDigest"
)
