Set-StrictMode -Version Latest

function Get-ManifestProvenanceReleaseSurfaceFiles {
  return @(
    "schemas/objc3c-application-architecture-evidence-summary-v1.schema.json",
    "schemas/objc3c-developer-tooling-editor-surface-v1.schema.json",
    "schemas/objc3-conformance-dashboard-status-v1.schema.json",
    "schemas/objc3c-platform-support-matrix-v1.schema.json",
    "schemas/objc3c-package-channels-manifest-v1.schema.json",
    "schemas/objc3c-package-install-receipt-v1.schema.json",
    "schemas/objc3c-package-lock-v1.schema.json",
    "schemas/objc3c-package-offline-mirror-index-v1.schema.json",
    "schemas/objc3c-adoption-legibility-evidence-v1.schema.json",
    "schemas/objc3c-governance-sustainability-evidence-v1.schema.json",
    "schemas/objc3c-update-manifest-v1.schema.json",
    "schemas/objc3c-upgrade-support-report-v1.schema.json",
    "schemas/objc3c-release-manifest-v1.schema.json",
    "schemas/objc3c-release-sbom-v1.schema.json",
    "schemas/objc3c-release-attestation-v1.schema.json",
    "scripts/check_release_evidence.py",
    "scripts/check_release_foundation_source_surface.py",
    "scripts/check_release_foundation_schema_surface.py",
    "scripts/build_objc3c_release_manifest.py",
    "scripts/publish_objc3c_release_provenance.py",
    "scripts/check_objc3c_release_foundation_integration.py",
    "scripts/check_packaging_channels_source_surface.py",
    "scripts/check_packaging_channels_schema_surface.py",
    "scripts/build_objc3c_package_channels.py",
    "scripts/check_objc3c_packaging_channels_integration.py",
    "scripts/check_objc3c_packaging_channels_end_to_end.py",
    "scripts/check_release_operations_source_surface.py",
    "scripts/check_release_operations_schema_surface.py",
    "scripts/build_objc3c_update_manifest.py",
    "scripts/publish_objc3c_release_operations_metadata.py",
    "scripts/check_objc3c_release_operations_integration.py",
    "scripts/check_objc3c_release_operations_end_to_end.py"
  )
}

Export-ModuleMember -Function @("Get-ManifestProvenanceReleaseSurfaceFiles")
