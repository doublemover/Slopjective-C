from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow.composite_step_nested import RUNNER_SCRIPT_PATH
from scripts.objc3c_workflow.composite_step_payload import composite_step_payload
from scripts.objc3c_workflow.composite_step_runtime_reuse import (
    runtime_acceptance_reuse_step,
)
from scripts.objc3c_workflow.composite_steps import (
    RUNNER_SCRIPT_PATH as PUBLIC_RUNNER_SCRIPT_PATH,
)
from scripts.objc3c_workflow.composite_steps import (
    runtime_acceptance_reuse_step as public_runtime_acceptance_reuse_step,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"

OWNER_MODULES = (
    "composite_step_runtime_reuse",
    "composite_step_nested",
    "composite_step_subprocess",
    "composite_step_payload",
)


def test_composite_steps_is_public_execution_facade() -> None:
    facade_text = (WORKFLOW_ROOT / "composite_steps.py").read_text(encoding="utf-8")

    for module_name in OWNER_MODULES:
        assert importlib.import_module(f"scripts.objc3c_workflow.{module_name}")
        assert f"from .{module_name} import" in facade_text
    assert "run_capture(" not in facade_text
    assert "OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN" not in facade_text


def test_composite_steps_preserves_nested_and_reuse_public_imports() -> None:
    assert PUBLIC_RUNNER_SCRIPT_PATH == RUNNER_SCRIPT_PATH
    assert public_runtime_acceptance_reuse_step is runtime_acceptance_reuse_step


def test_runtime_acceptance_reuse_step_shape(monkeypatch) -> None:
    monkeypatch.setenv("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN", "1")

    step = runtime_acceptance_reuse_step(
        "test-runtime-acceptance-fast",
        ["python", "acceptance.py"],
        0.0,
    )

    assert step is not None
    assert step["exit_code"] == 0
    assert step["report_reused"] is True
    assert step["report_paths"] == ["tmp/reports/runtime/acceptance/summary.json"]


def test_composite_step_payload_preserves_shared_shape() -> None:
    payload = composite_step_payload(
        action="lint",
        command=["python", "-m", "scripts.objc3c_workflow", "lint"],
        exit_code=0,
        report_paths=["tmp/reports/objc3c-public-workflow/lint.json"],
        started_at=0.0,
        extra={"executed_in_process": True},
    )

    assert payload["action"] == "lint"
    assert payload["command"] == ["python", "-m", "scripts.objc3c_workflow", "lint"]
    assert payload["exit_code"] == 0
    assert payload["report_paths"] == ["tmp/reports/objc3c-public-workflow/lint.json"]
    assert payload["executed_in_process"] is True
    assert isinstance(payload["duration_seconds"], float)
