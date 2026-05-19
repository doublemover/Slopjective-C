"""Open-blocker audit command execution."""

from __future__ import annotations

from pathlib import Path

from scripts.open_blocker_extraction.audit_runner import run_command as run_timed_command

from .constants import ROOT
from .models import CommandResult, CommandSpec


def run_command(spec: CommandSpec, *, root: Path = ROOT) -> CommandResult:
    return run_timed_command(spec, root=root)


__all__ = ["run_command"]
