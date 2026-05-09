from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.actions import runtime_runnable_conformance
from scripts.objc3c_workflow.actions import runtime_runnable_e2e

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"

OWNER_MODULES = (
    "runtime_runnable_block_arc",
    "runtime_runnable_bootstrap",
    "runtime_runnable_concurrency",
    "runtime_runnable_error",
    "runtime_runnable_interop",
    "runtime_runnable_metaprogramming",
    "runtime_runnable_object_model",
    "runtime_runnable_release_candidate",
    "runtime_runnable_storage_reflection",
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
