"""Public bridge payload serialization."""

from __future__ import annotations

from dataclasses import asdict

from .public_bridge_constants import PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE
from .public_bridge_registry import PACKAGE_BRIDGES
from .registry_schema_index import capability_truth_schema_ids


def describe_package_bridge_payload(script_name: str) -> dict[str, object]:
    payload = asdict(PACKAGE_BRIDGES[script_name])
    payload["capability_truth"] = {
        "scope": payload["capability_truth_scope"],
        "package_bridge": payload["package_bridge"],
        "machine_readable": True,
        "owner_surface": PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
        "schema_ids": capability_truth_schema_ids(),
        "owned_fields": [
            "package_bridge",
            "backend",
            "invocation_template",
            "public_entrypoint",
        ],
    }
    return payload


__all__ = ["describe_package_bridge_payload"]
