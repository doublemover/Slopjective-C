from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from .diagnostics import format_finding


def write_reports(report: dict[str, Any], json_path: Path, text_path: Path) -> None:
    json_path.parent.mkdir(parents=True, exist_ok=True)
    text_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    lines = [
        f"schema_version: {report['schema_version']}",
        f"ok: {str(report['ok']).lower()}",
        f"active_findings: {report['stats']['active_finding_count']}",
        f"tracked_generated_reports: {report['stats']['tracked_generated_report_count']}",
    ]
    for finding in report["active_findings"][:100]:
        lines.append(format_finding(finding))
    text_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
