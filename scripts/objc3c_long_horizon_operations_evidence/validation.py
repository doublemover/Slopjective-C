from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel


def status_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def record_failure(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def require_report_file(path: Path, report_name: str, failures: list[str], *, root: Path) -> None:
    record_failure(path.is_file(), f"missing {report_name} report {repo_rel(path, root=root)}", failures)


def require_passing_report(report_name: str, payload: dict[str, Any], failures: list[str]) -> None:
    record_failure(status_passes(payload), f"{report_name} report did not pass", failures)


def require_support_artifact(
    path: Path,
    payload: dict[str, Any],
    artifact_name: str,
    failures: list[str],
    *,
    root: Path,
) -> None:
    record_failure(bool(payload), f"missing {artifact_name} {repo_rel(path, root=root)}", failures)
