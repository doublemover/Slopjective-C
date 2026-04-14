from __future__ import annotations

import os
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Mapping, Sequence

from objc3c_tooling.paths import ROOT, display_path

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


def command_text(command: Sequence[object]) -> str:
    return " ".join(str(part) for part in command)


def bounded_text(value: str, limit: int = DEFAULT_SNIPPET_CHARS) -> str:
    if limit <= 0 or len(value) <= limit:
        return value
    omitted = len(value) - limit
    return f"{value[:limit]}\n... truncated {omitted} character(s) ..."


def environment_with_overlay(env_overlay: Mapping[str, str] | None = None) -> dict[str, str] | None:
    if env_overlay is None:
        return None
    env = os.environ.copy()
    env.update(env_overlay)
    return env


def python_child_environment(
    env_overlay: Mapping[str, str] | None = None,
    *,
    python_dont_write_bytecode: bool = True,
) -> dict[str, str] | None:
    overlay = dict(env_overlay or {})
    if python_dont_write_bytecode:
        overlay.setdefault("PYTHONDONTWRITEBYTECODE", "1")
    return environment_with_overlay(overlay) if overlay else None


def normalize_output(value: str | bytes | None) -> str:
    if value is None:
        return ""
    if isinstance(value, bytes):
        return value.decode("utf-8", errors="replace")
    return value


def echo_output(stdout: str, stderr: str) -> None:
    if stdout:
        sys.stdout.write(stdout)
    if stderr:
        sys.stderr.write(stderr)


def _completed_from_launch_error(command: Sequence[object], returncode: int, stderr: str) -> subprocess.CompletedProcess[str]:
    return subprocess.CompletedProcess([str(part) for part in command], returncode, stdout="", stderr=stderr)


def run_completed(
    command: Sequence[object],
    *,
    cwd: Path | str | None = ROOT,
    env_overlay: Mapping[str, str] | None = None,
    env: Mapping[str, str] | None = None,
    timeout: float | None = None,
    capture_output: bool = True,
    echo: bool = False,
    check: bool = False,
    encoding: str = "utf-8",
    errors: str = "replace",
    python_dont_write_bytecode: bool = True,
) -> subprocess.CompletedProcess[str]:
    command_list = [str(part) for part in command]
    if not command_list:
        return _completed_from_launch_error(command_list, LAUNCH_ERROR_EXIT_CODE, "command launch error: empty command")
    merged_overlay = dict(env_overlay or {})
    if env:
        merged_overlay.update(env)
    try:
        result = subprocess.run(
            command_list,
            cwd=cwd,
            env=python_child_environment(
                merged_overlay,
                python_dont_write_bytecode=python_dont_write_bytecode,
            ),
            capture_output=capture_output,
            text=True,
            encoding=encoding,
            errors=errors,
            check=False,
            timeout=timeout,
        )
    except subprocess.TimeoutExpired as exc:
        stdout = normalize_output(exc.stdout)
        timeout_note = f"command timed out after {timeout} seconds: {command_text(command_list)}"
        stderr = normalize_output(exc.stderr)
        stderr = f"{stderr}\n{timeout_note}" if stderr else timeout_note
        result = subprocess.CompletedProcess(command_list, TIMEOUT_EXIT_CODE, stdout=stdout, stderr=stderr)
    except FileNotFoundError:
        result = _completed_from_launch_error(
            command_list,
            MISSING_EXECUTABLE_EXIT_CODE,
            f"executable not found: {command_list[0] if command_list else ''}",
        )
    except OSError as exc:
        result = _completed_from_launch_error(
            command_list,
            LAUNCH_ERROR_EXIT_CODE,
            f"command launch error ({exc.__class__.__name__}): {command_list[0] if command_list else ''}",
        )
    if echo and capture_output:
        echo_output(result.stdout or "", result.stderr or "")
    if check:
        result.check_returncode()
    return result


def run_capture(
    command: Sequence[object],
    *,
    cwd: Path | str | None = ROOT,
    env_overlay: Mapping[str, str] | None = None,
    env: Mapping[str, str] | None = None,
    timeout: float | None = None,
    capture_output: bool = True,
    echo: bool = True,
) -> subprocess.CompletedProcess[str]:
    return run_completed(
        command,
        cwd=cwd,
        env_overlay=env_overlay,
        env=env,
        timeout=timeout,
        capture_output=capture_output,
        echo=echo,
        check=False,
    )


def run_timed(
    command: Sequence[object],
    *,
    cwd: Path | str | None = ROOT,
    env_overlay: Mapping[str, str] | None = None,
    env: Mapping[str, str] | None = None,
    timeout: float | None = None,
    echo: bool = False,
    metadata: Mapping[str, object] | None = None,
) -> CommandExecution:
    started = time.perf_counter()
    result = run_completed(
        command,
        cwd=cwd,
        env_overlay=env_overlay,
        env=env,
        timeout=timeout,
        capture_output=True,
        echo=echo,
        check=False,
    )
    duration = time.perf_counter() - started
    return CommandExecution(
        command=tuple(str(part) for part in command),
        cwd=display_path(Path(cwd)) if cwd is not None else None,
        returncode=int(result.returncode),
        stdout=result.stdout or "",
        stderr=result.stderr or "",
        duration_seconds=duration,
        timeout_seconds=timeout if result.returncode == TIMEOUT_EXIT_CODE else None,
        metadata=dict(metadata or {}),
    )


def failure_snippet(result: subprocess.CompletedProcess[str] | CommandExecution, *, limit: int = DEFAULT_SNIPPET_CHARS) -> str:
    stdout = result.stdout or ""
    stderr = result.stderr or ""
    parts: list[str] = []
    if stdout:
        parts.append(f"stdout:\n{bounded_text(stdout, limit)}")
    if stderr:
        parts.append(f"stderr:\n{bounded_text(stderr, limit)}")
    return "\n".join(parts)


def raise_if_failed(
    result: subprocess.CompletedProcess[str] | CommandExecution,
    *,
    context: str,
    limit: int = DEFAULT_SNIPPET_CHARS,
) -> None:
    returncode = int(result.returncode)
    if returncode == 0:
        return
    snippet = failure_snippet(result, limit=limit)
    detail = f"\n{snippet}" if snippet else ""
    raise RuntimeError(f"{context} failed with exit code {returncode}{detail}")
