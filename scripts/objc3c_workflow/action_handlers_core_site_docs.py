"""Core site handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import docs

CORE_SITE_DOCS_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "build-site": docs.action_build_site,
    "check-site": docs.action_check_site,
}

__all__ = ["CORE_SITE_DOCS_ACTION_HANDLERS"]
