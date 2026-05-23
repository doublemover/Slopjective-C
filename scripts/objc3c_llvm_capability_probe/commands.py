"""Subprocess-backed LLVM capability probes."""

from __future__ import annotations

import subprocess
import shutil
from pathlib import Path

from objc3c_tooling.subprocesses import run_timed

from .models import ExecutableProbe
from .parsing import (
    first_non_empty_line,
    llc_help_mentions_filetype_obj,
    tool_vendor_from_headline,
    tool_version_from_text,
)


def run_command(command: list[str]) -> tuple[subprocess.CompletedProcess[str], float]:
    execution = run_timed(command, cwd=None)
    return execution.completed_process(), execution.duration_ms


def resolved_command_path(path: Path) -> tuple[str, str]:
    configured = str(path)
    if path.is_absolute():
        return configured, "configured-absolute" if path.is_file() else "configured-absolute-missing"
    resolved = shutil.which(configured)
    if resolved:
        return resolved, "path-resolved"
    return "", "path-unresolved"


def probe_executable(path: Path, *, role: str) -> dict[str, object]:
    version_cmd = [str(path), "--version"]
    version_result, version_duration_ms = run_command(version_cmd)
    version_text = (version_result.stdout or "") + (version_result.stderr or "")

    found = version_result.returncode != 127
    resolved_path, shadowing_status = resolved_command_path(path)
    if found and not resolved_path:
        resolved_path = str(path)
        shadowing_status = "launch-resolved"
    if not found and not path.is_absolute():
        resolved_path = ""
        shadowing_status = "path-unresolved"
    diagnostic = f"{role} executable not found: {path}" if not found else None
    headline = first_non_empty_line(version_text)
    return ExecutableProbe(
        role=role,
        path=str(path),
        configured_path=str(path),
        resolved_path=resolved_path,
        shadowing_status=shadowing_status,
        found=found,
        version_exit_code=version_result.returncode,
        version_duration_ms=version_duration_ms,
        version_headline=headline,
        version=tool_version_from_text(version_text),
        vendor=tool_vendor_from_headline(headline),
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
    supports_from_command = filetype_version_result.returncode == 0
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


def probe_llvm_config_paths(path: Path) -> dict[str, object]:
    includedir_result, includedir_duration_ms = run_command([str(path), "--includedir"])
    libdir_result, libdir_duration_ms = run_command([str(path), "--libdir"])

    includedir = first_non_empty_line((includedir_result.stdout or "") + (includedir_result.stderr or ""))
    libdir = first_non_empty_line((libdir_result.stdout or "") + (libdir_result.stderr or ""))
    return {
        "includedir_exit_code": includedir_result.returncode,
        "includedir_duration_ms": includedir_duration_ms,
        "includedir": includedir,
        "libdir_exit_code": libdir_result.returncode,
        "libdir_duration_ms": libdir_duration_ms,
        "libdir": libdir,
        "headers_libraries_discovered": (
            includedir_result.returncode == 0
            and libdir_result.returncode == 0
            and bool(includedir)
            and bool(libdir)
        ),
        "discovery_source": "llvm-config",
    }


def _install_root_from_resolved_tool(probe: dict[str, object]) -> Path | None:
    resolved = str(probe.get("resolved_path", ""))
    if not resolved:
        return None
    resolved_path = Path(resolved)
    if resolved_path.parent.name.lower() != "bin":
        return None
    root = resolved_path.parent.parent
    include_dir = root / "include"
    lib_dir = root / "lib"
    if include_dir.is_dir() and lib_dir.is_dir():
        return root
    return None


def probe_llvm_install_root_paths(
    *probes: dict[str, object],
) -> dict[str, object]:
    for probe in probes:
        root = _install_root_from_resolved_tool(probe)
        if root is None:
            continue
        includedir = root / "include"
        libdir = root / "lib"
        return {
            "includedir_exit_code": 0,
            "includedir_duration_ms": 0.0,
            "includedir": str(includedir),
            "libdir_exit_code": 0,
            "libdir_duration_ms": 0.0,
            "libdir": str(libdir),
            "headers_libraries_discovered": True,
            "discovery_source": "install-root",
        }
    return {
        "includedir_exit_code": 1,
        "includedir_duration_ms": 0.0,
        "includedir": "",
        "libdir_exit_code": 1,
        "libdir_duration_ms": 0.0,
        "libdir": "",
        "headers_libraries_discovered": False,
        "discovery_source": "unavailable",
    }
