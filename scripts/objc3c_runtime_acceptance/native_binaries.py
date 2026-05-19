"""Native binary resolution for runtime acceptance."""

from __future__ import annotations

import shutil
from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.probe_compile import find_clangxx
from objc3c_workflow.command_powershell_policy import powershell_file_command

from .paths import BUILD_PS1
from .paths import NATIVE_EXE
from .paths import ROOT
from .paths import RUNTIME_LIB
from .process_execution import run


PWSH = shutil.which("pwsh") or shutil.which("powershell") or "pwsh"


@dataclass(frozen=True)
class NativeBinaryResolution:
    clangxx: str
    native_exe: Path
    runtime_lib: Path


def ensure_native_binaries() -> None:
    native_source_entrypoint = ROOT / "native" / "objc3c" / "src" / "main.cpp"
    if not native_source_entrypoint.is_file():
        if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
            return
        raise RuntimeError(
            "native source tree is not packaged and required runtime executable/library artifacts are missing"
        )
    result = run(
        powershell_file_command(
            PWSH,
            BUILD_PS1,
            "-ExecutionMode",
            "binaries-only",
        )
    )
    if result.returncode != 0:
        raise RuntimeError(
            "native build failed:\nSTDOUT:\n"
            + result.stdout
            + "\nSTDERR:\n"
            + result.stderr
        )
    if not NATIVE_EXE.is_file() or not RUNTIME_LIB.is_file():
        raise RuntimeError("native build completed without publishing the runtime executable/library")


def resolve_native_binary_set() -> NativeBinaryResolution:
    ensure_native_binaries()
    return NativeBinaryResolution(
        clangxx=find_clangxx(),
        native_exe=NATIVE_EXE,
        runtime_lib=RUNTIME_LIB,
    )


__all__ = [
    "NativeBinaryResolution",
    "PWSH",
    "ensure_native_binaries",
    "find_clangxx",
    "resolve_native_binary_set",
]
