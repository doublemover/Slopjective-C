"""Runtime architecture action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import runtime_test_acceptance

RUNTIME_ARCHITECTURE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "proof-runtime-architecture": runtime_test_acceptance.action_proof_runtime_architecture,
    "validate-runtime-architecture": (
        runtime_test_acceptance.action_validate_runtime_architecture
    ),
    "validate-public-runtime-reflection-api": (
        runtime_test_acceptance.action_validate_public_runtime_reflection_api
    ),
    "validate-advanced-runtime-closure": (
        runtime_test_acceptance.action_validate_advanced_runtime_closure
    ),
}


__all__ = ["RUNTIME_ARCHITECTURE_ACTION_HANDLERS"]
