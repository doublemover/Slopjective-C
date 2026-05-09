"""Package ecosystem integration action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PACKAGE_INTEGRATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-package-ecosystem": ActionSpec("validate-package-ecosystem", "validate the integrated package ecosystem workflow against stdlib program and canonical application surfaces", "python:scripts/check_objc3c_package_ecosystem_integration.py", validation_tier="repo", guarantee_owner="package ecosystem claims stay grounded in deterministic locks, stdlib programs, and canonical application workspaces"),
    "validate-runnable-package-ecosystem": ActionSpec("validate-runnable-package-ecosystem", "validate package authoring and offline mirror workflows from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py", validation_tier="full", guarantee_owner="packaged dependency and offline mirror workflows stay reproducible from the staged runnable toolchain bundle"),
}

__all__ = ["PACKAGE_INTEGRATION_ACTION_SPECS"]
