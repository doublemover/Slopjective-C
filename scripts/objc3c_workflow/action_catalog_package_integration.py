"""Package ecosystem integration action specs."""

from __future__ import annotations

from .action_catalog_package_integration_claims import (
    PACKAGE_INTEGRATION_PUBLIC_ACTIONS,
)
from .action_catalog_package_public_workflows import package_action_specs

PACKAGE_INTEGRATION_ACTION_SPECS = package_action_specs(
    PACKAGE_INTEGRATION_PUBLIC_ACTIONS
)

__all__ = ["PACKAGE_INTEGRATION_ACTION_SPECS"]
