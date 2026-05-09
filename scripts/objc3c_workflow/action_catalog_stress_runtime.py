"""Lowering/runtime stress action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

STRESS_RUNTIME_ACTION_SPECS: dict[str, ActionSpec] = {
    "test-lowering-runtime-stress": ActionSpec(
        "test-lowering-runtime-stress",
        "run the bounded lowering/runtime stress harness over checked-in fixtures",
        "python:scripts/run_objc3c_lowering_runtime_stress.py",
        validation_tier="repo",
        guarantee_owner=(
            "lowering-heavy compile paths and execution-smoke subsets stay runnable "
            "through the live compiler and runtime path"
        ),
        pass_through_args=True,
    ),
    "test-mixed-module-differential": ActionSpec(
        "test-mixed-module-differential",
        "run mixed-module and import/export differential stress validation",
        "python:scripts/run_objc3c_mixed_module_differential.py",
        validation_tier="repo",
        guarantee_owner=(
            "provider-consumer import/export and mixed-image surfaces stay executable "
            "on the live runtime acceptance path"
        ),
        pass_through_args=True,
    ),
}

__all__ = ["STRESS_RUNTIME_ACTION_SPECS"]
