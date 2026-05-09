"""Release foundation, packaging, operations, and distribution handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import release_governance, schema_surfaces

REPORTING_RELEASE_CHANNEL_HANDLERS: dict[str, ActionHandler] = {
    "check-release-foundation-surface": release_governance.action_check_release_foundation_surface,
    "check-release-foundation-schema-surface": schema_surfaces.action_check_release_foundation_schema_surface,
    "build-release-manifest": release_governance.action_build_release_manifest,
    "publish-release-provenance": release_governance.action_publish_release_provenance,
    "validate-release-foundation": release_governance.action_validate_release_foundation,
    "check-packaging-channels-surface": release_governance.action_check_packaging_channels_surface,
    "check-packaging-channels-schema-surface": schema_surfaces.action_check_packaging_channels_schema_surface,
    "build-package-channels": release_governance.action_build_package_channels,
    "build-platform-support-matrix": release_governance.action_build_platform_support_matrix,
    "validate-packaging-channels": release_governance.action_validate_packaging_channels,
    "validate-packaging-channels-end-to-end": release_governance.action_validate_packaging_channels_end_to_end,
    "validate-platform-hardening": release_governance.action_validate_platform_hardening,
    "validate-platform-hardening-end-to-end": release_governance.action_validate_platform_hardening_end_to_end,
    "check-release-operations-surface": release_governance.action_check_release_operations_surface,
    "check-release-operations-schema-surface": schema_surfaces.action_check_release_operations_schema_surface,
    "build-update-manifest": release_governance.action_build_update_manifest,
    "publish-release-operations": release_governance.action_publish_release_operations,
    "validate-release-operations": release_governance.action_validate_release_operations,
    "validate-release-operations-end-to-end": release_governance.action_validate_release_operations_end_to_end,
    "check-distribution-credibility-surface": release_governance.action_check_distribution_credibility_surface,
    "check-distribution-credibility-schema-surface": schema_surfaces.action_check_distribution_credibility_schema_surface,
    "build-distribution-credibility-dashboard": release_governance.action_build_distribution_credibility_dashboard,
    "publish-distribution-credibility": release_governance.action_publish_distribution_credibility,
    "validate-distribution-credibility": release_governance.action_validate_distribution_credibility,
    "validate-distribution-credibility-end-to-end": release_governance.action_validate_distribution_credibility_end_to_end,
}
