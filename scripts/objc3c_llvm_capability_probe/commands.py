"""Subprocess-backed LLVM capability probes."""

from __future__ import annotations

import subprocess
import shutil
import platform
import tempfile
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


def default_object_emission_target_triple() -> str:
    system = platform.system().lower()
    machine = platform.machine().lower()
    if system == "windows" and machine in {"amd64", "x86_64"}:
        return "x86_64-pc-windows-msvc"
    if system == "linux" and machine in {"amd64", "x86_64"}:
        return "x86_64-unknown-linux-gnu"
    if system == "darwin" and machine in {"arm64", "aarch64"}:
        return "aarch64-apple-darwin"
    return f"{machine or 'unknown'}-unknown-{system or 'unknown'}"


def _target_object_probe_ir(target_triple: str) -> str:
    return "\n".join(
        [
            '; ModuleID = "objc3c-native-object-emission-capability-probe"',
            'source_filename = "objc3c-native-object-emission-capability-probe"',
            f'target triple = "{target_triple}"',
            "",
            "define i32 @objc3c_native_object_emission_capability_probe() {",
            "entry:",
            "  ret i32 0",
            "}",
            "",
        ]
    )


def probe_llc_filetype_obj(path: Path, *, target_triple: str | None = None) -> dict[str, object]:
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

    resolved_target_triple = target_triple or default_object_emission_target_triple()
    target_object_exit_code = 1
    target_object_duration_ms = 0.0
    target_object_created = False
    target_object_size_bytes = 0
    target_object_diagnostic = ""
    if supports_filetype_obj:
        with tempfile.TemporaryDirectory(prefix="objc3c-llc-object-probe-") as temp_dir_text:
            temp_dir = Path(temp_dir_text)
            ir_path = temp_dir / "probe.ll"
            object_path = temp_dir / ("probe.obj" if platform.system().lower() == "windows" else "probe.o")
            ir_path.write_text(_target_object_probe_ir(resolved_target_triple), encoding="utf-8")
            target_object_cmd = [
                str(path),
                "--filetype=obj",
                f"--mtriple={resolved_target_triple}",
                "-o",
                str(object_path),
                str(ir_path),
            ]
            target_object_result, target_object_duration_ms = run_command(target_object_cmd)
            target_object_exit_code = target_object_result.returncode
            target_object_created = object_path.is_file()
            target_object_size_bytes = object_path.stat().st_size if target_object_created else 0
            if target_object_exit_code != 0 or not target_object_created or target_object_size_bytes <= 0:
                target_object_diagnostic = first_non_empty_line(
                    (target_object_result.stderr or "") + (target_object_result.stdout or "")
                )

    supports_target_object_emission = (
        supports_filetype_obj
        and target_object_exit_code == 0
        and target_object_created
        and target_object_size_bytes > 0
    )

    return {
        "help_exit_code": help_result.returncode,
        "help_duration_ms": help_duration_ms,
        "help_mentions_filetype": mentions_filetype,
        "help_mentions_obj": mentions_obj,
        "version_with_filetype_exit_code": filetype_version_result.returncode,
        "version_with_filetype_duration_ms": version_with_filetype_duration_ms,
        "supports_filetype_obj": supports_filetype_obj,
        "target_triple": resolved_target_triple,
        "target_object_exit_code": target_object_exit_code,
        "target_object_duration_ms": target_object_duration_ms,
        "target_object_created": target_object_created,
        "target_object_size_bytes": target_object_size_bytes,
        "supports_target_object_emission": supports_target_object_emission,
        "target_object_diagnostic": target_object_diagnostic,
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
