"""Package-script wrapper for canonical npm bridge metadata."""

from __future__ import annotations

from .npm_surface_policy import (
    package_script_lookup_fields,
    package_script_payload_name,
)
from .public_bridge_payloads import describe_package_bridge_payload
from .public_bridge_registry import PACKAGE_BRIDGES


def describe_package_script_payload(script_name: str) -> dict[str, object]:
    payload_name = package_script_payload_name(script_name)
    payload = describe_package_bridge_payload(payload_name)
    payload["script_name"] = payload_name
    payload["package_script_lookup"] = package_script_lookup_fields(
        PACKAGE_BRIDGES,
        payload_name,
    )
    return payload
