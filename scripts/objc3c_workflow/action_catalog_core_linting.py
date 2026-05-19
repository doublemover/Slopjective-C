"""Core lint and dependency-boundary action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

CORE_LINTING_ACTION_SPECS: dict[str, ActionSpec] = {
    "lint": ActionSpec(
        "lint",
        "run the canonical maintainer lint workflow",
        "runner-internal + task hygiene + site build + markdown format check",
    ),
    "check-dependency-boundaries": ActionSpec(
        "check-dependency-boundaries",
        "check strict objc3c dependency boundaries",
        "python:scripts/check_objc3c_dependency_boundaries.py --strict",
        validation_tier="repo",
        guarantee_owner="repo dependency boundaries stay explicit and strict",
    ),
}
