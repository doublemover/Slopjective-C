"""Runnable developer-tooling workspace action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-runnable-developer-tooling": ActionSpec("validate-runnable-developer-tooling", "validate packaged editor, formatter, debug, workspace, and integrated developer-tooling behavior from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_developer_tooling_end_to_end.py", validation_tier="full", guarantee_owner="packaged editor, formatter, debug anchors, workspace drills, and integrated developer-tooling validation stay reproducible from the staged runnable toolchain bundle"),
}


__all__ = ["APPLICATION_DEVELOPER_TOOLING_ACTION_SPECS"]
