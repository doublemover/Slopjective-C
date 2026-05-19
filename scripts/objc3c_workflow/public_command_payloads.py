"""Public workflow payload helpers."""

from __future__ import annotations

from .public_command_registry import public_workflow_action_names


def public_workflow_list_payload() -> dict[str, object]:
    from .action_payloads import list_actions_payload

    return list_actions_payload()


def public_workflow_action_payload(action: str) -> dict[str, object]:
    from .action_payloads import describe_action_payload

    return describe_action_payload(action)


def public_workflow_action_payloads() -> list[dict[str, object]]:
    return [
        public_workflow_action_payload(action)
        for action in public_workflow_action_names()
    ]


def public_workflow_package_bridge_payload(script_name: str) -> dict[str, object]:
    from .npm_surface import describe_package_script_payload

    return describe_package_script_payload(script_name)


__all__ = [
    "public_workflow_action_payload",
    "public_workflow_action_payloads",
    "public_workflow_list_payload",
    "public_workflow_package_bridge_payload",
]
