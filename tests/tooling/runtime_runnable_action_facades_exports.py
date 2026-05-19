from runtime_runnable_action_facades_support import (
    OWNER_EXPORTS,
    runtime_runnable_conformance,
    runtime_runnable_e2e,
    workflow_action_module,
)


def runtime_runnable_modules_expose_stable_exports() -> None:
    for module_name, expected_exports in OWNER_EXPORTS.items():
        module = workflow_action_module(module_name)
        assert expected_exports <= set(module.__all__)

    assert "RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS" in (
        runtime_runnable_conformance.__all__
    )
    assert "RUNTIME_RUNNABLE_E2E_ACTION_GROUPS" in runtime_runnable_e2e.__all__
