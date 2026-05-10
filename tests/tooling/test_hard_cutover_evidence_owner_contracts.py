from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_evidence_owner_contracts import (
    EVIDENCE_FAMILIES,
    HARD_CUTOVER_SOURCE_OWNER_CONTRACT,
    owner_contract_ids,
    validate_boundary_owner_contracts,
)


ROOT = Path(__file__).resolve().parents[2]


def _load_json(relative_path: str) -> dict[str, object]:
    return json.loads((ROOT / relative_path).read_text(encoding="utf-8"))


def test_evidence_families_publish_source_owned_hard_cutover_contracts() -> None:
    expected_owner_ids = {
        "security_hardening": {
            "security_response_owner",
            "release_key_owner",
            "supply_chain_owner",
            "runtime_hardening_owner",
        },
        "object_model_closure": {
            "object_model_semantic_owner",
            "object_model_runtime_owner",
            "object_model_artifact_owner",
            "object_model_workload_owner",
            "object_model_executable_proof_owner",
        },
        "runtime_corrective": {
            "runtime_corrective_dispatch_owner",
            "runtime_corrective_native_output_owner",
            "runtime_corrective_lowering_provenance_owner",
            "runtime_corrective_accessor_owner",
            "runtime_corrective_executable_proof_owner",
        },
        "metaprogramming_interop_closure": {
            "metaprogramming_runtime_owner",
            "property_behavior_owner",
            "interop_runtime_owner",
            "metaprogramming_lowering_owner",
            "runtime_abi_packaged_proof_owner",
        },
    }

    for family_name, family in EVIDENCE_FAMILIES.items():
        boundary = _load_json(family.boundary_inventory)
        owner_contracts = boundary["source_owner_contracts"]

        assert boundary["hard_cutover_source_owner_contract"] == HARD_CUTOVER_SOURCE_OWNER_CONTRACT
        assert set(owner_contract_ids(family_name)) == expected_owner_ids[family_name]
        assert set(owner_contracts) == expected_owner_ids[family_name]
        assert all(validate_boundary_owner_contracts(family_name, boundary, ROOT).values())

        for owner_id in expected_owner_ids[family_name]:
            owner_contract = owner_contracts[owner_id]
            assert owner_contract["evidence_log_allowed"] is False
            assert owner_contract["fallback_claims_allowed"] is False
            assert owner_contract["generated_report_claims_allowed"] is False


def test_boundary_summary_builders_publish_owner_contracts_not_legacy_summary_keys() -> None:
    builder_paths = [
        "scripts/build_security_hardening_boundary_inventory_summary.py",
        "scripts/build_object_model_closure_boundary_inventory_summary.py",
        "scripts/build_runtime_corrective_boundary_inventory_summary.py",
        "scripts/build_metaprogramming_interop_closure_boundary_inventory_summary.py",
    ]

    for relative_path in builder_paths:
        source = (ROOT / relative_path).read_text(encoding="utf-8")
        assert "validate_boundary_owner_contracts(" in source
        assert "owner_contract_ids(" in source
        assert "owner_contract_count(" in source
        assert 'contract["summary_implementation_anchor"]' in source
        assert 'contract["summary_script"]' not in source


def test_security_hardening_action_surface_remains_owner_facade() -> None:
    facade = (
        ROOT
        / "scripts"
        / "objc3c_workflow"
        / "actions"
        / "release_governance_security_hardening.py"
    ).read_text(encoding="utf-8")

    assert "release_governance_security_artifacts" in facade
    assert "release_governance_security_targets" in facade
    assert "release_governance_security_validation" in facade
    assert "schema_surfaces" in facade
    assert "run(" not in facade
    assert "run_composite_validation" not in facade
    assert "python_script_command" not in facade
