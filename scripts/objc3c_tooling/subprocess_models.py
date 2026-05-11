from __future__ import annotations

import subprocess
from dataclasses import dataclass, field
from typing import Mapping

from .subprocess_output import bounded_text


TIMEOUT_EXIT_CODE = 124
MISSING_EXECUTABLE_EXIT_CODE = 127
LAUNCH_ERROR_EXIT_CODE = 126
DEFAULT_SNIPPET_CHARS = 4000


@dataclass(frozen=True)
class CommandExecution:
    command: tuple[str, ...]
    cwd: str | None
    returncode: int
    stdout: str
    stderr: str
    duration_seconds: float
    timeout_seconds: float | None = None
    metadata: Mapping[str, object] = field(default_factory=dict)

    @property
    def duration_ms(self) -> float:
        return round(self.duration_seconds * 1000.0, 3)

    def completed_process(self) -> subprocess.CompletedProcess[str]:
        return subprocess.CompletedProcess(
            list(self.command),
            self.returncode,
            stdout=self.stdout,
            stderr=self.stderr,
        )

    def to_dict(self, *, include_output: bool = True, snippet_chars: int = DEFAULT_SNIPPET_CHARS) -> dict[str, object]:
        payload: dict[str, object] = {
            "command": list(self.command),
            "cwd": self.cwd,
            "returncode": self.returncode,
            "duration_ms": self.duration_ms,
        }
        if self.timeout_seconds is not None:
            payload["timeout_seconds"] = self.timeout_seconds
        if self.metadata:
            payload["metadata"] = dict(self.metadata)
        if include_output:
            payload["stdout"] = self.stdout
            payload["stderr"] = self.stderr
        else:
            payload["stdout_snippet"] = bounded_text(self.stdout, snippet_chars)
            payload["stderr_snippet"] = bounded_text(self.stderr, snippet_chars)
        return payload


__all__ = (
    "DEFAULT_SNIPPET_CHARS",
    "LAUNCH_ERROR_EXIT_CODE",
    "MISSING_EXECUTABLE_EXIT_CODE",
    "TIMEOUT_EXIT_CODE",
    "CommandExecution",
)
