"""Runtime acceptance action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import runtime_test_acceptance

RUNTIME_ACCEPTANCE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "test-runtime-acceptance": runtime_test_acceptance.action_test_runtime_acceptance,
    "test-runtime-acceptance-fast": runtime_test_acceptance.action_test_runtime_acceptance_fast,
    "test-runtime-acceptance-diagnostics": (
        runtime_test_acceptance.action_test_runtime_acceptance_diagnostics
    ),
    "test-runtime-acceptance-cross-module": (
        runtime_test_acceptance.action_test_runtime_acceptance_cross_module
    ),
    "test-runtime-acceptance-block-arc": (
        runtime_test_acceptance.action_test_runtime_acceptance_block_arc
    ),
    "test-runtime-acceptance-concurrency": (
        runtime_test_acceptance.action_test_runtime_acceptance_concurrency
    ),
}


__all__ = ["RUNTIME_ACCEPTANCE_ACTION_HANDLERS"]
