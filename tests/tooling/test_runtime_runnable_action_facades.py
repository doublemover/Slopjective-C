from __future__ import annotations

import importlib

from runtime_runnable_action_support import (
    ACTION_ROOT,
    OWNER_EXPORTS,
    OWNER_MODULES,
    runtime_runnable_conformance,
    runtime_runnable_e2e,
)


def test_runtime_runnable_facades_delegate_to_domain_owners() -> None:
    for facade_name in ("runtime_runnable_conformance", "runtime_runnable_e2e"):
        facade_text = (ACTION_ROOT / f"{facade_name}.py").read_text(encoding="utf-8")

        for module_name in OWNER_MODULES:
            assert importlib.import_module(
                f"scripts.objc3c_workflow.actions.{module_name}"
            )
            if module_name == "runtime_runnable_bootstrap":
                expected = facade_name == "runtime_runnable_e2e"
            else:
                expected = True
            assert (f"from .{module_name} import" in facade_text) == expected
        assert "from ..environment import ROOT" not in facade_text
        assert "run_python_check" not in facade_text
        assert "def " not in facade_text


def test_runtime_runnable_facades_preserve_public_actions() -> None:
    assert (
        runtime_runnable_conformance.action_validate_block_arc_conformance
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_block_arc"
        ).action_validate_block_arc_conformance
    )
    assert (
        runtime_runnable_conformance.action_validate_error_conformance
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_error"
        ).action_validate_error_conformance
    )
    assert (
        runtime_runnable_e2e.action_validate_runnable_interop
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_interop"
        ).action_validate_runnable_interop
    )
    assert (
        runtime_runnable_e2e.action_validate_runnable_release_candidate
        is importlib.import_module(
            "scripts.objc3c_workflow.actions.runtime_runnable_release_candidate"
        ).action_validate_runnable_release_candidate
    )


def test_runtime_runnable_modules_expose_stable_exports() -> None:
    for module_name, expected_exports in OWNER_EXPORTS.items():
        module = importlib.import_module(
            f"scripts.objc3c_workflow.actions.{module_name}"
        )
        assert expected_exports <= set(module.__all__)

    assert "RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS" in (
        runtime_runnable_conformance.__all__
    )
    assert "RUNTIME_RUNNABLE_E2E_ACTION_GROUPS" in runtime_runnable_e2e.__all__
