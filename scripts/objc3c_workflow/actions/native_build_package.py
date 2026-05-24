"""Native runnable package and proof workflow actions."""

from __future__ import annotations

from ..commands import pwsh_file
from .native_build_paths import PACKAGE_PS1, PROOF_PS1


def _strip_separator(rest: list[str]) -> list[str]:
    return rest[1:] if rest[:1] == ["--"] else rest


def action_package_runnable_toolchain(rest: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1, *_strip_separator(rest))


def action_package_runnable_toolchain_asan(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1, "-SanitizerVariant", "address")


def action_package_runnable_toolchain_ubsan(_: list[str]) -> int:
    return pwsh_file(PACKAGE_PS1, "-SanitizerVariant", "undefined")


def action_proof_objc3c(_: list[str]) -> int:
    return pwsh_file(PROOF_PS1)
