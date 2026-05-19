"""Command execution helpers for release/runtime claim matrix publication."""

from __future__ import annotations

import subprocess
from typing import Sequence

from objc3c_tooling.subprocesses import run_completed

from .paths import ROOT


def run(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    return run_completed(command, cwd=ROOT)


def ensure_success(result: subprocess.CompletedProcess[str], context: str) -> None:
    if result.returncode == 0:
        return
    detail = (result.stderr or result.stdout).strip()
    raise SystemExit(f"{context} failed ({result.returncode}): {detail}")
