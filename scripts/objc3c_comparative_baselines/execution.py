"""Command expansion and timed process execution."""

from __future__ import annotations

import time
from dataclasses import dataclass
from typing import Any, Sequence

from objc3c_tooling.subprocesses import run_capture


@dataclass(frozen=True)
class TimedStep:
    command: list[str]
    exit_code: int
    duration_ms: float
    stdout: str
    stderr: str

    def as_dict(self) -> dict[str, Any]:
        return {
            "command": self.command,
            "exit_code": self.exit_code,
            "duration_ms": self.duration_ms,
            "stdout": self.stdout,
            "stderr": self.stderr,
        }


def run_timed_step(command: Sequence[str]) -> dict[str, Any]:
    started = time.perf_counter()
    completed = run_capture(command)
    duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
    return TimedStep(
        command=[str(token) for token in command],
        exit_code=completed.returncode,
        duration_ms=duration_ms,
        stdout=completed.stdout,
        stderr=completed.stderr,
    ).as_dict()


def expand_command(
    *,
    tool: str,
    args: Sequence[str],
    source: str,
    output_exe: str,
) -> list[str]:
    expanded = [tool.format(source=source, output_exe=output_exe)]
    expanded.extend(argument.format(source=source, output_exe=output_exe) for argument in args)
    return expanded
