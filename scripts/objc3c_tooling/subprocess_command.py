from __future__ import annotations

import subprocess
import sys
import time
from pathlib import Path
from typing import Mapping, Sequence

from objc3c_tooling.paths import ROOT, display_path

from .subprocess_environment import python_child_environment
from .subprocess_models import (
    LAUNCH_ERROR_EXIT_CODE,
    MISSING_EXECUTABLE_EXIT_CODE,
    TIMEOUT_EXIT_CODE,
    CommandExecution,
)
from .subprocess_output import echo_output, normalize_output


def command_text(command: Sequence[object]) -> str:
    return " ".join(str(part) for part in command)


def python_script_command(script_path: Path | str, *args: object) -> list[str]:
    return [sys.executable, str(script_path), *(str(arg) for arg in args)]


def python_script_command_tuple(script_path: Path | str, *args: object) -> tuple[str, ...]:
    return tuple(python_script_command(script_path, *args))


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


__all__ = (
    "command_text",
    "python_script_command",
    "python_script_command_tuple",
    "run_capture",
    "run_completed",
    "run_timed",
)
