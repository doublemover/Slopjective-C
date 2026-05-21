"""Public object-model capability split contract for #8154."""

from __future__ import annotations

from typing import Any

OBJECT_MODEL_SPLIT_ISSUE = 8154
OBJECT_MODEL_RESERVED_UMBRELLA_ID = "runtime.object-model.full-realization"
OBJECT_MODEL_CAPABILITY_SPLIT_SOURCE = (
    "scripts/objc3c_runtime_acceptance/domains/object_model_capability_split.py"
)

OBJECT_MODEL_PUBLIC_CAPABILITY_ROWS: tuple[dict[str, str], ...] = (
    {
        "capability_id": "runtime.object-model.interface-method-table",
        "support_claim": "objc3c.behavior.runtime.object-model-interface-method-table",
        "behavior_fixture": "tests/native/runtime/object_model/interface_method_table_contract.objc3",
    },
    {
        "capability_id": "runtime.object-model.class-realization",
        "support_claim": "objc3c.behavior.runtime.object-model-class-realization",
        "behavior_fixture": "tests/native/runtime/object_model/class_realization_contract.objc3",
    },
    {
        "capability_id": "runtime.object-model.category-protocol-registration",
        "support_claim": "objc3c.behavior.runtime.object-model-category-protocol-registration",
        "behavior_fixture": "tests/native/runtime/object_model/category_protocol_registration_contract.objc3",
    },
    {
        "capability_id": "runtime.object-model.property-ivar-reflection",
        "support_claim": "objc3c.behavior.runtime.object-model-property-ivar-reflection",
        "behavior_fixture": "tests/native/runtime/object_model/property_ivar_reflection_contract.objc3",
    },
    {
        "capability_id": "runtime.object-model.registration-replay",
        "support_claim": "objc3c.behavior.runtime.object-model-registration-replay",
        "behavior_fixture": "tests/native/runtime/object_model/registration_replay_contract.objc3",
    },
    {
        "capability_id": "runtime.object-model.bounded-query-snapshots",
        "support_claim": "objc3c.behavior.runtime.object-model-bounded-query-snapshots",
        "behavior_fixture": "tests/native/runtime/object_model/bounded_query_snapshot_contract.objc3",
    },
    {
        "capability_id": "runtime.public-api.reflection",
        "support_claim": "objc3c.behavior.runtime.public-reflection-api",
        "behavior_fixture": "tests/tooling/fixtures/objc3c/public_runtime_reflection_api_contract.json",
    },
)


def build_object_model_capability_split_contract() -> dict[str, Any]:
    return {
        "issue": OBJECT_MODEL_SPLIT_ISSUE,
        "reserved_umbrella": OBJECT_MODEL_RESERVED_UMBRELLA_ID,
        "source": OBJECT_MODEL_CAPABILITY_SPLIT_SOURCE,
        "implemented_rows": list(OBJECT_MODEL_PUBLIC_CAPABILITY_ROWS),
    }


__all__ = [
    "OBJECT_MODEL_CAPABILITY_SPLIT_SOURCE",
    "OBJECT_MODEL_PUBLIC_CAPABILITY_ROWS",
    "OBJECT_MODEL_RESERVED_UMBRELLA_ID",
    "OBJECT_MODEL_SPLIT_ISSUE",
    "build_object_model_capability_split_contract",
]
