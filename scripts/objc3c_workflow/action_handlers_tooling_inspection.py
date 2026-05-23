"""Developer tooling and application inspection handler section."""

from __future__ import annotations

import sys

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import (
    application_surfaces,
    developer_tooling_bonus,
    developer_tooling_dump_actions,
    developer_tooling_playground,
    developer_tooling_llvm_parity,
    validation_timing,
)
from scripts.objc3c_workflow.actions.developer_tooling_paths import (
    CHECK_DEBUG_SOURCE_MAPS_PY,
    CHECK_DEBUGGER_INTEGRATION_PY,
    CHECK_LANGUAGE_SERVICE_PY,
    CHECK_OBJECT_MODEL_DEBUGGER_PROOF_PY,
    DEVELOPER_TOOLING_INTEGRATION_PY,
    RUNTIME_DEBUG_TRACE_PY,
    RUNNABLE_DEVELOPER_TOOLING_E2E_PY,
)
from scripts.objc3c_workflow.commands import run


def _run_python_script(script_path: object) -> int:
    return run([sys.executable, str(script_path)])


def _run_python_script_with_args(script_path: object, *args: str) -> int:
    return run([sys.executable, str(script_path), *args])


def _action_validate_developer_tooling(_: list[str]) -> int:
    return _run_python_script(DEVELOPER_TOOLING_INTEGRATION_PY)


def _action_validate_runnable_developer_tooling(_: list[str]) -> int:
    return _run_python_script(RUNNABLE_DEVELOPER_TOOLING_E2E_PY)


def _action_validate_language_service(_: list[str]) -> int:
    return _run_python_script(CHECK_LANGUAGE_SERVICE_PY)


def _action_validate_debug_source_maps(_: list[str]) -> int:
    return _run_python_script(CHECK_DEBUG_SOURCE_MAPS_PY)


def _action_validate_debugger_integration(rest: list[str]) -> int:
    return _run_python_script_with_args(CHECK_DEBUGGER_INTEGRATION_PY, *rest)


def _action_validate_object_model_debugger_proof(rest: list[str]) -> int:
    rc = developer_tooling_playground.ensure_frontend_runner_ready()
    if rc != 0:
        return rc
    return _run_python_script_with_args(CHECK_OBJECT_MODEL_DEBUGGER_PROOF_PY, *rest)


def _action_inspect_debug_map(rest: list[str]) -> int:
    return _run_python_script_with_args(CHECK_DEBUG_SOURCE_MAPS_PY, "--inspect", *rest)


def _action_trace_runtime_debug(rest: list[str]) -> int:
    return run([sys.executable, str(RUNTIME_DEBUG_TRACE_PY), *rest])


TOOLING_INSPECTION_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "inspect-debug-map": _action_inspect_debug_map,
    "inspect-bonus-tool-integration": developer_tooling_bonus.action_inspect_bonus_tool_integration,
    "inspect-validation-timing": validation_timing.action_inspect_validation_timing,
    "materialize-project-template": developer_tooling_bonus.action_materialize_project_template,
    "materialize-canonical-application-workspace": application_surfaces.action_materialize_canonical_application_workspace,
    "trace-runtime-debug": _action_trace_runtime_debug,
    "trace-compile-stages": developer_tooling_dump_actions.action_trace_compile_stages,
    "test-capability-routed-source-parity": developer_tooling_llvm_parity.action_test_capability_routed_source_parity,
    "validate-developer-tooling": _action_validate_developer_tooling,
    "validate-debugger-integration": _action_validate_debugger_integration,
    "validate-debug-source-maps": _action_validate_debug_source_maps,
    "validate-language-service": _action_validate_language_service,
    "validate-object-model-debugger-proof": _action_validate_object_model_debugger_proof,
    "validate-runnable-developer-tooling": _action_validate_runnable_developer_tooling,
    "validate-bonus-experiences": developer_tooling_bonus.action_validate_bonus_experiences,
    "validate-runnable-bonus-experiences": developer_tooling_bonus.action_validate_runnable_bonus_experiences,
    "validate-application-architecture": application_surfaces.action_validate_application_architecture,
    "validate-application-framework-samples": application_surfaces.action_validate_application_framework_samples,
    "validate-runnable-application-architecture": application_surfaces.action_validate_runnable_application_architecture,
}

__all__ = ["TOOLING_INSPECTION_ACTION_HANDLERS"]
