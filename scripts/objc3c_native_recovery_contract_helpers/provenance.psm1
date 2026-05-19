Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "hash_io.psm1") -Force -DisableNameChecking

function Assert-CompileOutputProvenance {
  param(
    [string]$CaseName,
    [string]$RunDir
  )

  $provenancePath = Join-Path $RunDir "module.compile-provenance.json"
  if (!(Test-Path -LiteralPath $provenancePath -PathType Leaf)) {
    throw "contract FAIL: missing compile provenance artifact for $CaseName"
  }
  $registrationManifestPath = Join-Path $RunDir "module.runtime-registration-manifest.json"
  $hasRegistrationManifest = Test-Path -LiteralPath $registrationManifestPath -PathType Leaf

  $provenanceText = Get-Content -LiteralPath $provenancePath -Raw
  if ([string]::IsNullOrWhiteSpace($provenanceText)) {
    throw "contract FAIL: empty compile provenance artifact for $CaseName"
  }
  $provenance = Read-JsonHashtable -Path $provenancePath
  $registrationManifest = if ($hasRegistrationManifest) { Read-JsonHashtable -Path $registrationManifestPath } else { $null }

  if ([string]$provenance["contract_id"] -ne "objc3c.native.compile.output.provenance.v1") {
    throw "contract FAIL: unexpected compile provenance contract id for $CaseName"
  }
  if ($hasRegistrationManifest -and [string]$registrationManifest["compile_output_provenance_artifact"] -ne "module.compile-provenance.json") {
    throw "contract FAIL: runtime registration manifest missing compile provenance binding for $CaseName"
  }

  $truthfulness = $provenance["compile_output_truthfulness"]
  if ($null -eq $truthfulness -or [string]$truthfulness["contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "contract FAIL: missing compile output truthfulness envelope for $CaseName"
  }
  if (-not [bool]$truthfulness["truthful"]) {
    throw "contract FAIL: compile output truthfulness envelope did not certify emitted artifacts for $CaseName"
  }
  if ($hasRegistrationManifest -and [string]$registrationManifest["compile_output_truthfulness_contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "contract FAIL: runtime registration manifest missing compile output truthfulness contract id for $CaseName"
  }
  if ($hasRegistrationManifest -and -not [bool]$registrationManifest["compile_output_truthful"]) {
    throw "contract FAIL: runtime registration manifest did not certify truthful compile output for $CaseName"
  }
  if ($hasRegistrationManifest -and [string]$truthfulness["runtime_dispatch_symbol"] -ne [string]$registrationManifest["compile_output_truthfulness_runtime_dispatch_symbol"]) {
    throw "contract FAIL: runtime registration manifest compile output truthfulness dispatch symbol mismatch for $CaseName"
  }
  if ($hasRegistrationManifest -and [int]$truthfulness["property_descriptor_definition_count"] -ne [int]$registrationManifest["compile_output_truthfulness_property_descriptor_count"]) {
    throw "contract FAIL: runtime registration manifest compile output truthfulness property descriptor count mismatch for $CaseName"
  }
  if ($hasRegistrationManifest -and [int]$truthfulness["ivar_descriptor_definition_count"] -ne [int]$registrationManifest["compile_output_truthfulness_ivar_descriptor_count"]) {
    throw "contract FAIL: runtime registration manifest compile output truthfulness ivar descriptor count mismatch for $CaseName"
  }

  $artifactEntries = @($provenance["emitted_artifacts"])
  if ($artifactEntries.Count -lt 4) {
    throw "contract FAIL: compile provenance emitted_artifacts too small for $CaseName"
  }

  $digestLines = New-Object System.Collections.Generic.List[string]
  foreach ($entry in $artifactEntries) {
    $artifactPath = Join-Path $RunDir ([string]$entry["path"])
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "contract FAIL: compile provenance referenced missing artifact '$artifactPath' for $CaseName"
    }
    $actualHash = Get-FileSha256Hex -Path $artifactPath
    if ($actualHash -ne ([string]$entry["sha256"]).ToLowerInvariant()) {
      throw "contract FAIL: compile provenance sha256 mismatch for '$artifactPath' in $CaseName"
    }
    $actualSize = (Get-Item -LiteralPath $artifactPath).Length
    if ([long]$actualSize -ne [long]$entry["byte_count"]) {
      throw "contract FAIL: compile provenance byte_count mismatch for '$artifactPath' in $CaseName"
    }
    $digestLines.Add(("{0}|{1}|{2}" -f [string]$entry["path"], [string]$entry["byte_count"], [string]$entry["sha256"]))
  }

  $actualArtifactSetDigest = Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($digestLines -join "`n")))
  if ($actualArtifactSetDigest -ne ([string]$provenance["artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "contract FAIL: compile provenance artifact set digest mismatch for $CaseName"
  }
  if ($hasRegistrationManifest -and $actualArtifactSetDigest -ne ([string]$registrationManifest["compile_output_artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "contract FAIL: runtime registration manifest compile output digest mismatch for $CaseName"
  }

  return $provenanceText
}

Export-ModuleMember -Function @(
  "Assert-CompileOutputProvenance"
)
