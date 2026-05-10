Set-StrictMode -Version Latest

function Assert-CompileOutputProvenance {
  param(
    [Parameter(Mandatory = $true)][string]$CaseId,
    [Parameter(Mandatory = $true)][string]$RunDir
  )

  $provenancePath = Join-Path $RunDir "module.compile-provenance.json"
  if (!(Test-Path -LiteralPath $provenancePath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing compile provenance artifact for $CaseId"
  }
  $registrationManifestPath = Join-Path $RunDir "module.runtime-registration-manifest.json"
  if (!(Test-Path -LiteralPath $registrationManifestPath -PathType Leaf)) {
    throw "execution replay proof FAIL: missing runtime registration manifest for $CaseId"
  }

  $provenanceText = Get-NormalizedTextFromFile -Path $provenancePath
  if ([string]::IsNullOrWhiteSpace($provenanceText)) {
    throw "execution replay proof FAIL: empty compile provenance artifact for $CaseId"
  }

  $provenance = Read-JsonHashtable -Path $provenancePath
  $registrationManifest = Read-JsonHashtable -Path $registrationManifestPath
  if ([string]$provenance["contract_id"] -ne "objc3c.native.compile.output.provenance.v1") {
    throw "execution replay proof FAIL: unexpected compile provenance contract id for $CaseId"
  }
  if ([string]$registrationManifest["compile_output_provenance_artifact"] -ne "module.compile-provenance.json") {
    throw "execution replay proof FAIL: runtime registration manifest missing compile provenance binding for $CaseId"
  }

  $truthfulness = $provenance["compile_output_truthfulness"]
  if ($null -eq $truthfulness -or [string]$truthfulness["contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "execution replay proof FAIL: missing compile output truthfulness envelope for $CaseId"
  }
  if (-not [bool]$truthfulness["truthful"]) {
    throw "execution replay proof FAIL: compile output truthfulness envelope did not certify emitted artifacts for $CaseId"
  }
  if ([string]$registrationManifest["compile_output_truthfulness_contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "execution replay proof FAIL: runtime registration manifest missing compile output truthfulness contract id for $CaseId"
  }
  if (-not [bool]$registrationManifest["compile_output_truthful"]) {
    throw "execution replay proof FAIL: runtime registration manifest did not certify truthful compile output for $CaseId"
  }

  $artifactEntries = @($provenance["emitted_artifacts"])
  if ($artifactEntries.Count -lt 4) {
    throw "execution replay proof FAIL: compile provenance emitted_artifacts too small for $CaseId"
  }

  $digestLines = New-Object System.Collections.Generic.List[string]
  foreach ($entry in $artifactEntries) {
    $artifactPath = Join-Path $RunDir ([string]$entry["path"])
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "execution replay proof FAIL: compile provenance referenced missing artifact '$artifactPath' for $CaseId"
    }
    $actualHash = Get-Sha256HexFromFile -Path $artifactPath
    if ($actualHash -ne ([string]$entry["sha256"]).ToLowerInvariant()) {
      throw "execution replay proof FAIL: compile provenance sha256 mismatch for '$artifactPath' in $CaseId"
    }
    $actualSize = (Get-Item -LiteralPath $artifactPath).Length
    if ([long]$actualSize -ne [long]$entry["byte_count"]) {
      throw "execution replay proof FAIL: compile provenance byte_count mismatch for '$artifactPath' in $CaseId"
    }
    $digestLines.Add(("{0}|{1}|{2}" -f [string]$entry["path"], [string]$entry["byte_count"], [string]$entry["sha256"]))
  }

  $actualArtifactSetDigest = Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($digestLines -join "`n")))
  if ($actualArtifactSetDigest -ne ([string]$provenance["artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "execution replay proof FAIL: compile provenance artifact set digest mismatch for $CaseId"
  }
  if ($actualArtifactSetDigest -ne ([string]$registrationManifest["compile_output_artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "execution replay proof FAIL: runtime registration manifest compile output digest mismatch for $CaseId"
  }

  return [ordered]@{
    provenance_path = Get-RepoRelativePath -Path $provenancePath -Root $script:repoRoot
    registration_manifest_path = Get-RepoRelativePath -Path $registrationManifestPath -Root $script:repoRoot
    provenance_sha256 = Get-Sha256HexFromText -Text $provenanceText
    registration_manifest_sha256 = Get-Sha256HexFromText -Text (Get-NormalizedTextFromFile -Path $registrationManifestPath)
    artifact_set_digest_sha256 = $actualArtifactSetDigest
  }
}
