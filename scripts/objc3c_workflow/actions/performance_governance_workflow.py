"""Composite performance-governance workflow actions."""

from __future__ import annotations

import sys

from ..commands import run
from ..composite_validation import run_composite_validation
from ..environment import ROOT
from .performance_artifacts import (
    PERFORMANCE_GOVERNANCE_DASHBOARD_PY,
    PERFORMANCE_GOVERNANCE_REPORT_PY,
)
from .performance_compiler_throughput import COMPILER_THROUGHPUT_INTEGRATION_PY
from .performance_foundation import PERFORMANCE_INTEGRATION_PY
from .performance_governance_policy import (
    PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY,
    PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY,
)
from .performance_runtime import RUNTIME_PERFORMANCE_INTEGRATION_PY

PERFORMANCE_GOVERNANCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_performance_governance_integration.py"
)
PERFORMANCE_GOVERNANCE_END_TO_END_PY = (
    ROOT / "scripts" / "check_objc3c_performance_governance_end_to_end.py"
)


def action_validate_performance_governance(_: list[str]) -> int:
    return run_composite_validation(
        "validate-performance-governance",
        [
            (
                "validate-performance-foundation",
                [sys.executable, str(PERFORMANCE_INTEGRATION_PY)],
            ),
            (
                "validate-compiler-throughput",
                [sys.executable, str(COMPILER_THROUGHPUT_INTEGRATION_PY)],
            ),
            (
                "validate-runtime-performance",
                [sys.executable, str(RUNTIME_PERFORMANCE_INTEGRATION_PY)],
            ),
            (
                "check-performance-governance-surface",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_SOURCE_SURFACE_PY)],
            ),
            (
                "check-performance-governance-schema-surface",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_SCHEMA_SURFACE_PY)],
            ),
            (
                "build-performance-dashboard",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_DASHBOARD_PY)],
            ),
            (
                "publish-performance-report",
                [sys.executable, str(PERFORMANCE_GOVERNANCE_REPORT_PY)],
            ),
        ],
    )


def action_validate_performance_governance_integration(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_INTEGRATION_PY)])


def action_validate_performance_governance_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(PERFORMANCE_GOVERNANCE_END_TO_END_PY)])


__all__ = [
    "PERFORMANCE_GOVERNANCE_END_TO_END_PY",
    "PERFORMANCE_GOVERNANCE_INTEGRATION_PY",
    "action_validate_performance_governance",
    "action_validate_performance_governance_end_to_end",
    "action_validate_performance_governance_integration",
]
