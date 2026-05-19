from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.subprocesses import run_capture

from .constants import PWSH


def compile_template_source(
    *,
    package_root: Path,
    compile_wrapper: Path,
    template_source: Path,
    compile_dir: Path,
    example_id: str,
) -> None:
    compile_result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(compile_wrapper),
            str(template_source),
            "--out-dir",
            str(compile_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=package_root,
    )
    if compile_result.returncode != 0:
        raise RuntimeError(f"packaged compile wrapper failed for bonus template {example_id}")


def link_template_executable(
    *,
    package_root: Path,
    clangxx: str,
    object_path: Path,
    runtime_library: Path,
    registration_manifest: dict[str, Any],
    exe_path: Path,
    link_log: Path,
    example_id: str,
) -> None:
    link_command = [
        clangxx,
        str(object_path),
        str(runtime_library),
        *[str(flag) for flag in registration_manifest.get("driver_linker_flags", [])],
        "-o",
        str(exe_path),
        "-fno-color-diagnostics",
    ]
    link_result = subprocess.run(
        link_command,
        cwd=package_root,
        check=False,
        text=True,
        capture_output=True,
    )
    link_log.write_text((link_result.stdout or "") + (link_result.stderr or ""), encoding="utf-8")
    if link_result.returncode != 0:
        raise RuntimeError(f"packaged bonus template link failed for {example_id}")


def run_template_executable(
    *,
    package_root: Path,
    exe_path: Path,
    run_log: Path,
    expected_exit: int,
    example_id: str,
) -> int:
    run_result = subprocess.run(
        [str(exe_path)],
        cwd=package_root,
        check=False,
        text=True,
        capture_output=True,
    )
    run_log.write_text((run_result.stdout or "") + (run_result.stderr or ""), encoding="utf-8")
    if run_result.returncode != expected_exit:
        raise RuntimeError(
            f"packaged bonus template {example_id} exited {run_result.returncode}, expected {expected_exit}"
        )
    return int(run_result.returncode)


__all__ = (
    "compile_template_source",
    "link_template_executable",
    "run_template_executable",
)
