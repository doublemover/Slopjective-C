"""Explicit field ownership for workflow action payloads."""

from __future__ import annotations

from dataclasses import dataclass


ACTION_PAYLOAD_BUILDER_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_payload_builder.py"
)
ACTION_PAYLOAD_PUBLIC_FIELDS_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_payload_public_fields.py"
)
ACTION_PAYLOAD_SCHEMA_FIELDS_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_payload_schema_fields.py"
)
ACTION_PAYLOAD_CAPABILITY_TRUTH_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_payload_capability_truth.py"
)
ACTION_PAYLOAD_FIELD_OWNER_SURFACE = (
    "scripts/objc3c_workflow/action_payload_field_owners.py"
)


@dataclass(frozen=True)
class ActionPayloadFieldOwner:
    field_group: str
    fields: tuple[str, ...]
    owner_surface: str
    payload_surface: str
    capability_truth_scope: str
    public_contract: bool


ACTION_PAYLOAD_FIELD_OWNERS: tuple[ActionPayloadFieldOwner, ...] = (
    ActionPayloadFieldOwner(
        field_group="public-invocation",
        fields=(
            "mode",
            "runner_path",
            "category",
            "audience",
            "package_bridge",
            "public_invocation",
            "public_command",
            "public_entrypoint",
        ),
        owner_surface=ACTION_PAYLOAD_PUBLIC_FIELDS_OWNER_SURFACE,
        payload_surface="npm run objc3c -- --list-json actions[]",
        capability_truth_scope="workflow-action-public-invocation",
        public_contract=True,
    ),
    ActionPayloadFieldOwner(
        field_group="schema-reference",
        fields=(
            "payload_schema_ref",
            "registry_schema_id",
            "registry_schema_path",
        ),
        owner_surface=ACTION_PAYLOAD_SCHEMA_FIELDS_OWNER_SURFACE,
        payload_surface="npm run objc3c -- --list-json actions[]",
        capability_truth_scope="workflow-action-schema-reference",
        public_contract=True,
    ),
    ActionPayloadFieldOwner(
        field_group="capability-truth",
        fields=("capability_truth",),
        owner_surface=ACTION_PAYLOAD_CAPABILITY_TRUTH_OWNER_SURFACE,
        payload_surface="npm run objc3c -- --list-json actions[]",
        capability_truth_scope="workflow-action-capability-truth",
        public_contract=True,
    ),
)


def action_payload_field_owner_entries() -> list[dict[str, object]]:
    return [
        {
            "field_group": owner.field_group,
            "fields": list(owner.fields),
            "owner_surface": owner.owner_surface,
            "payload_surface": owner.payload_surface,
            "capability_truth_scope": owner.capability_truth_scope,
            "public_contract": owner.public_contract,
        }
        for owner in ACTION_PAYLOAD_FIELD_OWNERS
    ]


def action_payload_field_owner_map() -> dict[str, str]:
    return {
        field: owner.owner_surface
        for owner in ACTION_PAYLOAD_FIELD_OWNERS
        for field in owner.fields
    }


def action_payload_owner_fields(action: str) -> dict[str, object]:
    return {
        "payload_owner_surface": ACTION_PAYLOAD_BUILDER_OWNER_SURFACE,
        "payload_field_owner_surface": ACTION_PAYLOAD_FIELD_OWNER_SURFACE,
        "payload_owner_action": action,
        "payload_field_owners": action_payload_field_owner_entries(),
        "payload_field_owner_map": action_payload_field_owner_map(),
    }


__all__ = [
    "ACTION_PAYLOAD_BUILDER_OWNER_SURFACE",
    "ACTION_PAYLOAD_CAPABILITY_TRUTH_OWNER_SURFACE",
    "ACTION_PAYLOAD_FIELD_OWNERS",
    "ACTION_PAYLOAD_FIELD_OWNER_SURFACE",
    "ACTION_PAYLOAD_PUBLIC_FIELDS_OWNER_SURFACE",
    "ACTION_PAYLOAD_SCHEMA_FIELDS_OWNER_SURFACE",
    "ActionPayloadFieldOwner",
    "action_payload_field_owner_entries",
    "action_payload_field_owner_map",
    "action_payload_owner_fields",
]
