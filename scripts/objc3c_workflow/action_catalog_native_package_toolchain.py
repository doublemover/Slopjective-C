"""Native runnable toolchain package action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS: dict[str, ActionSpec] = {
    "package-runnable-toolchain": ActionSpec(
        "package-runnable-toolchain",
        "package the runnable native toolchain",
        "pwsh:scripts/package_objc3c_runnable_toolchain.ps1",
        validation_tier="repo",
        guarantee_owner=(
            "runnable native toolchain package output stays rooted in the checked-in "
            "package script and public workflow bridge"
        ),
    ),
}

__all__ = ["NATIVE_PACKAGE_TOOLCHAIN_ACTION_SPECS"]
