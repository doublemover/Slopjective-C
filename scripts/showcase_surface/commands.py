from __future__ import annotations

from objc3c_tooling.subprocesses import run_completed

from .paths import ROOT


def run(command: list[str]) -> int:
    return run_completed(command, cwd=ROOT, capture_output=False).returncode
