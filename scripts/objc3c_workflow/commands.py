"""Subprocess helpers for objc3c workflow actions."""

from __future__ import annotations

import sys
from collections.abc import Sequence
from pathlib import Path

from objc3c_tooling.subprocesses import run_completed

from .environment import PWSH, ROOT, WORKFLOW_MODULE


def run(command: Sequence[str]) -> int:
    return run_completed(command, cwd=ROOT, capture_output=False).returncode


def workflow_command(action: str, *args: str) -> list[str]:
    return [sys.executable, "-m", WORKFLOW_MODULE, action, *args]


def extract_output_line(stdout: str, prefix: str) -> str:
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return ""


def pwsh_file(script: Path, *args: str) -> int:
    return run([PWSH, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", str(script), *args])
