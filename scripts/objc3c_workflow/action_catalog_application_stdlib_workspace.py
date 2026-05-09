"""Stdlib workspace materialization action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS: dict[str, ActionSpec] = {
    "materialize-stdlib-workspace": ActionSpec("materialize-stdlib-workspace", "copy the checked-in stdlib workspace and lowering/import contracts into a machine-owned artifact root under tmp", "python:scripts/materialize_objc3c_stdlib_workspace.py", validation_tier="repo", guarantee_owner="stdlib workspace materializations stay machine-owned and derived from the checked-in stdlib root plus lowering/import contract surface", pass_through_args=True),
}


__all__ = ["APPLICATION_STDLIB_WORKSPACE_ACTION_SPECS"]
