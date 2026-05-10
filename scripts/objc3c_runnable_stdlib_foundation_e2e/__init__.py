"""Runnable stdlib foundation end-to-end checker internals."""

from __future__ import annotations

from .assertions import expect
from .cli import main
from .compilation import compile_stdlib_modules
from .constants import (
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    SUMMARY_CONTRACT_ID,
)
from .manifest import load_stdlib_package_surface
from .models import StdlibCompileResult, StdlibPackageSurface
from .packaging import run_package_command
from .summary import build_summary_payload, write_summary


__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "StdlibCompileResult",
    "StdlibPackageSurface",
    "build_summary_payload",
    "compile_stdlib_modules",
    "expect",
    "load_stdlib_package_surface",
    "main",
    "run_package_command",
    "write_summary",
]
