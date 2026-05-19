$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "hash_io.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "path_config.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "truthfulness.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "artifact_inventory.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "report_shaping.psm1") -Force -DisableNameChecking

function Write-CompileOutputProvenance {
  param(
    [string]$RepoRoot,
    [string]$CompileDir,
    [string]$EmitPrefix,
    $InputPath,
    $CompilerBinaryPath,
    $RuntimeLibraryPath,
    $WrapperScriptPath
  )

  if ([string]::IsNullOrWhiteSpace($CompileDir) -or !(Test-Path -LiteralPath $CompileDir -PathType Container)) {
    return
  }
  if ([string]::IsNullOrWhiteSpace($EmitPrefix)) {
    $EmitPrefix = "module"
  }
  $inputPathText = if ($null -eq $InputPath) { "" } else { [string]$InputPath }
  $compilerBinaryPathText = if ($null -eq $CompilerBinaryPath) { "" } else { [string]$CompilerBinaryPath }
  $runtimeLibraryPathText = if ($null -eq $RuntimeLibraryPath) { "" } else { [string]$RuntimeLibraryPath }
  $wrapperScriptPathText = if ($null -eq $WrapperScriptPath) { "" } else { [string]$WrapperScriptPath }
  $inputSourceDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $inputPathText
  $inputSourceHash = Get-OptionalFileHash -Path $inputPathText
  $compilerBinaryDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $compilerBinaryPathText
  $compilerBinaryHash = Get-OptionalFileHash -Path $compilerBinaryPathText
  $runtimeLibraryDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $runtimeLibraryPathText
  $runtimeLibraryHash = Get-OptionalFileHash -Path $runtimeLibraryPathText
  $wrapperScriptDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $wrapperScriptPathText
  $wrapperScriptHash = Get-OptionalFileHash -Path $wrapperScriptPathText
  $truthfulness = Get-CompileOutputTruthfulness -CompileDir $CompileDir -EmitPrefix $EmitPrefix
  if (-not [bool]$truthfulness["truthful"]) {
    $truthfulnessFailures = @($truthfulness["failures"])
    $failureSummary = if ($truthfulnessFailures.Count -gt 0) { $truthfulnessFailures -join "; " } else { "unknown cross-check failure" }
    throw "compile output truthfulness check failed: $failureSummary"
  }

  $provenanceFileName = "$EmitPrefix.compile-provenance.json"
  $provenancePath = Join-Path $CompileDir $provenanceFileName
  $artifactEntryArray = @(
    Get-CompileOutputArtifactEntries `
      -CompileDir $CompileDir `
      -EmitPrefix $EmitPrefix `
      -ProvenanceFileName $provenanceFileName
  )
  $artifactSetDigest = Get-CompileOutputArtifactSetDigest -ArtifactEntries $artifactEntryArray
  $payload = New-CompileOutputProvenancePayload `
    -EmitPrefix $EmitPrefix `
    -ProvenanceFileName $provenanceFileName `
    -InputSourceDisplay $inputSourceDisplay `
    -InputSourceHash $inputSourceHash `
    -CompilerBinaryDisplay $compilerBinaryDisplay `
    -CompilerBinaryHash $compilerBinaryHash `
    -RuntimeLibraryDisplay $runtimeLibraryDisplay `
    -RuntimeLibraryHash $runtimeLibraryHash `
    -WrapperScriptDisplay $wrapperScriptDisplay `
    -WrapperScriptHash $wrapperScriptHash `
    -Truthfulness $truthfulness `
    -ArtifactEntries $artifactEntryArray `
    -ArtifactSetDigest $artifactSetDigest
  Set-Content -LiteralPath $provenancePath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8

  Set-CompileOutputRegistrationManifestProvenanceFields `
    -CompileDir $CompileDir `
    -EmitPrefix $EmitPrefix `
    -ProvenanceFileName $provenanceFileName `
    -Truthfulness $truthfulness `
    -ArtifactCount $artifactEntryArray.Count `
    -ArtifactSetDigest $artifactSetDigest
}

Export-ModuleMember -Function "Write-CompileOutputProvenance"
