"""Host and tool probe helpers for platform-hardening contracts."""

from __future__ import annotations

import platform
import shutil
import subprocess
import sys
from typing import Any, Sequence

from objc3c_tooling.llvm_discovery import find_llvm_tool_path

from .constants import ROOT, SUPPORTED_HOST_ARCH_ALIASES
from .models import HostSnapshot, ToolProbe


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
    clang_tool = find_llvm_tool_path("clang++") or find_llvm_tool_path("clang")
    clang_path = str(clang_tool) if clang_tool else None
    probes = {
        "python": ToolProbe((sys.executable, "--version"), True, 0, sys.version.splitlines()[0]),
        "pwsh": run_probe((pwsh_path, "--version")) if pwsh_path else missing_tool_probe("pwsh"),
        "clang": run_probe((clang_path, "--version")) if clang_path else missing_tool_probe("clang", "clang++"),
    }
    return {name: probe.as_json() for name, probe in probes.items()}


def host_matches_supported_platform(default_platform_id: str, host: HostSnapshot | None = None) -> bool:
    snapshot = host or current_host()
    if default_platform_id != "windows-x64":
        return False
    return snapshot.system == "windows" and snapshot.machine in SUPPORTED_HOST_ARCH_ALIASES["windows-x64"]


__all__ = [
    "current_host",
    "host_matches_supported_platform",
    "missing_tool_probe",
    "required_tool_probes",
    "run_probe",
]
