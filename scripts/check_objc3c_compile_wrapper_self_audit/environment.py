"""Environment discovery for the objc3c compile-wrapper self-audit checker."""

from __future__ import annotations

import shutil


def find_pwsh() -> str:
    for candidate in ("pwsh", "powershell"):
        resolved = shutil.which(candidate)
        if resolved:
            return resolved
    raise RuntimeError("PowerShell is required for objc3c compile-wrapper self-audit")


__all__ = ["find_pwsh"]
