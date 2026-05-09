from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .diagnostics import format_finding

REPORT_WRITER_CONTRACT_ID = "source-hygiene-report-writer-v1"
REPORT_WRITER_OWNER_SURFACE = "scripts/source_hygiene/report_writer.py"
REPORT_SUMMARY_FIELDS: tuple[str, ...] = (
    "schema_version",
    "ok",
    "active_findings",
    "tracked_generated_reports",
    "generated_truth_boundary_findings",
)


def report_writer_contract_payload() -> dict[str, object]:
    return {
        "contract_id": REPORT_WRITER_CONTRACT_ID,
        "owner_surface": REPORT_WRITER_OWNER_SURFACE,
        "summary_fields": list(REPORT_SUMMARY_FIELDS),
        "json_output_is_sorted": True,
        "text_output_lists_scan_findings": True,
        "text_output_lists_generated_boundary_findings": True,
    }


def write_reports(report: dict[str, Any], json_path: Path, text_path: Path) -> None:
    json_path.parent.mkdir(parents=True, exist_ok=True)
    text_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    stats = report["stats"]
    summary_values = {
        "schema_version": report["schema_version"],
        "ok": str(report["ok"]).lower(),
        "active_findings": stats["active_finding_count"],
        "tracked_generated_reports": stats["tracked_generated_report_count"],
        "generated_truth_boundary_findings": stats[
            "generated_truth_boundary_finding_count"
        ],
    }
    lines = [f"{field}: {summary_values[field]}" for field in REPORT_SUMMARY_FIELDS]
    for finding in report["active_findings"][:100]:
        lines.append(format_finding(finding))
    for finding in report["generated_truth_boundary_findings"][:100]:
        lines.append(
            f"{finding['path']}: generated-truth-boundary: {finding['description']}"
        )
    text_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


__all__ = [
    "REPORT_SUMMARY_FIELDS",
    "REPORT_WRITER_CONTRACT_ID",
    "REPORT_WRITER_OWNER_SURFACE",
    "report_writer_contract_payload",
    "write_reports",
]
