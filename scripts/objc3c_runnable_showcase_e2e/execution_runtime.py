from __future__ import annotations

import subprocess
from pathlib import Path
from typing import Any


def link_showcase_executable(
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
        raise RuntimeError(f"packaged showcase link failed for {example_id}")


def run_showcase_executable(
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
            f"packaged showcase example {example_id} exited {run_result.returncode}, expected {expected_exit}"
        )
    return int(run_result.returncode)


__all__ = (
    "link_showcase_executable",
    "run_showcase_executable",
)
