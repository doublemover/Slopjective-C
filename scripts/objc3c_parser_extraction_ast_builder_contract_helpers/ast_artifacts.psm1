function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Get-FileSha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Get-ParserAstBuilderPositiveArtifacts {
  return @("module.manifest.json", "module.diagnostics.txt", "module.ll", "module.obj", "module.object-backend.txt")
}

function New-ParserAstBuilderDigestRecord {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Path,
    [Parameter(Mandatory = $true)][string]$Run2Path
  )

  $hashRun1 = Get-FileSha256Hex -Path $Run1Path
  $hashRun2 = Get-FileSha256Hex -Path $Run2Path
  return [ordered]@{
    run1_sha256 = $hashRun1
    run2_sha256 = $hashRun2
    deterministic = ($hashRun1 -eq $hashRun2)
  }
}

function Assert-ParserAstBuilderReplayedArtifactExists {
  param(
    [Parameter(Mandatory = $true)][string]$ArtifactName,
    [Parameter(Mandatory = $true)][string]$Run1Path,
    [Parameter(Mandatory = $true)][string]$Run2Path
  )

  $existsRun1 = Test-Path -LiteralPath $Run1Path -PathType Leaf
  $existsRun2 = Test-Path -LiteralPath $Run2Path -PathType Leaf
  Assert-Contract `
    -Condition ($existsRun1 -and $existsRun2) `
    -Id ("runtime.positive.artifact.exists.{0}" -f $ArtifactName) `
    -FailureMessage ("positive parser scaffold fixture missing artifact across replay: {0}" -f $ArtifactName) `
    -PassMessage ("positive parser scaffold artifact present across replay: {0}" -f $ArtifactName)
}

function Assert-ParserAstBuilderObjectArtifactNonEmpty {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Path,
    [Parameter(Mandatory = $true)][string]$Run2Path
  )

  $objRun1Bytes = (Get-Item -LiteralPath $Run1Path).Length
  $objRun2Bytes = (Get-Item -LiteralPath $Run2Path).Length
  Assert-Contract `
    -Condition ($objRun1Bytes -gt 0 -and $objRun2Bytes -gt 0) `
    -Id "runtime.positive.artifact.nonempty.module.obj" `
    -FailureMessage "positive parser scaffold module.obj is empty in one or more runs" `
    -PassMessage "positive parser scaffold module.obj is non-empty across replay" `
    -Evidence @{ run1_bytes = $objRun1Bytes; run2_bytes = $objRun2Bytes }
}

function Assert-ParserAstBuilderReplayDigest {
  param(
    [Parameter(Mandatory = $true)][string]$ArtifactName,
    [Parameter(Mandatory = $true)]$DigestRecord
  )

  if ($ArtifactName -eq "module.obj") {
    # COFF object payloads can contain non-deterministic metadata in this environment.
    Add-Check `
      -Id "runtime.positive.artifact.hash_recorded.module.obj" `
      -Passed $true `
      -Detail "positive parser scaffold module.obj hashes recorded; determinism is not enforced for this artifact" `
      -Evidence @{ run1_sha256 = $DigestRecord["run1_sha256"]; run2_sha256 = $DigestRecord["run2_sha256"] }
    return
  }

  Assert-Contract `
    -Condition $DigestRecord["deterministic"] `
    -Id ("runtime.positive.artifact.deterministic_sha256.{0}" -f $ArtifactName) `
    -FailureMessage ("positive parser scaffold artifact hash drift detected for {0}" -f $ArtifactName) `
    -PassMessage ("positive parser scaffold artifact hash stable for {0}" -f $ArtifactName) `
    -Evidence @{ run1_sha256 = $DigestRecord["run1_sha256"]; run2_sha256 = $DigestRecord["run2_sha256"] }
}
