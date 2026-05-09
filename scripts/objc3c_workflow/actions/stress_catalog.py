"""Stress scenario and script catalog policy."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from ..environment import ROOT

STRESS_SOURCE_SURFACE_PY = ROOT / "scripts" / "check_stress_source_surface.py"
FUZZ_SAFETY_PY = ROOT / "scripts" / "run_objc3c_fuzz_safety.py"
LOWERING_RUNTIME_STRESS_PY = ROOT / "scripts" / "run_objc3c_lowering_runtime_stress.py"
MIXED_MODULE_DIFFERENTIAL_PY = ROOT / "scripts" / "run_objc3c_mixed_module_differential.py"
STRESS_MINIMIZATION_PY = ROOT / "scripts" / "run_objc3c_stress_minimization.py"
STRESS_CRASH_TRIAGE_PY = ROOT / "scripts" / "run_objc3c_stress_crash_triage.py"
STRESS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stress_integration.py"
STRESS_END_TO_END_PY = ROOT / "scripts" / "check_objc3c_stress_end_to_end.py"


@dataclass(frozen=True)
class StressScenario:
    action_name: str
    script: Path
    pass_through_args: bool = False


STRESS_SCENARIOS: dict[str, StressScenario] = {
    "check-stress-surface": StressScenario(
        "check-stress-surface",
        STRESS_SOURCE_SURFACE_PY,
    ),
    "test-fuzz-safety": StressScenario(
        "test-fuzz-safety",
        FUZZ_SAFETY_PY,
        pass_through_args=True,
    ),
    "test-lowering-runtime-stress": StressScenario(
        "test-lowering-runtime-stress",
        LOWERING_RUNTIME_STRESS_PY,
        pass_through_args=True,
    ),
    "test-mixed-module-differential": StressScenario(
        "test-mixed-module-differential",
        MIXED_MODULE_DIFFERENTIAL_PY,
        pass_through_args=True,
    ),
    "test-stress-minimization": StressScenario(
        "test-stress-minimization",
        STRESS_MINIMIZATION_PY,
        pass_through_args=True,
    ),
    "test-stress-crash-triage": StressScenario(
        "test-stress-crash-triage",
        STRESS_CRASH_TRIAGE_PY,
        pass_through_args=True,
    ),
}

VALIDATE_STRESS_CHILD_ACTIONS = (
    "check-stress-surface",
    "test-fuzz-safety",
    "test-lowering-runtime-stress",
    "test-mixed-module-differential",
    "test-stress-minimization",
    "test-stress-crash-triage",
)
