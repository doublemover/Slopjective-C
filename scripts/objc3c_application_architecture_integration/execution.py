"""Child execution and summary loading for application architecture validation."""

from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import run_capture


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def summary_passes(payload: dict[str, Any]) -> bool:
    return payload.get("status") in {"PASS", "OK"} or payload.get("ok") is True


def require_executed_summary(
    *,
    name: str,
    summary_path: Path,
    failures: list[str],
    command: list[str],
) -> dict[str, Any]:
    step_result: dict[str, Any] = {
        "name": name,
        "command": command,
        "summary_path": repo_rel(summary_path),
        "mode": "executed",
    }
    result = run_capture(command)
    step_result["exit_code"] = result.returncode
    expect(result.returncode == 0, f"{name} failed", failures)

    if not summary_path.is_file():
        failures.append(f"{name} missing expected summary {repo_rel(summary_path)}")
        step_result["summary_ok"] = False
        return step_result

    summary = load_json(summary_path)
    step_result["summary_ok"] = summary_passes(summary)
    step_result["summary_status"] = summary.get("status", summary.get("ok"))
    if not step_result["summary_ok"]:
        failures.append(f"{name} summary did not report PASS")
    return summary
