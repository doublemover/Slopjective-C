from __future__ import annotations

from contextlib import contextmanager, redirect_stderr, redirect_stdout
from io import StringIO
import os
import subprocess
import sys
import time
from pathlib import Path
from typing import Mapping, Sequence

from .subprocess_environment import python_child_environment
from .subprocess_models import (
    LAUNCH_ERROR_EXIT_CODE,
    MISSING_EXECUTABLE_EXIT_CODE,
    TIMEOUT_EXIT_CODE,
    CommandExecution,
)
from .subprocess_output import echo_output, normalize_output
from .paths import ROOT, display_path


def command_text(command: Sequence[object]) -> str:
    return " ".join(str(part) for part in command)


def python_script_command(script_path: Path | str, *args: object) -> list[str]:
    return [sys.executable, str(script_path), *(str(arg) for arg in args)]


def python_script_command_tuple(script_path: Path | str, *args: object) -> tuple[str, ...]:
    return tuple(python_script_command(script_path, *args))


def _completed_from_launch_error(command: Sequence[object], returncode: int, stderr: str) -> subprocess.CompletedProcess[str]:
    return subprocess.CompletedProcess([str(part) for part in command], returncode, stdout="", stderr=stderr)


def _public_workflow_action_offset(command_list: Sequence[str]) -> int | None:
    if (
        len(command_list) >= 5
        and command_list[0] == "npm"
        and command_list[1] == "run"
        and command_list[3] == "--"
    ):
        return 4
    return None


def _portable_public_workflow_command(
    command_list: list[str],
    *,
    cwd: Path | str | None,
) -> list[str]:
    if cwd is None:
        return command_list
    try:
        if Path(cwd).resolve() == ROOT.resolve():
            return command_list
    except OSError:
        return command_list

    action_offset = _public_workflow_action_offset(command_list)
    if action_offset is None:
        return command_list
    return [sys.executable, "-m", "scripts.objc3c_workflow", *command_list[action_offset:]]


def _completed_from_public_workflow_action(
    command_list: list[str],
    *,
    cwd: Path | str | None,
    capture_output: bool,
    env_overlay: Mapping[str, str] | None = None,
) -> subprocess.CompletedProcess[str] | None:
    if cwd is not None:
        try:
            if Path(cwd).resolve() != ROOT.resolve():
                return None
        except OSError:
            return None

    root_text = str(ROOT)
    if root_text not in sys.path:
        sys.path.insert(0, root_text)

    try:
        from scripts.objc3c_workflow.composite_step_nested import (
            execute_nested_action,
            npm_bridge_action_offset,
        )
    except ModuleNotFoundError as exc:
        if exc.name != "scripts" and not str(exc.name).startswith("scripts."):
            raise
        from objc3c_workflow.composite_step_nested import (
            execute_nested_action,
            npm_bridge_action_offset,
        )

    action_offset = npm_bridge_action_offset(command_list)
    if action_offset is None:
        return None

    action = command_list[action_offset]
    rest = command_list[action_offset + 1 :]

    @contextmanager
    def patched_environment() -> object:
        overlay = dict(env_overlay or {})
        previous: dict[str, str | None] = {
            key: os.environ.get(key)
            for key in overlay
        }
        try:
            os.environ.update(overlay)
            yield
        finally:
            for key, value in previous.items():
                if value is None:
                    os.environ.pop(key, None)
                else:
                    os.environ[key] = value

    if not capture_output:
        with patched_environment():
            returncode = execute_nested_action(action, rest)
        return subprocess.CompletedProcess(
            command_list,
            returncode,
            stdout="",
            stderr="",
        )

    stdout = StringIO()
    stderr = StringIO()
    with patched_environment(), redirect_stdout(stdout), redirect_stderr(stderr):
        returncode = execute_nested_action(action, rest)
    return subprocess.CompletedProcess(
        command_list,
        returncode,
        stdout=stdout.getvalue(),
        stderr=stderr.getvalue(),
    )


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
        result = _completed_from_public_workflow_action(
            command_list,
            cwd=cwd,
            capture_output=capture_output,
            env_overlay=merged_overlay,
        )
        if result is None:
            command_list = _portable_public_workflow_command(command_list, cwd=cwd)
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
