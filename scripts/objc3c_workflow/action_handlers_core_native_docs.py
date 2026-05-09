"""Core native documentation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions.docs_documentation import (
    BUILD_NATIVE_DOCS_ACTION,
    CHECK_NATIVE_DOCS_ACTION,
    action_build_native_docs,
    action_check_native_docs,
)

CORE_NATIVE_DOCS_ACTION_HANDLERS: dict[str, ActionHandler] = {
    BUILD_NATIVE_DOCS_ACTION: action_build_native_docs,
    CHECK_NATIVE_DOCS_ACTION: action_check_native_docs,
}

__all__ = ["CORE_NATIVE_DOCS_ACTION_HANDLERS"]
