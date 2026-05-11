"""GitHub command invocation primitives for issue extraction workflows."""

from __future__ import annotations

import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Protocol

GITHUB_TIMEOUT_EXIT_CODE = 124
GITHUB_MISSING_EXECUTABLE_EXIT_CODE = 127
GITHUB_LAUNCH_ERROR_EXIT_CODE = 126


@dataclass(frozen=True)
class GitHubCommand:
    args: tuple[str, ...]
    cwd: Path | None = None
    executable: str = "gh"
    timeout_seconds: float | None = None

    @property
    def command(self) -> tuple[str, ...]:
        return (self.executable, *self.args)


@dataclass(frozen=True)
class GitHubCommandResult:
    command: tuple[str, ...]
    cwd: str | None
    returncode: int
    stdout: str
    stderr: str
    timed_out: bool = False

    @property
    def ok(self) -> bool:
        return self.returncode == 0


class GitHubCommandError(RuntimeError):
    def __init__(self, result: GitHubCommandResult) -> None:
        self.result = result
        super().__init__(_format_command_error(result))


class GitHubCommandRunner(Protocol):
    def run(self, command: GitHubCommand) -> GitHubCommandResult:
        """Run a GitHub CLI command or raise GitHubCommandError."""


class SubprocessGitHubCommandRunner:
    def run(self, command: GitHubCommand) -> GitHubCommandResult:
        command_tuple = command.command
        if not command.executable:
            result = GitHubCommandResult(
                command=command_tuple,
                cwd=command.cwd.as_posix() if command.cwd is not None else None,
                returncode=GITHUB_LAUNCH_ERROR_EXIT_CODE,
                stdout="",
                stderr="command launch error: empty executable",
            )
            raise GitHubCommandError(result)

        try:
            completed = subprocess.run(
                list(command_tuple),
                cwd=command.cwd,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                check=False,
                timeout=command.timeout_seconds,
            )
            result = GitHubCommandResult(
                command=command_tuple,
                cwd=command.cwd.as_posix() if command.cwd is not None else None,
                returncode=int(completed.returncode),
                stdout=completed.stdout or "",
                stderr=completed.stderr or "",
            )
        except subprocess.TimeoutExpired as exc:
            result = GitHubCommandResult(
                command=command_tuple,
                cwd=command.cwd.as_posix() if command.cwd is not None else None,
                returncode=GITHUB_TIMEOUT_EXIT_CODE,
                stdout=_normalize_timeout_output(exc.stdout),
                stderr=_timeout_stderr(command_tuple, command.timeout_seconds, exc.stderr),
                timed_out=True,
            )
        except FileNotFoundError:
            result = GitHubCommandResult(
                command=command_tuple,
                cwd=command.cwd.as_posix() if command.cwd is not None else None,
                returncode=GITHUB_MISSING_EXECUTABLE_EXIT_CODE,
                stdout="",
                stderr=f"executable not found: {command.executable}",
            )
        except OSError as exc:
            result = GitHubCommandResult(
                command=command_tuple,
                cwd=command.cwd.as_posix() if command.cwd is not None else None,
                returncode=GITHUB_LAUNCH_ERROR_EXIT_CODE,
                stdout="",
                stderr=f"command launch error ({exc.__class__.__name__}): {command.executable}",
            )

        if not result.ok:
            raise GitHubCommandError(result)
        return result


def _normalize_timeout_output(value: bytes | str | None) -> str:
    if value is None:
        return ""
    if isinstance(value, bytes):
        return value.decode("utf-8", errors="replace")
    return value


def _timeout_stderr(command: tuple[str, ...], timeout_seconds: float | None, stderr: bytes | str | None) -> str:
    stderr_text = _normalize_timeout_output(stderr)
    timeout_note = f"command timed out after {timeout_seconds} seconds: {' '.join(command)}"
    if stderr_text:
        return f"{stderr_text}\n{timeout_note}"
    return timeout_note


def _format_command_error(result: GitHubCommandResult) -> str:
    command_text = " ".join(result.command)
    location = f" in {result.cwd}" if result.cwd is not None else ""
    parts = [f"GitHub command failed with exit code {result.returncode}{location}: {command_text}"]
    if result.stdout:
        parts.append(f"stdout:\n{result.stdout}")
    if result.stderr:
        parts.append(f"stderr:\n{result.stderr}")
    return "\n".join(parts)


__all__ = (
    "GITHUB_LAUNCH_ERROR_EXIT_CODE",
    "GITHUB_MISSING_EXECUTABLE_EXIT_CODE",
    "GITHUB_TIMEOUT_EXIT_CODE",
    "GitHubCommand",
    "GitHubCommandError",
    "GitHubCommandResult",
    "GitHubCommandRunner",
    "SubprocessGitHubCommandRunner",
)
