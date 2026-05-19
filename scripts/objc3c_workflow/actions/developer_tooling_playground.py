"""Playground and editor-facing developer-tooling action entrypoints."""

from __future__ import annotations

from .developer_tooling_playground_runner import (
    action_format_objc3c,
    action_inspect_editor_tooling,
    ensure_frontend_runner_ready,
)
from .developer_tooling_playground_workspace import run_playground_workspace


def action_inspect_playground_repro(rest: list[str]) -> int:
    return run_playground_workspace(rest, emit_payload=True)


def action_materialize_playground_workspace(rest: list[str]) -> int:
    return run_playground_workspace(rest, emit_payload=False)
