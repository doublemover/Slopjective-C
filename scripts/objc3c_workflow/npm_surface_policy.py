"""Owned package-script surface policy for workflow bridge metadata."""

from __future__ import annotations

from collections.abc import Mapping

from .public_bridge_model import PackageBridgeSpec

NPM_SURFACE_POLICY_OWNER = "objc3c-workflow-npm-surface-policy"
PACKAGE_SCRIPT_LOOKUP_OWNER = "objc3c-workflow-package-script-lookup"


def package_script_is_registered(
    package_bridges: Mapping[str, PackageBridgeSpec],
    script_name: str,
) -> bool:
    return script_name in package_bridges


def package_script_payload_name(script_name: str) -> str:
    return script_name


__all__ = [
    "NPM_SURFACE_POLICY_OWNER",
    "PACKAGE_SCRIPT_LOOKUP_OWNER",
    "package_script_is_registered",
    "package_script_payload_name",
]
