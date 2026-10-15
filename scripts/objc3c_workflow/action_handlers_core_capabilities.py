"""Core LLVM capability handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import developer_tooling
from scripts.objc3c_workflow.actions.ci_llvm_install import action_materialize_ci_llvm

CORE_CAPABILITY_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "materialize-ci-llvm": action_materialize_ci_llvm,
    "check-llvm-capabilities": developer_tooling.action_check_llvm_capabilities,
    "check-hosted-llvm-capabilities": developer_tooling.action_check_hosted_llvm_capabilities,
}

__all__ = ["CORE_CAPABILITY_ACTION_HANDLERS"]
