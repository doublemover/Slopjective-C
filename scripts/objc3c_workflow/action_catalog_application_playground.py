"""Playground workspace action specs."""

from __future__ import annotations

from .action_spec import ActionSpec
from .actions.developer_tooling_llvm_contract_constants import FRONTEND_RUNNER_BACKEND

APPLICATION_PLAYGROUND_ACTION_SPECS: dict[str, ActionSpec] = {
    "materialize-playground-workspace": ActionSpec("materialize-playground-workspace", "compile one source through the live frontend runner and materialize a machine-owned playground workspace contract under tmp", FRONTEND_RUNNER_BACKEND, validation_tier="repo", guarantee_owner="playground workspaces stay machine-owned, compile-coupled, and rooted in tmp outputs with editor/debug drill references instead of shared evidence-only buckets", pass_through_args=True),
}


__all__ = ["APPLICATION_PLAYGROUND_ACTION_SPECS"]
