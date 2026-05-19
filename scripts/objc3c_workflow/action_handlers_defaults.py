"""Default workflow action handlers."""

from __future__ import annotations

from scripts.objc3c_workflow.actions import native_build, test_orchestration


def action_build_default(_: list[str]) -> int:
    return native_build.action_build_native_binaries([])


def action_test_default(_: list[str]) -> int:
    return test_orchestration.action_test_smoke([])
