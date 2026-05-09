"""Release operation action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import release_governance, schema_surfaces

RELEASE_OPERATIONS_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-release-operations-surface": release_governance.action_check_release_operations_surface,
    "check-release-operations-schema-surface": schema_surfaces.action_check_release_operations_schema_surface,
    "build-update-manifest": release_governance.action_build_update_manifest,
    "publish-release-operations": release_governance.action_publish_release_operations,
    "validate-release-operations": release_governance.action_validate_release_operations,
    "validate-release-operations-end-to-end": release_governance.action_validate_release_operations_end_to_end,
}


__all__ = ["RELEASE_OPERATIONS_ACTION_HANDLERS"]
