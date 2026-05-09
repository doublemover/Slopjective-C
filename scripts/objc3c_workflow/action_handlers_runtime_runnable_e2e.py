"""Runtime runnable end-to-end action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import runtime_runnable_e2e

RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "validate-runnable-bootstrap": runtime_runnable_e2e.action_validate_runnable_bootstrap,
    "validate-runnable-block-arc": runtime_runnable_e2e.action_validate_runnable_block_arc,
    "validate-runnable-concurrency": (
        runtime_runnable_e2e.action_validate_runnable_concurrency
    ),
    "validate-runnable-object-model": (
        runtime_runnable_e2e.action_validate_runnable_object_model
    ),
    "validate-runnable-storage-reflection": (
        runtime_runnable_e2e.action_validate_runnable_storage_reflection
    ),
    "validate-runnable-error": runtime_runnable_e2e.action_validate_runnable_error,
    "validate-runnable-interop": runtime_runnable_e2e.action_validate_runnable_interop,
    "validate-runnable-metaprogramming": (
        runtime_runnable_e2e.action_validate_runnable_metaprogramming
    ),
    "validate-runnable-release-candidate": (
        runtime_runnable_e2e.action_validate_runnable_release_candidate
    ),
}


__all__ = ["RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS"]
