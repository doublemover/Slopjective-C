"""Package registry and mirror action specs."""

from __future__ import annotations

from .action_catalog_package_public_workflows import package_action_specs
from .action_catalog_package_registry_publication import (
    PACKAGE_REGISTRY_PUBLIC_ACTIONS,
)

PACKAGE_REGISTRY_ACTION_SPECS = package_action_specs(PACKAGE_REGISTRY_PUBLIC_ACTIONS)

__all__ = ["PACKAGE_REGISTRY_ACTION_SPECS"]
