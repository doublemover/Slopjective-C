"""Stdlib workspace and validation handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import application_surfaces

APPLICATION_STDLIB_HANDLER_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_handlers_application_stdlib.py"
)

APPLICATION_STDLIB_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "materialize-stdlib-workspace": application_surfaces.action_materialize_stdlib_workspace,
    "validate-string-text-model-runtime": application_surfaces.action_validate_string_text_model_runtime,
    "validate-stdlib-foundation": application_surfaces.action_validate_stdlib_foundation,
    "validate-stdlib-advanced": application_surfaces.action_validate_stdlib_advanced,
    "validate-stdlib-program": application_surfaces.action_validate_stdlib_program,
    "validate-runnable-stdlib-advanced": application_surfaces.action_validate_runnable_stdlib_advanced,
    "validate-runnable-stdlib-foundation": application_surfaces.action_validate_runnable_stdlib_foundation,
    "validate-runnable-stdlib-program": application_surfaces.action_validate_runnable_stdlib_program,
}


__all__ = [
    "APPLICATION_STDLIB_ACTION_HANDLERS",
    "APPLICATION_STDLIB_HANDLER_OWNER_SURFACE",
]
