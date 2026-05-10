from runtime_runnable_action_catalogs_support import (
    EXPECTED_CONFORMANCE_ACTIONS,
    EXPECTED_E2E_ACTIONS,
    conformance_action_group_actions,
    e2e_action_group_actions,
)


def runtime_runnable_child_order_is_explicit() -> None:
    assert e2e_action_group_actions() == EXPECTED_E2E_ACTIONS
    assert conformance_action_group_actions() == EXPECTED_CONFORMANCE_ACTIONS
