"""Native compiler backend selection and invocation surfaces."""

from __future__ import annotations

import json
import os
from pathlib import Path

from objc3c_tooling.probe_compile import normal_user_manifest_link_args
from objc3c_workflow.command_powershell_policy import powershell_file_command

from .native_binaries import PWSH
from .paths import COMPILE_PS1
from .paths import NATIVE_EXE
from .paths import RUNTIME_LIB
from .process_execution import run


DIRECT_COMPILE_BACKEND = "direct-native"
WRAPPER_COMPILE_BACKEND = "powershell-wrapper"
LIVE_ERROR_RUNTIME_SURFACE_FLAG = "--objc3-enable-live-error-runtime-surface"
DEFAULT_COMPILE_BACKEND = os.environ.get(
    "OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND",
    DIRECT_COMPILE_BACKEND,
).strip().lower() or DIRECT_COMPILE_BACKEND


def explicit_live_error_runtime_args(extra_args: list[str] | None = None) -> list[str]:
    args = list(extra_args or [])
    if LIVE_ERROR_RUNTIME_SURFACE_FLAG not in args:
        args.append(LIVE_ERROR_RUNTIME_SURFACE_FLAG)
    return args


def runtime_registration_manifest_path(obj_path: Path) -> Path:
    return obj_path.with_name("module.runtime-registration-manifest.json")


def runtime_driver_linker_flags(obj_path: Path) -> list[str]:
    manifest_path = runtime_registration_manifest_path(obj_path)
    try:
        payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise RuntimeError(
            f"fixture link missing runtime registration manifest for {obj_path}: "
            f"{manifest_path}"
        ) from exc
    except json.JSONDecodeError as exc:
        raise RuntimeError(
            f"fixture link runtime registration manifest is invalid JSON: {manifest_path}"
        ) from exc
    raw_flags = payload.get("driver_linker_flags")
    if not isinstance(raw_flags, list) or not raw_flags:
        raise RuntimeError(
            "fixture link runtime registration manifest missing "
            f"non-empty driver_linker_flags: {manifest_path}"
        )
    flags: list[str] = []
    for index, flag in enumerate(raw_flags):
        if not isinstance(flag, str) or not flag.strip():
            raise RuntimeError(
                "fixture link runtime registration manifest has invalid "
                f"driver_linker_flags entry {index}: {manifest_path}"
            )
        flags.append(flag)
    return flags


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
            powershell_file_command(
                PWSH,
                COMPILE_PS1,
                str(fixture),
                "--out-dir",
                str(out_dir),
                "--emit-prefix",
                "module",
                *(extra_args or []),
            ),
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
        *runtime_driver_linker_flags(obj_path),
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
    "LIVE_ERROR_RUNTIME_SURFACE_FLAG",
    "PWSH",
    "WRAPPER_COMPILE_BACKEND",
    "compile_command",
    "explicit_live_error_runtime_args",
    "link_fixture_executable",
    "runtime_driver_linker_flags",
    "runtime_registration_manifest_path",
]
