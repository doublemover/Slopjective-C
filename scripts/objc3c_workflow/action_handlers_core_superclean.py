"""Core release-evidence and repo-superclean handler section."""

from __future__ import annotations

from scripts.objc3c_workflow.action_spec import ActionHandler
from scripts.objc3c_workflow.actions import hygiene

CORE_SUPERCLEAN_ACTION_HANDLERS: dict[str, ActionHandler] = {
    "check-release-evidence": hygiene.action_check_release_evidence,
    "check-repo-superclean-surface": hygiene.action_check_repo_superclean_surface,
    "validate-repo-superclean": hygiene.action_validate_repo_superclean,
}

__all__ = ["CORE_SUPERCLEAN_ACTION_HANDLERS"]
