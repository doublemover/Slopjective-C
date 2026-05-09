"""Composite workflow report finalization helpers."""

from __future__ import annotations

from pathlib import Path

from .actions.validation_timing import load_latest_report_payload
from .composite_reports import write_composite_validation_report
from .environment import ROOT


def announce_composite_report(report_path: Path) -> None:
    print(f"public-workflow-report: {report_path.relative_to(ROOT).as_posix()}")


def write_and_announce_composite_report(
    action: str,
    results: list[dict[str, object]],
    *,
    status: str,
) -> Path:
    report_path = write_composite_validation_report(action, results, status=status)
    announce_composite_report(report_path)
    return report_path


def composite_report_failed(report_path: Path) -> bool:
    report_payload = load_latest_report_payload(report_path)
    return isinstance(report_payload, dict) and report_payload.get("status") != "PASS"
