from runtime_runnable_action_facades_support import (
    FACADE_NAMES,
    OWNER_MODULES,
    facade_requires_owner_module,
    facade_text,
    workflow_action_module,
)


def runtime_runnable_facades_delegate_to_domain_owners() -> None:
    for facade_name in FACADE_NAMES:
        text = facade_text(facade_name)

        for module_name in OWNER_MODULES:
            assert workflow_action_module(module_name)
            expected = facade_requires_owner_module(facade_name, module_name)
            assert (f"from .{module_name} import" in text) == expected
        assert "from ..environment import ROOT" not in text
        assert "run_python_check" not in text
        assert "def " not in text
