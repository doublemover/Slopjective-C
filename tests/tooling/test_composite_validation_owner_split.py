from __future__ import annotations

import importlib
from pathlib import Path

from scripts.objc3c_workflow import composite_validation
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


def test_composite_report_failed_treats_missing_or_unreadable_payload_as_not_failed() -> None:
    assert composite_report_failed(ROOT / "tmp" / "missing-composite-report.json") is False
