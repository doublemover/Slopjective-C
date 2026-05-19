"""Runnable bonus-experience end-to-end checker internals."""

from __future__ import annotations

from .assertions import expect
from .capability_probe import run_capability_probe
from .cli import main
from .constants import (
    PACKAGE_CONTRACT_ID,
    PACKAGE_PS1,
    PWSH,
    REPORT_PATH,
    ROOT,
    SUMMARY_CONTRACT_ID,
)
from .examples import materialize_and_run_examples
from .manifest import load_bonus_package_surface
from .models import BonusPackageSurface, ExampleRunResult
from .packaging import run_package_command
from .summary import build_summary_payload, write_summary


__all__ = [
    "PACKAGE_CONTRACT_ID",
    "PACKAGE_PS1",
    "PWSH",
    "REPORT_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "BonusPackageSurface",
    "ExampleRunResult",
    "build_summary_payload",
    "expect",
    "load_bonus_package_surface",
    "main",
    "materialize_and_run_examples",
    "run_capability_probe",
    "run_package_command",
    "write_summary",
]
