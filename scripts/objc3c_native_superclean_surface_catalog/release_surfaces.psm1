$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "report_rendering.psm1") -Force -DisableNameChecking

function New-Objc3cNativeRepoSupercleanReleaseSurfaces {
  return New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
    release_foundation_surface = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
      runbook = "docs/runbooks/objc3c_release_foundation.md"
      source_surface_contract = "tests/tooling/fixtures/release_foundation/source_surface.json"
      artifact_taxonomy = "tests/tooling/fixtures/release_foundation/artifact_taxonomy.json"
      distribution_trust_model = "tests/tooling/fixtures/release_foundation/distribution_trust_model.json"
      distribution_audit = "tests/tooling/fixtures/release_foundation/distribution_audit.json"
      reproducibility_policy = "tests/tooling/fixtures/release_foundation/reproducibility_policy.json"
      release_payload_policy = "tests/tooling/fixtures/release_foundation/release_payload_policy.json"
      provenance_policy = "tests/tooling/fixtures/release_foundation/provenance_policy.json"
      workflow_surface = "tests/tooling/fixtures/release_foundation/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/release_foundation/schema_surface.json"
      release_manifest_schema = "schemas/objc3c-release-manifest-v1.schema.json"
      release_sbom_schema = "schemas/objc3c-release-sbom-v1.schema.json"
      release_attestation_schema = "schemas/objc3c-release-attestation-v1.schema.json"
      source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "scripts/check_release_foundation_source_surface.py",
        "scripts/check_release_foundation_schema_surface.py",
        "scripts/build_objc3c_release_manifest.py",
        "scripts/publish_objc3c_release_provenance.py",
        "scripts/check_objc3c_release_foundation_integration.py",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "scripts/check_release_evidence.py"
      )
      report_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "tmp/reports/release-foundation",
        "tmp/artifacts/release-foundation",
        "tmp/pkg/objc3c-release-foundation"
      )
      public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
        "check-release-foundation-surface",
        "check-release-foundation-schema-surface",
        "build-release-manifest",
        "publish-release-provenance",
        "validate-release-foundation"
      )
    })
    packaging_channels_surface = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
      runbook = "docs/runbooks/objc3c_packaging_channels.md"
      source_surface_contract = "tests/tooling/fixtures/packaging_channels/source_surface.json"
      supported_platforms = "tests/tooling/fixtures/packaging_channels/supported_platforms.json"
      installer_policy = "tests/tooling/fixtures/packaging_channels/installer_policy.json"
      metadata_surface = "tests/tooling/fixtures/packaging_channels/metadata_surface.json"
      workflow_surface = "tests/tooling/fixtures/packaging_channels/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/packaging_channels/schema_surface.json"
      package_channels_manifest_schema = "schemas/objc3c-package-channels-manifest-v1.schema.json"
      install_receipt_schema = "schemas/objc3c-package-install-receipt-v1.schema.json"
      source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "scripts/check_packaging_channels_source_surface.py",
        "scripts/check_packaging_channels_schema_surface.py",
        "scripts/build_objc3c_package_channels.py",
        "scripts/check_objc3c_packaging_channels_integration.py",
        "scripts/check_objc3c_packaging_channels_end_to_end.py",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "scripts/build_objc3c_release_manifest.py",
        "scripts/publish_objc3c_release_provenance.py"
      )
      report_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "tmp/reports/package-channels",
        "tmp/artifacts/package-channels",
        "tmp/pkg/objc3c-package-channels"
      )
      public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
        "check-packaging-channels-surface",
        "check-packaging-channels-schema-surface",
        "build-package-channels",
        "validate-packaging-channels",
        "validate-packaging-channels-end-to-end"
      )
    })
    release_operations_surface = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
      runbook = "docs/runbooks/objc3c_release_operations.md"
      source_surface_contract = "tests/tooling/fixtures/release_operations/source_surface.json"
      versioning_model = "tests/tooling/fixtures/release_operations/versioning_model.json"
      upgrade_support_claim_policy = "tests/tooling/fixtures/release_operations/upgrade_support_claim_policy.json"
      update_channel_policy = "tests/tooling/fixtures/release_operations/update_channel_policy.json"
      fail_closed_diagnostics_policy = "tests/tooling/fixtures/release_operations/fail_closed_diagnostics_policy.json"
      metadata_surface = "tests/tooling/fixtures/release_operations/metadata_surface.json"
      workflow_surface = "tests/tooling/fixtures/release_operations/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/release_operations/schema_surface.json"
      update_manifest_schema = "schemas/objc3c-update-manifest-v1.schema.json"
      upgrade_support_report_schema = "schemas/objc3c-upgrade-support-report-v1.schema.json"
      source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "scripts/check_release_operations_source_surface.py",
        "scripts/check_release_operations_schema_surface.py",
        "scripts/build_objc3c_update_manifest.py",
        "scripts/publish_objc3c_release_operations_metadata.py",
        "scripts/check_objc3c_release_operations_integration.py",
        "scripts/check_objc3c_release_operations_end_to_end.py",
        "scripts/build_objc3c_package_channels.py",
        "scripts/build_objc3c_release_manifest.py"
      )
      report_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "tmp/reports/release-operations",
        "tmp/artifacts/release-operations",
        "tmp/reports/objc3c-public-workflow"
      )
      public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
        "check-release-operations-surface",
        "check-release-operations-schema-surface",
        "build-update-manifest",
        "publish-release-operations",
        "validate-release-operations",
        "validate-release-operations-end-to-end"
      )
    })
    distribution_credibility_surface = New-Objc3cNativeRepoSupercleanCatalogPayload -Entries ([ordered]@{
      runbook = "docs/runbooks/objc3c_distribution_credibility.md"
      source_surface_contract = "tests/tooling/fixtures/distribution_credibility/source_surface.json"
      trust_signal_architecture = "tests/tooling/fixtures/distribution_credibility/trust_signal_architecture.json"
      install_release_doc_surface = "tests/tooling/fixtures/distribution_credibility/install_release_doc_surface.json"
      operator_release_policy = "tests/tooling/fixtures/distribution_credibility/operator_release_policy.json"
      release_drill_policy = "tests/tooling/fixtures/distribution_credibility/release_drill_policy.json"
      artifact_surface = "tests/tooling/fixtures/distribution_credibility/artifact_surface.json"
      workflow_surface = "tests/tooling/fixtures/distribution_credibility/workflow_surface.json"
      schema_surface = "tests/tooling/fixtures/distribution_credibility/schema_surface.json"
      dashboard_schema = "schemas/objc3c-distribution-credibility-dashboard-v1.schema.json"
      trust_report_schema = "schemas/objc3c-distribution-trust-report-v1.schema.json"
      source_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "scripts/check_distribution_credibility_source_surface.py",
        "scripts/check_distribution_credibility_schema_surface.py",
        "scripts/build_objc3c_distribution_credibility_dashboard.py",
        "scripts/publish_objc3c_distribution_trust_report.py",
        "scripts/check_objc3c_distribution_credibility_integration.py",
        "scripts/check_objc3c_distribution_credibility_end_to_end.py",
        "scripts/check_release_evidence.py",
        "scripts/publish_objc3c_release_operations_metadata.py"
      )
      report_roots = New-Objc3cNativeRepoSupercleanPathList -Paths @(
        "tmp/reports/distribution-credibility",
        "tmp/artifacts/distribution-credibility",
        "tmp/reports/objc3c-public-workflow"
      )
      public_actions = New-Objc3cNativeRepoSupercleanActionList -Actions @(
        "check-distribution-credibility-surface",
        "check-distribution-credibility-schema-surface",
        "build-distribution-credibility-dashboard",
        "publish-distribution-credibility",
        "validate-distribution-credibility",
        "validate-distribution-credibility-end-to-end"
      )
    })
  })
}

Export-ModuleMember -Function "New-Objc3cNativeRepoSupercleanReleaseSurfaces"
