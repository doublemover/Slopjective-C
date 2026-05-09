"""Package ecosystem action specs."""

from __future__ import annotations

from .action_catalog_package_integration import PACKAGE_INTEGRATION_ACTION_SPECS
from .action_catalog_package_lock import PACKAGE_LOCK_ACTION_SPECS
from .action_catalog_package_registry import PACKAGE_REGISTRY_ACTION_SPECS
from .action_spec import ActionSpec
from scripts.package_ecosystem_contracts import package_ecosystem_owner_payload

PACKAGE_ECOSYSTEM_ACTION_SPECS: dict[str, ActionSpec] = {
    **PACKAGE_LOCK_ACTION_SPECS,
    **PACKAGE_REGISTRY_ACTION_SPECS,
    **PACKAGE_INTEGRATION_ACTION_SPECS,
}

PACKAGE_ECOSYSTEM_ACTION_OWNER_POLICY: dict[str, object] = package_ecosystem_owner_payload()
PACKAGE_ECOSYSTEM_ACTION_OWNER_MAP: dict[str, dict[str, object]] = {
    action: dict(PACKAGE_ECOSYSTEM_ACTION_OWNER_POLICY)
    for action in PACKAGE_ECOSYSTEM_ACTION_SPECS
}

__all__ = [
    "PACKAGE_ECOSYSTEM_ACTION_OWNER_MAP",
    "PACKAGE_ECOSYSTEM_ACTION_OWNER_POLICY",
    "PACKAGE_ECOSYSTEM_ACTION_SPECS",
]
