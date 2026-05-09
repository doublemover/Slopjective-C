"""Developer tooling and application inspection handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import (
    application_surfaces,
    developer_tooling,
    validation_timing,
)

TOOLING_INSPECTION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "inspect-bonus-tool-integration": developer_tooling.action_inspect_bonus_tool_integration,
    "inspect-validation-timing": validation_timing.action_inspect_validation_timing,
    "materialize-project-template": developer_tooling.action_materialize_project_template,
    "materialize-canonical-application-workspace": application_surfaces.action_materialize_canonical_application_workspace,
    "trace-compile-stages": developer_tooling.action_trace_compile_stages,
    "test-capability-routed-source-parity": developer_tooling.action_test_capability_routed_source_parity,
    "validate-developer-tooling": developer_tooling.action_validate_developer_tooling,
    "validate-runnable-developer-tooling": developer_tooling.action_validate_runnable_developer_tooling,
    "validate-bonus-experiences": developer_tooling.action_validate_bonus_experiences,
    "validate-runnable-bonus-experiences": developer_tooling.action_validate_runnable_bonus_experiences,
    "validate-application-architecture": application_surfaces.action_validate_application_architecture,
    "validate-runnable-application-architecture": application_surfaces.action_validate_runnable_application_architecture,
}

__all__ = ["TOOLING_INSPECTION_ACTION_HANDLERS"]
