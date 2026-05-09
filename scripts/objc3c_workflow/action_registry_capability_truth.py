"""Capability truth fields for workflow registry payloads."""

from __future__ import annotations

from .argument_option_contracts import (
    ARGUMENT_OPTION_CONTRACT_ID,
    ARGUMENT_OPTION_OWNER_SURFACE,
)
from .argument_usage import ARGUMENT_USAGE_CONTRACT_ID, ARGUMENT_USAGE_OWNER_SURFACE
from .npm_surface_policy import (
    PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID,
    PACKAGE_SCRIPT_LOOKUP_OWNER,
)
from .public_bridge_constants import (
    PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
    PUBLIC_BRIDGE_INTEGRITY_OWNER_SURFACE,
)
from .registry_schema_index import (
    ACTION_PAYLOAD_SCHEMA_REF,
    REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
    capability_truth_schema_ids,
)


def registry_capability_truth_fields() -> dict[str, object]:
    return {
        "capability_truth": {
            "scope": "workflow-action-registry",
            "machine_readable": True,
            "owner_surface": "scripts/objc3c_workflow/action_registry_capability_truth.py",
            "schema_owner_surface": REGISTRY_SCHEMA_PAYLOAD_OWNER_SURFACE,
            "owned_fields": ["capability_truth"],
            "schema_ids": capability_truth_schema_ids(),
            "action_payload_schema_ref": ACTION_PAYLOAD_SCHEMA_REF,
            "workflow_surface_contracts": {
                "public_bridge_integrity": {
                    "contract_id": PUBLIC_BRIDGE_INTEGRITY_CONTRACT_ID,
                    "owner_surface": PUBLIC_BRIDGE_INTEGRITY_OWNER_SURFACE,
                },
                "package_script_lookup": {
                    "contract_id": PACKAGE_SCRIPT_LOOKUP_CONTRACT_ID,
                    "owner_surface": PACKAGE_SCRIPT_LOOKUP_OWNER,
                },
                "argument_options": {
                    "contract_id": ARGUMENT_OPTION_CONTRACT_ID,
                    "owner_surface": ARGUMENT_OPTION_OWNER_SURFACE,
                },
                "argument_usage": {
                    "contract_id": ARGUMENT_USAGE_CONTRACT_ID,
                    "owner_surface": ARGUMENT_USAGE_OWNER_SURFACE,
                },
            },
            "retired_surface_claims_allowed": False,
        },
    }
