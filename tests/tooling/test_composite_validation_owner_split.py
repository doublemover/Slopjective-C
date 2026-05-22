from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow import composite_validation
from scripts.objc3c_workflow.actions.validation_timing_budgets import (
    composite_elapsed_threshold_seconds,
)
from scripts.objc3c_workflow.composite_report_finalization import (
    composite_report_failed,
)

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"


def test_composite_validation_delegates_report_finalization() -> None:
    module = importlib.import_module("scripts.objc3c_workflow.composite_report_finalization")
    source = (WORKFLOW_ROOT / "composite_validation.py").read_text(encoding="utf-8")

    assert module
    assert "from .composite_report_finalization import" in source
    assert "write_composite_validation_report(" not in source
    assert "public-workflow-report:" not in source
    assert "load_latest_report_payload(" not in source


def test_composite_validation_preserves_public_entrypoint() -> None:
    assert callable(composite_validation.run_composite_validation)


def test_composite_validation_continue_mode_runs_later_steps_after_failure(monkeypatch) -> None:
    executed: list[str] = []
    reports: list[tuple[str, list[str], str]] = []

    def fake_run_composite_step(action: str, command: object) -> dict[str, object]:
        del command
        executed.append(action)
        return {
            "action": action,
            "exit_code": 7 if action == "first" else 0,
            "duration_seconds": 0.0,
            "report_paths": [],
        }

    def fake_write_and_announce(
        action: str,
        results: list[dict[str, object]],
        *,
        status: str,
    ) -> Path:
        reports.append((action, [str(result["action"]) for result in results], status))
        return ROOT / "tmp" / "fake-composite-report.json"

    monkeypatch.setattr(
        composite_validation,
        "run_composite_step",
        fake_run_composite_step,
    )
    monkeypatch.setattr(
        composite_validation,
        "write_and_announce_composite_report",
        fake_write_and_announce,
    )
    monkeypatch.setattr(
        composite_validation,
        "print_composite_step_start",
        lambda **_: None,
    )
    monkeypatch.setattr(
        composite_validation,
        "print_composite_step_done",
        lambda **_: None,
    )

    exit_code = composite_validation.run_composite_validation(
        "validate-stress",
        [("first", ()), ("second", ())],
        continue_on_failure=True,
    )

    assert exit_code == 7
    assert executed == ["first", "second"]
    assert reports == [("validate-stress", ["first", "second"], "FAIL")]


def test_composite_report_failed_treats_missing_or_unreadable_payload_as_not_failed() -> None:
    assert composite_report_failed(ROOT / "tmp" / "missing-composite-report.json") is False


def test_validate_stress_budget_covers_cold_native_build_lane() -> None:
    assert composite_elapsed_threshold_seconds("validate-stress") == 900.0


def test_public_full_budget_covers_cold_developer_validation_lane() -> None:
    assert composite_elapsed_threshold_seconds("test-full") == 180.0
