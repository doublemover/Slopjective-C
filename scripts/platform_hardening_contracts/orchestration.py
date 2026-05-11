"""Host probing and report-writing orchestration for platform hardening."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
import platform
import shutil
import subprocess
import sys
from typing import Any, Sequence

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .models import HostSnapshot, ToolProbe
from .source_surface_catalog import ROOT, SUPPORT_MATRIX_ARTIFACT_PATH, SUPPORT_MATRIX_SUMMARY_PATH


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat().replace("+00:00", "Z")


def current_host() -> HostSnapshot:
    return HostSnapshot(
        os=platform.platform(),
        arch=platform.machine() or "",
        system=platform.system().lower(),
        machine=(platform.machine() or "").lower(),
    )


def run_probe(command: Sequence[str]) -> ToolProbe:
    result = subprocess.run(list(command), cwd=ROOT, text=True, capture_output=True, check=False)
    headline = ""
    for stream in (result.stdout, result.stderr):
        for line in stream.splitlines():
            stripped = line.strip()
            if stripped:
                headline = stripped
                break
        if headline:
            break
    return ToolProbe(tuple(command), result.returncode == 0, result.returncode, headline)


def missing_tool_probe(tool_name: str, command_name: str | None = None) -> ToolProbe:
    display_name = command_name or tool_name
    return ToolProbe((display_name, "--version"), False, 127, f"{display_name} not found")


def required_tool_probes() -> dict[str, dict[str, Any]]:
    pwsh_path = shutil.which("pwsh")
    clang_path = shutil.which("clang++") or shutil.which("clang")
    probes = {
        "python": ToolProbe((sys.executable, "--version"), True, 0, sys.version.splitlines()[0]),
        "pwsh": run_probe((pwsh_path, "--version")) if pwsh_path else missing_tool_probe("pwsh"),
        "clang": run_probe((clang_path, "--version")) if clang_path else missing_tool_probe("clang", "clang++"),
    }
    return {name: probe.as_json() for name, probe in probes.items()}


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(path, payload)


def write_markdown_summary(path: Path, title: str, rows: Sequence[tuple[str, Any]]) -> None:
    body = f"# {title}\n\n" + "".join(f"- {label}: `{value}`\n" for label, value in rows)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(body, encoding="utf-8")


def write_support_matrix(payload: dict[str, Any]) -> dict[str, Any]:
    write_json(SUPPORT_MATRIX_ARTIFACT_PATH, payload)
    summary = {
        "contract_id": "objc3c.platform.hardening.support.matrix.summary.v1",
        "status": "PASS",
        "artifact_path": repo_rel(SUPPORT_MATRIX_ARTIFACT_PATH),
        "default_platform_id": payload["default_platform_id"],
        "platform_count": payload["platform_count"],
        "channel_count": len(payload["channels"]),
        "tier_count": len(payload["tiers"]),
    }
    write_json(SUPPORT_MATRIX_SUMMARY_PATH, summary)
    return summary


__all__ = [
    "current_host",
    "missing_tool_probe",
    "required_tool_probes",
    "run_probe",
    "utc_now",
    "write_json",
    "write_markdown_summary",
    "write_support_matrix",
]
