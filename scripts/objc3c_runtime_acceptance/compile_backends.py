"""Native compiler backend selection and invocation surfaces."""

from __future__ import annotations

import os
from pathlib import Path

from objc3c_tooling.probe_compile import normal_user_manifest_link_args

from .native_binaries import PWSH
from .paths import COMPILE_PS1
from .paths import NATIVE_EXE
from .paths import RUNTIME_LIB
from .process_execution import run


DIRECT_COMPILE_BACKEND = "direct-native"
WRAPPER_COMPILE_BACKEND = "powershell-wrapper"
DEFAULT_COMPILE_BACKEND = os.environ.get(
    "OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND",
    DIRECT_COMPILE_BACKEND,
).strip().lower() or DIRECT_COMPILE_BACKEND


def compile_command(
    fixture: Path,
    out_dir: Path,
    *,
    extra_args: list[str] | None = None,
    backend: str | None = None,
) -> tuple[list[str], str]:
    selected_backend = (backend or DEFAULT_COMPILE_BACKEND).strip().lower()
    if selected_backend in {"wrapper", WRAPPER_COMPILE_BACKEND}:
        return (
            [
                PWSH,
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(COMPILE_PS1),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *(extra_args or []),
            ],
            WRAPPER_COMPILE_BACKEND,
        )
    if selected_backend in {"direct", DIRECT_COMPILE_BACKEND}:
        return (
            [
                str(NATIVE_EXE),
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *(extra_args or []),
            ],
            DIRECT_COMPILE_BACKEND,
        )
    raise RuntimeError(
        "unsupported OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND "
        f"{selected_backend!r}; expected direct-native or powershell-wrapper"
    )


def link_fixture_executable(clangxx: str, obj_path: Path, exe_path: Path) -> None:
    exe_path.parent.mkdir(parents=True, exist_ok=True)
    command = [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        *normal_user_manifest_link_args(),
        str(obj_path),
        str(RUNTIME_LIB),
        "-o",
        str(exe_path),
    ]
    result = run(command)
    if result.returncode != 0:
        raise RuntimeError(
            f"fixture link failed for {obj_path}:\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}"
        )


__all__ = [
    "DEFAULT_COMPILE_BACKEND",
    "DIRECT_COMPILE_BACKEND",
    "PWSH",
    "WRAPPER_COMPILE_BACKEND",
    "compile_command",
    "link_fixture_executable",
]
