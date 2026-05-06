"""Native build, package, and proof workflow actions."""

from __future__ import annotations

from ..commands import pwsh_file
from ..environment import ROOT

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"


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


def action_package_runnable_toolchain(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1)


def action_proof_objc3c(_: list[str]) -> int:
    return pwsh_file(PROOF_PS1)
