"""Core showcase and getting-started handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import application_surfaces

CORE_SHOWCASE_HANDLER_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_handlers_core_showcase.py"
)

CORE_SHOWCASE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-showcase-surface": application_surfaces.action_check_showcase_surface,
    "check-stdlib-surface": application_surfaces.action_check_stdlib_surface,
    "validate-showcase-runtime": application_surfaces.action_validate_showcase_runtime,
    "validate-showcase": application_surfaces.action_validate_showcase,
    "validate-runnable-showcase": application_surfaces.action_validate_runnable_showcase,
    "validate-getting-started": application_surfaces.action_validate_getting_started,
}


__all__ = ["CORE_SHOWCASE_ACTION_HANDLERS", "CORE_SHOWCASE_HANDLER_OWNER_SURFACE"]
