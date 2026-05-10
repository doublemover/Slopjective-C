Set-StrictMode -Version Latest

function Get-ManifestProvenanceReleaseFoundationFixtureFiles {
  return @(
    "tests/tooling/fixtures/release_foundation/artifact_taxonomy.json",
    "tests/tooling/fixtures/release_foundation/distribution_trust_model.json",
    "tests/tooling/fixtures/release_foundation/distribution_audit.json",
    "tests/tooling/fixtures/release_foundation/reproducibility_policy.json",
    "tests/tooling/fixtures/release_foundation/release_payload_policy.json",
    "tests/tooling/fixtures/release_foundation/provenance_policy.json",
    "tests/tooling/fixtures/release_foundation/source_surface.json",
    "tests/tooling/fixtures/release_foundation/schema_surface.json",
    "tests/tooling/fixtures/release_foundation/workflow_surface.json"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceReleaseFoundationFixtureFiles")
