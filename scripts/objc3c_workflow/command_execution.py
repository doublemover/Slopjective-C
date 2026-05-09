"""Subprocess execution owner for objc3c workflow actions."""

from __future__ import annotations

from collections.abc import Sequence

from objc3c_tooling.subprocesses import run_completed

from .environment import ROOT


def run(command: Sequence[str]) -> int:
    return run_completed(command, cwd=ROOT, capture_output=False).returncode


__all__ = ["run"]
