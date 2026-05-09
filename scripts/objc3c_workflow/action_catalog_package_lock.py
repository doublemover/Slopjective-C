"""Package lock and authoring action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PACKAGE_LOCK_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-package-lock": ActionSpec("build-package-lock", "generate the local package lock from checked-in stdlib and showcase package surfaces", "python:scripts/build_objc3c_package_lock.py", validation_tier="repo", guarantee_owner="package locks stay deterministic, provenance-bearing, and derived from checked-in local package surfaces"),
    "validate-package-authoring": ActionSpec("validate-package-authoring", "validate local package authoring and deterministic lock generation", "python:scripts/check_objc3c_package_authoring_workflow.py", validation_tier="repo", guarantee_owner="package authoring stays replayable through checked-in package surfaces and public workflow commands"),
}

__all__ = ["PACKAGE_LOCK_ACTION_SPECS"]
