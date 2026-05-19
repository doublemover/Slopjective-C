"""Public npm bridge metadata for the objc3c workflow."""

from __future__ import annotations

from .public_bridge_constants import (
    PACKAGE_BRIDGE_IDS,
    PACKAGE_BRIDGE_INVOCATIONS,
    PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE,
    PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE,
    PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
    PUBLIC_BRIDGE_INTEGRITY_OWNER_SURFACE,
    PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE,
    PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_MODULE,
    WORKFLOW_PUBLIC_ACTION_TOKEN,
    WORKFLOW_PUBLIC_COMMAND_PREFIX,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from .public_bridge_invocation import (
    public_action_invocation,
    public_action_invocation_template,
)
from .public_bridge_integrity import (
    PublicBridgeIntegrity,
    package_bridge_spec,
    public_bridge_integrity,
    public_bridge_integrity_fields,
    require_public_bridge_integrity,
)
from .public_bridge_model import PackageBridgeSpec
from .public_bridge_payloads import describe_package_bridge_payload
from .public_bridge_registry import OBJC3C_PACKAGE_BRIDGE, PACKAGE_BRIDGES


__all__ = [
    "OBJC3C_PACKAGE_BRIDGE",
    "PACKAGE_BRIDGES",
    "PACKAGE_BRIDGE_IDS",
    "PACKAGE_BRIDGE_INVOCATIONS",
    "PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE",
    "PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE",
    "PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID",
    "PUBLIC_BRIDGE_INTEGRITY_OWNER_SURFACE",
    "PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE",
    "PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE",
    "PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE",
    "PUBLIC_ENTRYPOINT_KIND",
    "PackageBridgeSpec",
    "PublicBridgeIntegrity",
    "WORKFLOW_BRIDGE_SCRIPT",
    "WORKFLOW_MODULE",
    "WORKFLOW_PUBLIC_ACTION_TOKEN",
    "WORKFLOW_PUBLIC_COMMAND_PREFIX",
    "WORKFLOW_PUBLIC_COMMAND_TEMPLATE",
    "WORKFLOW_RUNNER_MODE",
    "WORKFLOW_RUNNER_SURFACE",
    "describe_package_bridge_payload",
    "package_bridge_spec",
    "public_action_invocation",
    "public_action_invocation_template",
    "public_bridge_integrity",
    "public_bridge_integrity_fields",
    "require_public_bridge_integrity",
]
