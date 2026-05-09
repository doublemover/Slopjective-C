"""Public npm bridge metadata for the objc3c workflow."""

from __future__ import annotations

from dataclasses import asdict, dataclass


WORKFLOW_MODULE = "scripts.objc3c_workflow"
WORKFLOW_BRIDGE_SCRIPT = "objc3c"
WORKFLOW_PUBLIC_COMMAND_PREFIX = f"npm run {WORKFLOW_BRIDGE_SCRIPT} --"
WORKFLOW_RUNNER_SURFACE = f"package.json scripts.{WORKFLOW_BRIDGE_SCRIPT} -> {WORKFLOW_MODULE}"
WORKFLOW_RUNNER_MODE = "objc3c-workflow-action-registry-v1"
PUBLIC_ENTRYPOINT_KIND = "single-npm-bridge"


@dataclass(frozen=True)
class PackageBridgeSpec:
    package_bridge: str
    action: str
    summary: str
    audience: str
    category: str
    backend: str
    validation_tier: str
    guarantee_owner: str
    pass_through_args: bool
    mode: str
    runner_path: str
    public_entrypoint: str


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


def public_action_invocation(action: str) -> str:
    return f"{WORKFLOW_PUBLIC_COMMAND_PREFIX} {action}"


def describe_package_bridge_payload(script_name: str) -> dict[str, object]:
    return asdict(PACKAGE_BRIDGES[script_name])
