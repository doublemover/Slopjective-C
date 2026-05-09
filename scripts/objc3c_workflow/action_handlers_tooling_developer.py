"""Developer-tooling, ecosystem, and application handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_ecosystem_publication import (
    ECOSYSTEM_PUBLICATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_hygiene import (
    TOOLING_HYGIENE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_tooling_inspection import (
    TOOLING_INSPECTION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

DEVELOPER_ECOSYSTEM_ACTION_HANDLERS: dict[str, ActionHandler] = (
    merge_action_handler_sections(
        TOOLING_INSPECTION_ACTION_HANDLERS,
        ECOSYSTEM_PUBLICATION_ACTION_HANDLERS,
        TOOLING_HYGIENE_ACTION_HANDLERS,
    )
)

__all__ = ["DEVELOPER_ECOSYSTEM_ACTION_HANDLERS"]
