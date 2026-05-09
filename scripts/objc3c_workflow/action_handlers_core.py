"""Core build, docs, hygiene, and command-surface action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_core_build import CORE_BUILD_ACTION_HANDLERS
from scripts.objc3c_workflow.action_handlers_core_docs import CORE_DOCS_ACTION_HANDLERS
from scripts.objc3c_workflow.action_handlers_core_hygiene import (
    CORE_HYGIENE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_showcase import (
    CORE_SHOWCASE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

CORE_ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    CORE_BUILD_ACTION_HANDLERS,
    CORE_DOCS_ACTION_HANDLERS,
    CORE_HYGIENE_ACTION_HANDLERS,
    CORE_SHOWCASE_ACTION_HANDLERS,
)
