"""Package ecosystem action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PACKAGE_ECOSYSTEM_ACTION_SPECS: dict[str, ActionSpec] = {
    "build-package-lock": ActionSpec("build-package-lock", "generate the local package lock from checked-in stdlib and showcase package surfaces", "python:scripts/build_objc3c_package_lock.py", validation_tier="repo", guarantee_owner="package locks stay deterministic, provenance-bearing, and derived from checked-in local package surfaces"),
    "validate-package-authoring": ActionSpec("validate-package-authoring", "validate local package authoring and deterministic lock generation", "python:scripts/check_objc3c_package_authoring_workflow.py", validation_tier="repo", guarantee_owner="package authoring stays replayable through checked-in package surfaces and public workflow commands"),
    "validate-package-mirror": ActionSpec("validate-package-mirror", "validate offline mirror and local registry metadata reproducibility from the generated package lock", "python:scripts/check_objc3c_package_registry_mirror_reproducibility.py", validation_tier="repo", guarantee_owner="offline mirror and local registry metadata stay lock-derived, no-network, and hosted-registry-deferred"),
    "validate-package-ecosystem": ActionSpec("validate-package-ecosystem", "validate the integrated package ecosystem workflow against stdlib program and canonical application surfaces", "python:scripts/check_objc3c_package_ecosystem_integration.py", validation_tier="repo", guarantee_owner="package ecosystem claims stay grounded in deterministic locks, stdlib programs, and canonical application workspaces"),
    "validate-runnable-package-ecosystem": ActionSpec("validate-runnable-package-ecosystem", "validate package authoring and offline mirror workflows from the staged runnable toolchain bundle", "python:scripts/check_objc3c_runnable_package_ecosystem_end_to_end.py", validation_tier="full", guarantee_owner="packaged dependency and offline mirror workflows stay reproducible from the staged runnable toolchain bundle"),
}
