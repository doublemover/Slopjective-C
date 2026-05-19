"""Distribution credibility action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import release_governance, schema_surfaces

DISTRIBUTION_CREDIBILITY_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-distribution-credibility-surface": release_governance.action_check_distribution_credibility_surface,
    "check-distribution-credibility-schema-surface": schema_surfaces.action_check_distribution_credibility_schema_surface,
    "build-distribution-credibility-dashboard": release_governance.action_build_distribution_credibility_dashboard,
    "publish-distribution-credibility": release_governance.action_publish_distribution_credibility,
    "validate-distribution-credibility": release_governance.action_validate_distribution_credibility,
    "validate-distribution-credibility-end-to-end": release_governance.action_validate_distribution_credibility_end_to_end,
}


__all__ = ["DISTRIBUTION_CREDIBILITY_ACTION_HANDLERS"]
