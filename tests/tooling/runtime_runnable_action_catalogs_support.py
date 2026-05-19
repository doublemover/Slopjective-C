from __future__ import annotations

from runtime_runnable_action_support import (
    EXPECTED_CONFORMANCE_ACTIONS,
    EXPECTED_E2E_ACTIONS,
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
    RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
    RUNTIME_RUNNABLE_E2E_ACTION_SPECS,
    runtime_runnable_conformance,
    runtime_runnable_e2e,
)


def e2e_action_group_actions() -> list[str]:
    return [
        group.action for group in runtime_runnable_e2e.RUNTIME_RUNNABLE_E2E_ACTION_GROUPS
    ]


def conformance_action_group_actions() -> list[str]:
    return [
        group.action
        for group in runtime_runnable_conformance.RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS
    ]
