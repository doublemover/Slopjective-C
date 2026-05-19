"""Summary payload construction and report writing."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path

from objc3c_tooling.behavior_fixtures import BehaviorFixture, REQUIRED_TREE
from objc3c_tooling.json_io import write_json_file

from .config import CONTRACT_ID


def build_summary_payload(
    *,
    results: list[dict[str, object]],
    failures: list[dict[str, object]],
    selected: list[BehaviorFixture],
) -> dict[str, object]:
    return {
        "contract_id": CONTRACT_ID,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "fixture_count": len(selected),
        "passed": len(results),
        "failed": len(failures),
        "phase_coverage": sorted({result["owner_phase"] for result in results}),
        "required_phase_coverage": sorted(REQUIRED_TREE),
        "results": results,
        "failures": failures,
    }


def write_report(report_path: Path, payload: dict[str, object]) -> None:
    write_json_file(report_path, payload)


def render_report(report_path: Path, payload: dict[str, object]) -> None:
    write_report(report_path, payload)
