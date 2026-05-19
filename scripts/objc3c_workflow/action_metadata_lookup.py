"""Combined registry and handler metadata lookup for workflow actions."""

from __future__ import annotations

from dataclasses import dataclass

from scripts.objc3c_workflow.action_handler_lookup import registered_action_handler
from scripts.objc3c_workflow.action_spec import ActionHandler, ActionSpec
from scripts.objc3c_workflow.registry_views import action_spec

ACTION_METADATA_LOOKUP_CONTRACT_ID = "objc3c-workflow-action-metadata-lookup-v1"
ACTION_METADATA_LOOKUP_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_metadata_lookup.py"
)


@dataclass(frozen=True)
class WorkflowActionMetadata:
    action: str
    spec: ActionSpec | None
    handler: ActionHandler | None

    @property
    def registered(self) -> bool:
        return self.spec is not None and self.handler is not None

    @property
    def pass_through_args(self) -> bool:
        return bool(self.spec is not None and self.spec.pass_through_args)


def workflow_action_metadata(action: str) -> WorkflowActionMetadata:
    return WorkflowActionMetadata(
        action=action,
        spec=action_spec(action),
        handler=registered_action_handler(action),
    )


def action_metadata_lookup_contract_payload() -> dict[str, object]:
    return {
        "contract_id": ACTION_METADATA_LOOKUP_CONTRACT_ID,
        "owner_surface": ACTION_METADATA_LOOKUP_OWNER_SURFACE,
        "registry_view_surface": "scripts/objc3c_workflow/registry_views.py",
        "handler_lookup_surface": "scripts/objc3c_workflow/action_handler_lookup.py",
        "combines_registry_spec_and_handler": True,
        "registered_requires_spec_and_handler": True,
    }


__all__ = [
    "ACTION_METADATA_LOOKUP_CONTRACT_ID",
    "ACTION_METADATA_LOOKUP_OWNER_SURFACE",
    "WorkflowActionMetadata",
    "action_metadata_lookup_contract_payload",
    "workflow_action_metadata",
]
