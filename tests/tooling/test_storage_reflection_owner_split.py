from __future__ import annotations

from pathlib import Path

import pytest

from scripts.objc3c_runtime_acceptance.domains import storage_reflection
from scripts.objc3c_runtime_acceptance.domains.storage_reflection_owner_contracts import (
    CROSS_MODULE_ARTIFACT_OWNER,
    IVAR_LAYOUT_ACCESSOR_OWNER,
    LOWERING_METADATA_OWNER,
    OWNERSHIP_ARC_OWNER,
    RUNTIME_PROPERTY_EXECUTION_OWNER,
    RUNTIME_PROPERTY_REFLECTION_OWNER,
    SEMANTIC_SYNTHESIS_OWNER,
    STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS,
    STORAGE_REFLECTION_OWNER_CASE_IDS,
    STORAGE_REFLECTION_OWNER_CONTRACT_ID,
    STORAGE_REFLECTION_REQUIRED_OWNER_IDS,
    STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID,
    STRICT_STATUS_OWNER,
    storage_reflection_case_owner_payload,
    storage_reflection_case_summary,
    storage_reflection_surface_owner_payload,
)
from scripts.objc3c_workflow.actions.runtime_runnable_storage_reflection import (
    RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION,
    RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_GUARANTEE_OWNER,
    RUNNABLE_STORAGE_REFLECTION_E2E_ACTION,
    RUNNABLE_STORAGE_REFLECTION_E2E_GUARANTEE_OWNER,
)


ROOT = Path(__file__).resolve().parents[2]


def test_storage_reflection_owner_contracts_cover_required_surfaces() -> None:
    assert STORAGE_REFLECTION_REQUIRED_OWNER_IDS == (
        SEMANTIC_SYNTHESIS_OWNER,
        LOWERING_METADATA_OWNER,
        IVAR_LAYOUT_ACCESSOR_OWNER,
        RUNTIME_PROPERTY_REFLECTION_OWNER,
        RUNTIME_PROPERTY_EXECUTION_OWNER,
        OWNERSHIP_ARC_OWNER,
        CROSS_MODULE_ARTIFACT_OWNER,
        STRICT_STATUS_OWNER,
    )

    surface_owner = storage_reflection_surface_owner_payload()

    assert surface_owner["contract_id"] == STORAGE_REFLECTION_OWNER_CONTRACT_ID
    assert surface_owner["owner_ids"] == list(STORAGE_REFLECTION_REQUIRED_OWNER_IDS)
    assert surface_owner["strict_status_owner"]["contract_id"] == (
        STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID
    )
    assert surface_owner["strict_status_owner"]["case_ids"] == list(
        STORAGE_REFLECTION_OWNER_CASE_IDS
    )


def test_storage_reflection_case_summaries_are_source_owned() -> None:
    for case_id in STORAGE_REFLECTION_OWNER_CASE_IDS:
        owner_payload = storage_reflection_case_owner_payload(case_id)
        summary = storage_reflection_case_summary(case_id, {"evidence": "source"})

        assert owner_payload["contract_id"] == STORAGE_REFLECTION_OWNER_CONTRACT_ID
        assert owner_payload["strict_status_owner"]["owner_id"] == STRICT_STATUS_OWNER
        assert summary["owner_contract"] == owner_payload
        assert summary["evidence"] == "source"

    with pytest.raises(ValueError, match="unowned storage/reflection"):
        storage_reflection_case_owner_payload("storage-reflection-report-shell")


def test_storage_reflection_direct_factory_inventory_excludes_cross_module_route() -> None:
    assert "cross-module-storage-reflection-artifact-preservation" in (
        STORAGE_REFLECTION_OWNER_CASE_IDS
    )
    assert "cross-module-storage-reflection-artifact-preservation" not in (
        STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS
    )
    assert "synthesized-accessor-codegen" in STORAGE_REFLECTION_DIRECT_FACTORY_CASE_IDS

    codegen_case = (
        ROOT
        / "scripts"
        / "objc3c_runtime_acceptance"
        / "domains"
        / "storage_reflection_lowering_layout_codegen_case.py"
    ).read_text(encoding="utf-8")
    assert 'case_id=SYNTHESIZED_ACCESSOR_CODEGEN_CASE_ID' in codegen_case
    assert 'case_id="property-codegen"' not in codegen_case


def test_storage_reflection_domain_exports_owner_contract_helpers() -> None:
    exported = set(storage_reflection.__all__)

    assert "storage_reflection_case_owner_payload" in exported
    assert "storage_reflection_case_summary" in exported
    assert "storage_reflection_surface_owner_payload" in exported
    assert "storage_reflection_strict_status_owner_payload" in exported


def test_storage_reflection_workflow_actions_name_strict_status_owner() -> None:
    for action_group, guarantee_owner in (
        (
            RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_ACTION,
            RUNNABLE_STORAGE_REFLECTION_CONFORMANCE_GUARANTEE_OWNER,
        ),
        (
            RUNNABLE_STORAGE_REFLECTION_E2E_ACTION,
            RUNNABLE_STORAGE_REFLECTION_E2E_GUARANTEE_OWNER,
        ),
    ):
        assert action_group.guarantee_owner == guarantee_owner
        assert STORAGE_REFLECTION_OWNER_CONTRACT_ID in guarantee_owner
        assert STORAGE_REFLECTION_STRICT_STATUS_CONTRACT_ID in guarantee_owner
        assert STRICT_STATUS_OWNER in guarantee_owner
