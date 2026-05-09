"""Capability-routed source parity workflow action."""

from __future__ import annotations

import sys

from ..commands import run
from .developer_tooling_llvm_contracts import capability_routed_parity_command
from .hosted_llvm_summary import hosted_llc_object_emission_available


def action_test_capability_routed_source_parity(_: list[str]) -> int:
    if not hosted_llc_object_emission_available():
        print(
            "Capability-routed source parity requires hosted llc --filetype=obj; "
            "no native object parity success claim can be published.",
            file=sys.stderr,
        )
        return 1
    return run(capability_routed_parity_command())
