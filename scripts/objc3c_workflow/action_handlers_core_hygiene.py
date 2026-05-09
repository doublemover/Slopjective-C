"""Core hygiene and capability handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_core_capabilities import (
    CORE_CAPABILITY_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_linting import (
    CORE_LINTING_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_source_hygiene import (
    CORE_SOURCE_HYGIENE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_superclean import (
    CORE_SUPERCLEAN_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

CORE_HYGIENE_ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    CORE_LINTING_ACTION_HANDLERS,
    CORE_CAPABILITY_ACTION_HANDLERS,
    {"check-release-evidence": CORE_SUPERCLEAN_ACTION_HANDLERS["check-release-evidence"]},
    CORE_SOURCE_HYGIENE_ACTION_HANDLERS,
    {
        "check-repo-superclean-surface": CORE_SUPERCLEAN_ACTION_HANDLERS[
            "check-repo-superclean-surface"
        ],
        "validate-repo-superclean": CORE_SUPERCLEAN_ACTION_HANDLERS[
            "validate-repo-superclean"
        ],
    },
)

__all__ = ["CORE_HYGIENE_ACTION_HANDLERS"]
