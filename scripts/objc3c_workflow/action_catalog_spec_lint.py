"""Spec lint action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

SPEC_LINT_ACTION_SPECS: dict[str, ActionSpec] = {
    "lint-spec": ActionSpec("lint-spec", "run spec lint", "python:scripts/spec_lint.py"),
}
