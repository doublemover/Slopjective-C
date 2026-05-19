"""Open-blocker audit runner data models."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class CommandSpec:
    name: str
    script_path: Path
    actual_args: tuple[str, ...]
    display_args: tuple[str, ...]


@dataclass(frozen=True)
class CommandResult:
    spec: CommandSpec
    exit_code: int
    stdout: str
    stderr: str
