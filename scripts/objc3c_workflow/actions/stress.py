"""Stress and fuzz workflow actions."""

from __future__ import annotations

import sys

from ..composite_validation import run_composite_validation
from ..commands import run
from ..environment import ROOT
from ..registry import ACTION_SPECS

STRESS_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_stress_source_surface.py"
FUZZ_SAFETY_PY = ROOT / "scripts" / "run_objc3c_fuzz_safety.py"
LOWERING_RUNTIME_STRESS_PY = ROOT / "scripts" / "run_objc3c_lowering_runtime_stress.py"
MIXED_MODULE_DIFFERENTIAL_PY = ROOT / "scripts" / "run_objc3c_mixed_module_differential.py"
STRESS_MINIMIZATION_PY = ROOT / "scripts" / "run_objc3c_stress_minimization.py"
STRESS_CRASH_TRIAGE_PY = ROOT / "scripts" / "run_objc3c_stress_crash_triage.py"
STRESS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stress_integration.py"
STRESS_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_stress_end_to_end.py"


def action_names() -> list[str]:
    return [action for action in ACTION_SPECS if "stress" in action or "fuzz" in action]


def action_check_stress_surface(_: list[str]) -> int:
    return run([sys.executable, str(STRESS_SOURCE_SURFACE_PY)])


def action_test_fuzz_safety(rest: list[str]) -> int:
    return run([sys.executable, str(FUZZ_SAFETY_PY), *rest])


def action_test_lowering_runtime_stress(rest: list[str]) -> int:
    return run([sys.executable, str(LOWERING_RUNTIME_STRESS_PY), *rest])


def action_test_mixed_module_differential(rest: list[str]) -> int:
    return run([sys.executable, str(MIXED_MODULE_DIFFERENTIAL_PY), *rest])


def action_test_stress_minimization(rest: list[str]) -> int:
    return run([sys.executable, str(STRESS_MINIMIZATION_PY), *rest])


def action_test_stress_crash_triage(rest: list[str]) -> int:
    return run([sys.executable, str(STRESS_CRASH_TRIAGE_PY), *rest])


def action_validate_stress(_: list[str]) -> int:
    return run_composite_validation(
        "validate-stress",
        [
            ("check-stress-surface", [sys.executable, str(STRESS_SOURCE_SURFACE_PY)]),
            ("test-fuzz-safety", [sys.executable, str(FUZZ_SAFETY_PY)]),
            (
                "test-lowering-runtime-stress",
                [sys.executable, str(LOWERING_RUNTIME_STRESS_PY)],
            ),
            (
                "test-mixed-module-differential",
                [sys.executable, str(MIXED_MODULE_DIFFERENTIAL_PY)],
            ),
            ("test-stress-minimization", [sys.executable, str(STRESS_MINIMIZATION_PY)]),
            ("test-stress-crash-triage", [sys.executable, str(STRESS_CRASH_TRIAGE_PY)]),
        ],
    )


def action_validate_stress_integration(_: list[str]) -> int:
    return run([sys.executable, str(STRESS_INTEGRATION_PY)])


def action_validate_stress_end_to_end(_: list[str]) -> int:
    return run([sys.executable, str(STRESS_END_TO_END_PY)])
