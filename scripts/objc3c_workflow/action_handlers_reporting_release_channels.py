"""Release foundation, packaging, operations, and distribution handler facade."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_distribution_credibility import (
    DISTRIBUTION_CREDIBILITY_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_packaging_channels import (
    PACKAGING_CHANNEL_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_release_foundation import (
    RELEASE_FOUNDATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_release_operations import (
    RELEASE_OPERATIONS_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

REPORTING_RELEASE_CHANNEL_HANDLERS: dict[str, ActionHandler] = (
    merge_action_handler_sections(
        RELEASE_FOUNDATION_ACTION_HANDLERS,
        PACKAGING_CHANNEL_ACTION_HANDLERS,
        RELEASE_OPERATIONS_ACTION_HANDLERS,
        DISTRIBUTION_CREDIBILITY_ACTION_HANDLERS,
    )
)


__all__ = ["REPORTING_RELEASE_CHANNEL_HANDLERS"]
