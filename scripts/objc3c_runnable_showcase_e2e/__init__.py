"""Runnable showcase end-to-end checker internals."""

from __future__ import annotations

from .assertions import expect
from .cli import main
from .constants import (
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    SUMMARY_CONTRACT_ID,
)
from .execution import run_showcase_examples
from .manifest import load_showcase_package_surface
from .models import (
    ShowcaseExecutionResult,
    ShowcaseExampleResult,
    ShowcasePackageSurface,
)
from .packaging import run_package_command
from .report import build_summary_payload, write_summary


__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "ShowcaseExecutionResult",
    "ShowcaseExampleResult",
    "ShowcasePackageSurface",
    "build_summary_payload",
    "expect",
    "load_showcase_package_surface",
    "main",
    "run_package_command",
    "run_showcase_examples",
    "write_summary",
]
