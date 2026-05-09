"""Package bridge fields for workflow registry payloads."""

from __future__ import annotations

from .public_bridge import PACKAGE_BRIDGES


def registry_bridge_fields() -> dict[str, object]:
    return {
        "package_bridge_count": len(PACKAGE_BRIDGES),
        "package_bridges": list(PACKAGE_BRIDGES),
        "single_package_bridge_only": True,
    }
