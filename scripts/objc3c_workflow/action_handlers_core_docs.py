"""Core docs and public command handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_handler_sections import merge_action_handler_sections
from scripts.objc3c_workflow.action_handlers_core_documentation_surface import (
    CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_documentation_validation import (
    CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_markdown import (
    CORE_MARKDOWN_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_native_docs import (
    CORE_NATIVE_DOCS_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_public_commands import (
    CORE_PUBLIC_COMMAND_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_core_site_docs import (
    CORE_SITE_DOCS_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_spec import ActionHandler

CORE_DOCS_ACTION_HANDLERS: dict[str, ActionHandler] = merge_action_handler_sections(
    CORE_SITE_DOCS_ACTION_HANDLERS,
    CORE_NATIVE_DOCS_ACTION_HANDLERS,
    CORE_PUBLIC_COMMAND_ACTION_HANDLERS,
    CORE_DOCUMENTATION_SURFACE_ACTION_HANDLERS,
    CORE_MARKDOWN_ACTION_HANDLERS,
    CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS,
)

__all__ = ["CORE_DOCS_ACTION_HANDLERS"]
