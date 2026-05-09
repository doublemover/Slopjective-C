"""Core hygiene and capability handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import developer_tooling, hygiene

CORE_HYGIENE_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "lint": hygiene.action_lint,
    "check-dependency-boundaries": hygiene.action_check_dependency_boundaries,
    "check-llvm-capabilities": developer_tooling.action_check_llvm_capabilities,
    "check-hosted-llvm-capabilities": developer_tooling.action_check_hosted_llvm_capabilities,
    "check-release-evidence": hygiene.action_check_release_evidence,
    "check-source-hygiene-authenticity": hygiene.action_check_source_hygiene_authenticity,
    "check-source-hygiene-hard-cutover": hygiene.action_check_source_hygiene_hard_cutover,
    "check-task-hygiene": hygiene.action_check_task_hygiene,
    "check-repo-superclean-surface": hygiene.action_check_repo_superclean_surface,
    "validate-repo-superclean": hygiene.action_validate_repo_superclean,
}
