"""Release foundation action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import release_governance_foundation, schema_surfaces

RELEASE_FOUNDATION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-release-foundation-surface": release_governance_foundation.action_check_release_foundation_surface,
    "check-release-foundation-schema-surface": schema_surfaces.action_check_release_foundation_schema_surface,
    "check-release-abi-api-drift": release_governance_foundation.action_check_release_abi_api_drift,
    "validate-abi-governance": release_governance_foundation.action_validate_abi_governance,
    "build-release-manifest": release_governance_foundation.action_build_release_manifest,
    "publish-release-provenance": release_governance_foundation.action_publish_release_provenance,
    "validate-release-foundation": release_governance_foundation.action_validate_release_foundation,
}


__all__ = ["RELEASE_FOUNDATION_ACTION_HANDLERS"]
