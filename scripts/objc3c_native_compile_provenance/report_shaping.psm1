$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-CompileOutputProvenancePayload {
  param(
    [string]$EmitPrefix,
    [string]$ProvenanceFileName,
    [string]$InputSourceDisplay,
    [string]$InputSourceHash,
    [string]$CompilerBinaryDisplay,
    [string]$CompilerBinaryHash,
    [string]$RuntimeLibraryDisplay,
    [string]$RuntimeLibraryHash,
    [string]$WrapperScriptDisplay,
    [string]$WrapperScriptHash,
    $Truthfulness,
    [object[]]$ArtifactEntries,
    [string]$ArtifactSetDigest
  )

  $artifactEntryArray = @($ArtifactEntries)
  $payload = [ordered]@{}
  $payload["contract_id"] = "objc3c.native.compile.output.provenance.v1"
  $payload["provenance_artifact"] = $ProvenanceFileName
  $payload["manifest_artifact"] = "$EmitPrefix.manifest.json"
  $payload["registration_manifest_artifact"] = "$EmitPrefix.runtime-registration-manifest.json"
  $payload["input_source"] = $InputSourceDisplay
  $payload["input_source_sha256"] = $InputSourceHash
  $payload["compiler_binary"] = $CompilerBinaryDisplay
  $payload["compiler_binary_sha256"] = $CompilerBinaryHash
  $payload["runtime_support_library"] = $RuntimeLibraryDisplay
  $payload["runtime_support_library_sha256"] = $RuntimeLibraryHash
  $payload["compile_wrapper_script"] = $WrapperScriptDisplay
  $payload["compile_wrapper_script_sha256"] = $WrapperScriptHash
  $payload["replay_verification_model"] = "artifact-set-digest-plus-per-file-sha256-over-real-emitted-compile-outputs"
  $payload["compile_output_truthfulness"] = $Truthfulness
  $payload["artifact_count"] = $artifactEntryArray.Count
  $payload["artifact_set_digest_sha256"] = $ArtifactSetDigest
  $payload["emitted_artifacts"] = $artifactEntryArray
  return $payload
}

function Set-CompileOutputRegistrationManifestProvenanceFields {
  param(
    [string]$CompileDir,
    [string]$EmitPrefix,
    [string]$ProvenanceFileName,
    $Truthfulness,
    [int]$ArtifactCount,
    [string]$ArtifactSetDigest
  )

  $registrationManifestPath = Join-Path $CompileDir ($EmitPrefix + ".runtime-registration-manifest.json")
  if (Test-Path -LiteralPath $registrationManifestPath -PathType Leaf) {
    $registrationManifest =
      Get-Content -LiteralPath $registrationManifestPath -Raw |
      ConvertFrom-Json -AsHashtable
    $registrationManifest["compile_output_provenance_contract_id"] = "objc3c.native.compile.output.provenance.v1"
    $registrationManifest["compile_output_provenance_artifact"] = $ProvenanceFileName
    $registrationManifest["compile_output_truthfulness_contract_id"] = [string]$Truthfulness["contract_id"]
    $registrationManifest["compile_output_truthful"] = [bool]$Truthfulness["truthful"]
    $registrationManifest["compile_output_truthfulness_runtime_dispatch_symbol"] = [string]$Truthfulness["runtime_dispatch_symbol"]
    $registrationManifest["compile_output_truthfulness_property_descriptor_count"] = [int]$Truthfulness["property_descriptor_definition_count"]
    $registrationManifest["compile_output_truthfulness_ivar_descriptor_count"] = [int]$Truthfulness["ivar_descriptor_definition_count"]
    $registrationManifest["compile_output_artifact_count"] = $ArtifactCount
    $registrationManifest["compile_output_artifact_set_digest_sha256"] = $ArtifactSetDigest
    Set-Content -LiteralPath $registrationManifestPath -Value ($registrationManifest | ConvertTo-Json -Depth 64) -Encoding utf8
  }
}

Export-ModuleMember -Function @(
  "New-CompileOutputProvenancePayload",
  "Set-CompileOutputRegistrationManifestProvenanceFields"
)
