"""Capability-routed source parity workflow action."""

from __future__ import annotations

import sys

from ..commands import run
from .developer_tooling_llvm_contracts import capability_routed_parity_command
from .hosted_llvm_summary import (
    hosted_llc_object_emission_available,
    hosted_native_object_emission_status,
)


def action_test_capability_routed_source_parity(_: list[str]) -> int:
    if not hosted_llc_object_emission_available():
        status = hosted_native_object_emission_status()
        print(
            "Capability-routed source parity requires hosted llc --filetype=obj; "
            f"{status}; skipping without publishing a native object parity "
            "success claim.",
            file=sys.stderr,
        )
        return 0
    return run(capability_routed_parity_command())
