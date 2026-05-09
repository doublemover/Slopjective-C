"""Package registry and mirror action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

PACKAGE_REGISTRY_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-package-mirror": ActionSpec("validate-package-mirror", "validate offline mirror and local registry metadata reproducibility from the generated package lock", "python:scripts/check_objc3c_package_registry_mirror_reproducibility.py", validation_tier="repo", guarantee_owner="offline mirror and local registry metadata stay lock-derived, no-network, and hosted-registry-deferred"),
}

__all__ = ["PACKAGE_REGISTRY_ACTION_SPECS"]
