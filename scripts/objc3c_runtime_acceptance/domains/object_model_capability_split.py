"""Public object-model capability split contract for #8154."""

from __future__ import annotations

from typing import Any

OBJECT_MODEL_SPLIT_ISSUE = 8154
OBJECT_MODEL_RESERVED_UMBRELLA_ID = "runtime.object-model.full-realization"
OBJECT_MODEL_CAPABILITY_SPLIT_SOURCE = (
    "scripts/objc3c_runtime_acceptance/domains/object_model_capability_split.py"
)
OBJECT_MODEL_FULL_REALIZATION_READINESS_CONTRACT = (
    "tests/tooling/fixtures/object_model_closure/"
    "full_realization_combined_readiness_contract.json"
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

OBJECT_MODEL_IMPLEMENTED_SUPPORT_CONTRACTS: tuple[dict[str, object], ...] = (
    {
        "contract_id": "objc3c.object-model.public-support.class-metaclass-identity.v1",
        "capability_id": "runtime.object-model.class-realization",
        "support_claim": "objc3c.behavior.runtime.object-model-class-realization",
        "public_command": "npm run objc3c -- validate-object-model-conformance",
        "contract_scope": "class and metaclass identity, root invariants, superclass links, and duplicate metadata rejection",
        "source_truth": (
            "native/objc3c/src/runtime/classes/class_graph.cpp",
            "native/objc3c/src/runtime/classes/metaclass_graph.cpp",
            "native/objc3c/src/runtime/classes/class_graph_snapshots.cpp",
            "native/objc3c/src/runtime/classes/runtime_object_snapshot_contracts.h",
        ),
        "positive_evidence": (
            "tests/native/runtime/object_model/class_realization_contract.objc3",
            "tests/tooling/runtime/class_realization_runtime_probe.cpp",
            "tests/tooling/runtime/metaclass_graph_root_class_probe.cpp",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_class_cases.py",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/class_metaclass_missing_superclass.objc3",
            "tests/tooling/fixtures/native/runtime_export_enforcement_duplicate_interface.objc3",
        ),
    },
    {
        "contract_id": "objc3c.object-model.public-support.category-protocol-registration.v1",
        "capability_id": "runtime.object-model.category-protocol-registration",
        "support_claim": "objc3c.behavior.runtime.object-model-category-protocol-registration",
        "public_command": "npm run objc3c -- test-runtime-acceptance-fast",
        "contract_scope": "category attachment, protocol declaration lookup, inherited protocol conformance, and conflict rejection",
        "source_truth": (
            "native/objc3c/src/runtime/classes/category_attachment.cpp",
            "native/objc3c/src/runtime/classes/protocol_conformance.cpp",
            "native/objc3c/src/runtime/classes/protocol_conformance_snapshots.cpp",
        ),
        "positive_evidence": (
            "tests/native/runtime/object_model/category_protocol_registration_contract.objc3",
            "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
            "tests/tooling/runtime/protocol_category_runtime_probe.cpp",
            "scripts/objc3c_runtime_acceptance/domains/object_model_category_attachment_cases.py",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/category_unknown_class_rejected.objc3",
            "tests/tooling/fixtures/native/execution/negative/duplicate_protocol_runtime_export.objc3",
            "tests/tooling/runtime/protocol_category_invalid_metadata_probe.cpp",
        ),
    },
    {
        "contract_id": "objc3c.object-model.public-support.metadata-layout.v1",
        "capability_id": "runtime.object-model.property-ivar-reflection",
        "support_claim": "objc3c.behavior.runtime.object-model-property-ivar-reflection",
        "public_command": "npm run objc3c -- validate-storage-reflection-conformance",
        "contract_scope": "realized property and ivar layout, inherited storage slots, runtime accessors, and invalid layout rejection",
        "source_truth": (
            "native/objc3c/src/runtime/storage/property_layout_realization.cpp",
            "native/objc3c/src/runtime/storage/property_ivar_layout_index.cpp",
            "native/objc3c/src/runtime/storage/property_lookup.cpp",
            "native/objc3c/src/runtime/reflection/property_snapshot_api.cpp",
        ),
        "positive_evidence": (
            "tests/native/runtime/object_model/property_ivar_reflection_contract.objc3",
            "tests/native/runtime/storage/property_accessor_storage_contract.objc3",
            "tests/tooling/runtime/property_ivar_execution_matrix_probe.cpp",
            "tests/tooling/runtime/runtime_property_metadata_reflection_probe.cpp",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/property_reflection_attribute_compatibility_negative.objc3",
            "tests/tooling/runtime/property_ivar_invalid_layout_probe.cpp",
        ),
    },
    {
        "contract_id": "objc3c.object-model.public-support.runtime-query-truth.v1",
        "capability_id": "runtime.object-model.bounded-query-snapshots",
        "support_claim": "objc3c.behavior.runtime.object-model-bounded-query-snapshots",
        "public_command": "npm run objc3c -- validate-object-model-conformance",
        "contract_scope": "bounded runtime query snapshots, registration replay coherence, and public capability truth for realized object-model state",
        "source_truth": (
            "native/objc3c/src/runtime/classes/object_model_query_snapshot.cpp",
            "native/objc3c/src/runtime/images/registration.cpp",
            "native/objc3c/src/runtime/state/runtime_live_state_reset.cpp",
            "scripts/check_objc3c_runnable_object_model_conformance.py",
        ),
        "positive_evidence": (
            "tests/native/runtime/object_model/bounded_query_snapshot_contract.objc3",
            "tests/native/runtime/object_model/registration_replay_contract.objc3",
            "tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_implementation.py",
            "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_abi.py",
        ),
        "negative_evidence": (
            "tests/tooling/fixtures/native/execution/negative/module_duplicate_declaration.objc3",
            "tests/tooling/runtime/property_ivar_invalid_layout_probe.cpp",
        ),
    },
)

OBJECT_MODEL_RESERVED_BOUNDARIES: tuple[dict[str, str], ...] = (
    {
        "boundary_id": "runtime.object-model.generic-class-abi",
        "public_status": "reserved",
        "matrix_owner": OBJECT_MODEL_RESERVED_UMBRELLA_ID,
        "reason": "Generic class ABI and generic metadata reflection are outside this bounded object-model support slice.",
    },
    {
        "boundary_id": "runtime.object-model.debugger-reflection-closure",
        "public_status": "reserved",
        "matrix_owner": OBJECT_MODEL_RESERVED_UMBRELLA_ID,
        "reason": "The checked runtime query and reflection rows do not claim a full debugger reflection API.",
    },
)

OBJECT_MODEL_FULL_REALIZATION_READINESS_EVIDENCE: tuple[dict[str, object], ...] = (
    {
        "contract_id": "objc3c.object-model.full-realization.combined-readiness.v1",
        "capability_id": OBJECT_MODEL_RESERVED_UMBRELLA_ID,
        "issue": 8198,
        "public_status": "implemented",
        "support_claim_published": True,
        "contract_path": OBJECT_MODEL_FULL_REALIZATION_READINESS_CONTRACT,
        "combined_positive_fixture": "tests/native/runtime/object_model/full_realization_combined_reflection_replay_contract.objc3",
        "public_reflection_probe": "tests/tooling/runtime/public_runtime_reflection_api_probe.cpp",
        "public_commands": (
            "npm run objc3c -- validate-object-model-conformance",
            "npm run objc3c -- validate-public-runtime-reflection-api",
            "npm run objc3c -- validate-object-model-debugger-proof",
        ),
        "covered_axes": (
            "class",
            "metaclass",
            "category",
            "protocol",
            "property",
            "ivar",
            "selector",
            "public-reflection",
            "registration-replay",
        ),
        "remaining_blockers": (),
    },
)


def build_object_model_capability_split_contract() -> dict[str, Any]:
    return {
        "issue": OBJECT_MODEL_SPLIT_ISSUE,
        "reserved_umbrella": OBJECT_MODEL_RESERVED_UMBRELLA_ID,
        "source": OBJECT_MODEL_CAPABILITY_SPLIT_SOURCE,
        "implemented_rows": list(OBJECT_MODEL_PUBLIC_CAPABILITY_ROWS),
        "implemented_support_contracts": list(
            OBJECT_MODEL_IMPLEMENTED_SUPPORT_CONTRACTS
        ),
        "reserved_boundaries": list(OBJECT_MODEL_RESERVED_BOUNDARIES),
        "full_realization_readiness_evidence": list(
            OBJECT_MODEL_FULL_REALIZATION_READINESS_EVIDENCE
        ),
    }


__all__ = [
    "OBJECT_MODEL_CAPABILITY_SPLIT_SOURCE",
    "OBJECT_MODEL_FULL_REALIZATION_READINESS_CONTRACT",
    "OBJECT_MODEL_FULL_REALIZATION_READINESS_EVIDENCE",
    "OBJECT_MODEL_IMPLEMENTED_SUPPORT_CONTRACTS",
    "OBJECT_MODEL_PUBLIC_CAPABILITY_ROWS",
    "OBJECT_MODEL_RESERVED_BOUNDARIES",
    "OBJECT_MODEL_RESERVED_UMBRELLA_ID",
    "OBJECT_MODEL_SPLIT_ISSUE",
    "build_object_model_capability_split_contract",
]
