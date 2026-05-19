"""Canonical single npm bridge registry."""

from __future__ import annotations

from .action_audience_constants import AUDIENCE_OPERATOR
from .public_bridge_constants import (
    PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE,
    PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE,
    PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE,
    PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from .public_bridge_model import PackageBridgeSpec


OBJC3C_PACKAGE_BRIDGE = PackageBridgeSpec(
    package_bridge=WORKFLOW_BRIDGE_SCRIPT,
    action="<action>",
    summary="canonical npm bridge for the objc3c workflow action registry",
    audience=AUDIENCE_OPERATOR,
    category="bridge",
    backend=WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
    validation_tier="repo",
    guarantee_owner=(
        "GitHub Actions and local npm users route workflow actions through one package bridge"
    ),
    pass_through_args=True,
    mode=WORKFLOW_RUNNER_MODE,
    runner_path=WORKFLOW_RUNNER_SURFACE,
    public_entrypoint=PUBLIC_ENTRYPOINT_KIND,
    invocation_template=WORKFLOW_PUBLIC_COMMAND_TEMPLATE,
    constants_owner_surface=PUBLIC_BRIDGE_CONSTANTS_OWNER_SURFACE,
    invocation_owner_surface=PUBLIC_BRIDGE_INVOCATION_OWNER_SURFACE,
    payload_owner_surface=PUBLIC_BRIDGE_PAYLOAD_OWNER_SURFACE,
    registry_owner_surface=PUBLIC_BRIDGE_REGISTRY_OWNER_SURFACE,
    capability_truth_scope=PUBLIC_BRIDGE_CAPABILITY_TRUTH_SCOPE,
)

PACKAGE_BRIDGES: dict[str, PackageBridgeSpec] = {
    OBJC3C_PACKAGE_BRIDGE.package_bridge: OBJC3C_PACKAGE_BRIDGE,
}


__all__ = ["OBJC3C_PACKAGE_BRIDGE", "PACKAGE_BRIDGES"]
