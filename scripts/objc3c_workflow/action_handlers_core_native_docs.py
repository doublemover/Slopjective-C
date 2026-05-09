"""Core native documentation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_NATIVE_DOCS_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-native-docs": docs.action_build_native_docs,
    "check-native-docs": docs.action_check_native_docs,
}

__all__ = ["CORE_NATIVE_DOCS_ACTION_HANDLERS"]
