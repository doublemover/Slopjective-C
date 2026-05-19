from runtime_runnable_action_catalogs_support import (
    EXPECTED_CONFORMANCE_ACTIONS,
    EXPECTED_E2E_ACTIONS,
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
    RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
    RUNTIME_RUNNABLE_E2E_ACTION_SPECS,
)


def runtime_runnable_catalogs_and_handlers_derive_from_action_groups() -> None:
    assert list(RUNTIME_RUNNABLE_E2E_ACTION_SPECS) == EXPECTED_E2E_ACTIONS
    assert list(RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS) == EXPECTED_E2E_ACTIONS
    assert list(RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS) == (
        EXPECTED_CONFORMANCE_ACTIONS
    )
    assert list(RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS) == (
        EXPECTED_CONFORMANCE_ACTIONS
    )

    for action, spec in RUNTIME_RUNNABLE_E2E_ACTION_SPECS.items():
        assert spec.action == action
        assert spec.validation_tier == "full"
        assert spec.backend.startswith("python:scripts/check_objc3c_runnable_")
        assert RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS[action]

    for action, spec in RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS.items():
        assert spec.action == action
        assert spec.validation_tier == "full"
        assert spec.backend.startswith("python:scripts/check_objc3c_runnable_")
        assert RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS[action]
