"""Native binary and contract build workflow actions."""

from __future__ import annotations

from ..commands import pwsh_file
from .native_build_paths import BUILD_PS1, COMPILE_PS1


def action_build_native_binaries(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only")


def action_build_native_contracts(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "contracts-binary")


def action_build_native_full(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "full")


def action_build_native_reconfigure(_: list[str]) -> int:
    return pwsh_file(BUILD_PS1, "-ExecutionMode", "binaries-only", "-ForceReconfigure")


def action_compile_objc3c(rest: list[str]) -> int:
    return pwsh_file(COMPILE_PS1, *rest)
