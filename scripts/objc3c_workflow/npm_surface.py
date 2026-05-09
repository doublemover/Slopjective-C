"""Package-script wrapper for canonical npm bridge metadata."""

from __future__ import annotations

from .npm_surface_policy import package_script_payload_name
from .public_bridge_payloads import describe_package_bridge_payload


def describe_package_script_payload(script_name: str) -> dict[str, object]:
    return describe_package_bridge_payload(package_script_payload_name(script_name))
