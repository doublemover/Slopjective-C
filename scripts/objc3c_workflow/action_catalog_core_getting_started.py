"""Getting-started tutorial action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_GETTING_STARTED_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-getting-started": ActionSpec("validate-getting-started", "run the integrated getting-started tutorial and onboarding validation flow", "python:scripts/check_getting_started_integration.py", validation_tier="repo", guarantee_owner="getting-started tutorials stay compile-coupled, runnable, and wired into the normal repo validation path"),
}


__all__ = ["CORE_GETTING_STARTED_ACTION_SPECS"]
