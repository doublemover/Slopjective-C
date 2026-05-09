"""Canonical single npm bridge registry."""

from __future__ import annotations

from .public_bridge_constants import (
    PUBLIC_ENTRYPOINT_KIND,
    WORKFLOW_BRIDGE_SCRIPT,
    WORKFLOW_PUBLIC_COMMAND_PREFIX,
    WORKFLOW_RUNNER_MODE,
    WORKFLOW_RUNNER_SURFACE,
)
from .public_bridge_model import PackageBridgeSpec


OBJC3C_PACKAGE_BRIDGE = PackageBridgeSpec(
    package_bridge=WORKFLOW_BRIDGE_SCRIPT,
    action="<action>",
    summary="canonical npm bridge for the objc3c workflow action registry",
    audience="operator",
    category="bridge",
    backend=f"{WORKFLOW_PUBLIC_COMMAND_PREFIX} <action>",
    validation_tier="repo",
    guarantee_owner=(
        "GitHub Actions and local npm users route workflow actions through one package bridge"
    ),
    pass_through_args=True,
    mode=WORKFLOW_RUNNER_MODE,
    runner_path=WORKFLOW_RUNNER_SURFACE,
    public_entrypoint=PUBLIC_ENTRYPOINT_KIND,
)

PACKAGE_BRIDGES: dict[str, PackageBridgeSpec] = {
    OBJC3C_PACKAGE_BRIDGE.package_bridge: OBJC3C_PACKAGE_BRIDGE,
}


__all__ = ["OBJC3C_PACKAGE_BRIDGE", "PACKAGE_BRIDGES"]
