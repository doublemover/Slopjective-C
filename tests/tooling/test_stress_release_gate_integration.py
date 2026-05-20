from __future__ import annotations

import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.check_objc3c_stress_integration import validate_performance_regression_gate


def test_stress_integration_accepts_blocking_performance_regression_gate() -> None:
    summary = validate_performance_regression_gate(
        {
            "regression_gate": {
                "gate_id": "objc3c.performance.release.regression-gate.v1",
                "passed": False,
                "release_status": "blocked",
                "blocking_breach_count": 2,
                "warning_breach_count": 1,
            }
        },
        "objc3c.performance.release.regression-gate.v1",
    )

    assert summary == {
        "gate_id": "objc3c.performance.release.regression-gate.v1",
        "passed": False,
        "release_status": "blocked",
        "blocking_breach_count": 2,
        "warning_breach_count": 1,
    }


def test_stress_integration_rejects_missing_performance_regression_gate() -> None:
    with pytest.raises(RuntimeError, match="missing regression_gate"):
        validate_performance_regression_gate({}, "objc3c.performance.release.regression-gate.v1")


def test_stress_integration_rejects_drifted_performance_regression_gate_id() -> None:
    with pytest.raises(RuntimeError, match="gate id drifted"):
        validate_performance_regression_gate(
            {
                "regression_gate": {
                    "gate_id": "objc3c.performance.release.other",
                    "passed": True,
                    "release_status": "release-ready",
                    "blocking_breach_count": 0,
                    "warning_breach_count": 0,
                }
            },
            "objc3c.performance.release.regression-gate.v1",
        )
