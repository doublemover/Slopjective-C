from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Callable

from objc3c_library_cli_parity_fixtures import write_source_mode_artifacts

FakeRun = Callable[[list[str]], subprocess.CompletedProcess[str]]


def fake_successful_source_mode_run(marker: str) -> FakeRun:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        out_dir = Path(command[command.index("--out-dir") + 1])
        emit_prefix = command[command.index("--emit-prefix") + 1]
        write_source_mode_artifacts(out_dir, emit_prefix=emit_prefix, marker=marker)
        return subprocess.CompletedProcess(command, 0, stdout="ok\n", stderr="")

    return fake_run


def fake_observed_backend_run(
    marker: str,
    observed_commands: list[list[str]],
) -> FakeRun:
    def fake_run(command: list[str], **_: object) -> subprocess.CompletedProcess[str]:
        observed_commands.append(command)
        out_dir = Path(command[command.index("--out-dir") + 1])
        emit_prefix = command[command.index("--emit-prefix") + 1]
        write_source_mode_artifacts(out_dir, emit_prefix=emit_prefix, marker=marker)
        return subprocess.CompletedProcess(command, 0, stdout="ok\n", stderr="")

    return fake_run


def never_run_on_fail_closed(
    _: list[str],
    **__: object,
) -> subprocess.CompletedProcess[str]:
    raise AssertionError("commands must not execute on capability fail-closed preflight")


def fake_command_failure_run(
    command: list[str],
    **_: object,
) -> subprocess.CompletedProcess[str]:
    out_dir = Path(command[command.index("--out-dir") + 1])
    emit_prefix = command[command.index("--emit-prefix") + 1]
    write_source_mode_artifacts(out_dir, emit_prefix=emit_prefix, marker="cmd-fail")
    role = "cli" if "objc3c-native" in command[0] else "c-api"
    exit_code = 17 if role == "cli" else 23
    return subprocess.CompletedProcess(
        command,
        exit_code,
        stdout=f"{role}-stdout\n",
        stderr=f"{role}-stderr\n",
    )
