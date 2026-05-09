"""Package lock and authoring action specs."""

from __future__ import annotations

from .action_catalog_package_lock_contracts import PACKAGE_LOCK_PUBLIC_ACTIONS
from .action_catalog_package_public_workflows import package_action_specs

PACKAGE_LOCK_ACTION_SPECS = package_action_specs(PACKAGE_LOCK_PUBLIC_ACTIONS)

__all__ = ["PACKAGE_LOCK_ACTION_SPECS"]
