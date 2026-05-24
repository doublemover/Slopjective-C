"""Packaging channel action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import release_governance, schema_surfaces

PACKAGING_CHANNEL_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-packaging-channels-surface": release_governance.action_check_packaging_channels_surface,
    "check-packaging-channels-schema-surface": schema_surfaces.action_check_packaging_channels_schema_surface,
    "build-package-channels": release_governance.action_build_package_channels,
    "build-package-channels-asan": release_governance.action_build_package_channels_asan,
    "build-package-channels-ubsan": release_governance.action_build_package_channels_ubsan,
    "build-platform-support-matrix": release_governance.action_build_platform_support_matrix,
    "ingest-platform-host-evidence": release_governance.action_ingest_platform_host_evidence,
    "validate-packaging-channels": release_governance.action_validate_packaging_channels,
    "validate-packaging-channels-end-to-end": release_governance.action_validate_packaging_channels_end_to_end,
    "validate-platform-hardening": release_governance.action_validate_platform_hardening,
    "validate-platform-hardening-end-to-end": release_governance.action_validate_platform_hardening_end_to_end,
}


__all__ = ["PACKAGING_CHANNEL_ACTION_HANDLERS"]
