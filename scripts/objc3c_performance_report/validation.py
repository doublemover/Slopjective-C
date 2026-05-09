from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel


def require_pass_status(payload: dict[str, Any], path: Path) -> None:
    if payload.get("status") != "PASS":
        raise RuntimeError(f"required report did not pass: {repo_rel(path)}")


def require_dashboard_upstream_reports(dashboard: dict[str, Any]) -> dict[str, Any]:
    upstream_reports = dashboard["upstream_reports"]
    if not isinstance(upstream_reports, dict):
        raise RuntimeError("performance dashboard upstream_reports must be an object")
    return upstream_reports
