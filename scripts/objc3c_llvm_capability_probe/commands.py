"""Subprocess-backed LLVM capability probes."""

from __future__ import annotations

import subprocess
from pathlib import Path

from objc3c_tooling.subprocesses import run_timed

from .models import ExecutableProbe
from .parsing import first_non_empty_line, llc_help_mentions_filetype_obj


def run_command(command: list[str]) -> tuple[subprocess.CompletedProcess[str], float]:
    execution = run_timed(command, cwd=None)
    return execution.completed_process(), execution.duration_ms


def probe_executable(path: Path, *, role: str) -> dict[str, object]:
    version_cmd = [str(path), "--version"]
    version_result, version_duration_ms = run_command(version_cmd)
    version_text = (version_result.stdout or "") + (version_result.stderr or "")

    found = version_result.returncode != 127
    diagnostic = f"{role} executable not found: {path}" if not found else None
    return ExecutableProbe(
        role=role,
        path=str(path),
        found=found,
        version_exit_code=version_result.returncode,
        version_duration_ms=version_duration_ms,
        version_headline=first_non_empty_line(version_text),
        diagnostic=diagnostic,
    ).as_payload()


def probe_llc_filetype_obj(path: Path) -> dict[str, object]:
    help_cmd = [str(path), "--help"]
    help_result, help_duration_ms = run_command(help_cmd)
    help_text = (help_result.stdout or "") + (help_result.stderr or "")

    mentions_filetype, mentions_obj = llc_help_mentions_filetype_obj(help_text)
    supports_from_help = mentions_filetype and mentions_obj

    version_with_filetype_cmd = [str(path), "--filetype=obj", "--version"]
    filetype_version_result, version_with_filetype_duration_ms = run_command(
        version_with_filetype_cmd
    )
    supports_from_command = filetype_version_result.returncode != 127
    supports_filetype_obj = supports_from_help or supports_from_command

    return {
        "help_exit_code": help_result.returncode,
        "help_duration_ms": help_duration_ms,
        "help_mentions_filetype": mentions_filetype,
        "help_mentions_obj": mentions_obj,
        "version_with_filetype_exit_code": filetype_version_result.returncode,
        "version_with_filetype_duration_ms": version_with_filetype_duration_ms,
        "supports_filetype_obj": supports_filetype_obj,
    }
