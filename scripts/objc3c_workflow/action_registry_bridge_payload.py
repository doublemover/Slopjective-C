"""Package bridge fields for workflow registry payloads."""

from __future__ import annotations

from dataclasses import asdict

from .public_bridge_constants import (
    PACKAGE_BRIDGE_INVOCATIONS,
    PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE,
    PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE,
    PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE,
    PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
)
from .public_bridge_registry import PACKAGE_BRIDGES
from .registry_schema_index import capability_truth_schema_ids


def registry_bridge_fields() -> dict[str, object]:
    return {
        "package_bridge_count": len(PACKAGE_BRIDGES),
        "package_bridges": list(PACKAGE_BRIDGES),
        "package_bridge_invocations": list(PACKAGE_BRIDGE_INVOCATIONS),
        "single_package_bridge_only": True,
        "public_bridge_owners": {
            "constants": PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE,
            "invocation": PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE,
            "payload": PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
            "registry": PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
        },
        "public_bridge_contracts": [
            asdict(spec) for spec in PACKAGE_BRIDGES.values()
        ],
        "public_bridge_capability_truth": {
            "scope": PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE,
            "machine_readable": True,
            "owner_surface": PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
            "schema_ids": capability_truth_schema_ids(),
            "owned_fields": [
                "package_bridge_count",
                "package_bridges",
                "package_bridge_invocations",
                "single_package_bridge_only",
            ],
        },
    }
