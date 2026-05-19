from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import render_json
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from objc3c_performance_dashboard.paths import OUTPUT_PATH


def render_dashboard_summary_json(payload: dict[str, Any]) -> str:
    return render_json(payload)


def render_dashboard_summary_markdown(payload: dict[str, Any]) -> str:
    lines = [
        "# Objective-C 3 Performance Governance Dashboard",
        "",
        f"- Release status: `{payload.get('release_status', '')}`",
        f"- Claim ready: `{str(payload.get('claim_ready', False)).lower()}`",
        f"- Blocking breaches: `{payload.get('blocking_breach_count', 0)}`",
        f"- Warning or caution breaches: `{payload.get('warning_breach_count', 0)}`",
        "",
        "## Upstream Reports",
        "",
    ]
    upstream_reports = payload.get("upstream_reports", {})
    if isinstance(upstream_reports, dict):
        lines.extend(f"- `{name}`: `{path}`" for name, path in sorted(upstream_reports.items()))
    lines.extend(["", "## Failures", ""])
    failures = payload.get("failures", [])
    if isinstance(failures, list) and failures:
        lines.extend(f"- {failure}" for failure in failures)
    else:
        lines.append("- none")
    return "\n".join(lines) + "\n"


def publish_dashboard_summary(
    payload: dict[str, Any],
    *,
    output_path: Path = OUTPUT_PATH,
) -> str:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(output_path, payload)
    return repo_rel(output_path)
