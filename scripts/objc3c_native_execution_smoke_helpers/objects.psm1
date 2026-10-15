Set-StrictMode -Version Latest

function Get-NativeObjectArtifactNameForHost {
  param([string]$EmitPrefix = "module")

  if ([System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
      [System.Runtime.InteropServices.OSPlatform]::Windows
    )) {
    return $EmitPrefix + ".obj"
  }
  return $EmitPrefix + ".o"
}

function Assert-NativeObjectArtifactLeafName {
  param(
    [Parameter(Mandatory = $true)][string]$ObjectArtifact,
    [Parameter(Mandatory = $true)][string]$ManifestPath
  )

  if ([string]::IsNullOrWhiteSpace($ObjectArtifact)) {
    throw "execution smoke FAIL: runtime registration manifest object_artifact is empty at $ManifestPath"
  }
  if ([System.IO.Path]::IsPathRooted($ObjectArtifact) -or $ObjectArtifact -match '[\\/]' -or $ObjectArtifact -in @(".", "..")) {
    throw "execution smoke FAIL: runtime registration manifest object_artifact must be a leaf file name at $ManifestPath"
  }
}

function Resolve-NativeObjectArtifactName {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [string]$EmitPrefix = "module"
  )

  $manifestPath = Join-Path $CompileDir ($EmitPrefix + ".runtime-registration-manifest.json")
  if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    return Get-NativeObjectArtifactNameForHost -EmitPrefix $EmitPrefix
  }

  try {
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
  } catch {
    throw "execution smoke FAIL: invalid runtime registration manifest json at $manifestPath"
  }

  $objectArtifactProperty = $manifest.PSObject.Properties["object_artifact"]
  if ($null -eq $objectArtifactProperty) {
    throw "execution smoke FAIL: runtime registration manifest missing object_artifact at $manifestPath"
  }

  $objectArtifact = "$($objectArtifactProperty.Value)".Trim()
  Assert-NativeObjectArtifactLeafName -ObjectArtifact $objectArtifact -ManifestPath $manifestPath
  return $objectArtifact
}

function Resolve-NativeObjectPath {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [string]$EmitPrefix = "module"
  )

  $objectArtifact = Resolve-NativeObjectArtifactName -CompileDir $CompileDir -EmitPrefix $EmitPrefix
  $objectPath = Join-Path $CompileDir $objectArtifact
  if (Test-Path -LiteralPath $objectPath -PathType Leaf) {
    return $objectPath
  }

  throw "execution smoke FAIL: missing native object artifact $objectArtifact for $fixtureRel in $CompileDir"
}

Export-ModuleMember -Function @(
  "Resolve-NativeObjectPath"
)
