from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class EvidenceOwner:
    owner_id: str
    source_contract: str
    summary_implementation_anchor: str
    check_implementation_anchors: tuple[str, ...] = ()
    supporting_contracts: tuple[str, ...] = ()


@dataclass(frozen=True)
class EvidenceFamily:
    boundary_inventory: str
    owners: tuple[EvidenceOwner, ...]


EVIDENCE_FAMILIES: dict[str, EvidenceFamily] = {
    "security_hardening": EvidenceFamily(
        boundary_inventory="tests/tooling/fixtures/security_hardening/boundary_inventory.json",
        owners=(
            EvidenceOwner(
                owner_id="security_response_owner",
                source_contract="tests/tooling/fixtures/security_hardening/security_response_disclosure_policy.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/security_hardening/response_drill_contract.json",
                ),
                summary_implementation_anchor="scripts/build_security_hardening_response_policy_summary.py",
                check_implementation_anchors=(
                    "scripts/check_security_hardening_response_drill.py",
                ),
            ),
            EvidenceOwner(
                owner_id="release_key_owner",
                source_contract="tests/tooling/fixtures/security_hardening/installer_update_release_key_hardening_policy.json",
                summary_implementation_anchor="scripts/build_security_hardening_release_key_policy_summary.py",
            ),
            EvidenceOwner(
                owner_id="supply_chain_owner",
                source_contract="tests/tooling/fixtures/security_hardening/macro_package_provenance_trust_policy.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/security_hardening/supply_chain_audit_contract.json",
                    "tests/tooling/fixtures/security_hardening/source_surface.json",
                    "tests/tooling/fixtures/security_hardening/schema_surface.json",
                ),
                summary_implementation_anchor="scripts/build_security_hardening_macro_trust_policy_summary.py",
                check_implementation_anchors=(
                    "scripts/check_security_hardening_supply_chain_audit.py",
                    "scripts/check_security_hardening_source_surface.py",
                    "scripts/check_security_hardening_schema_surface.py",
                ),
            ),
            EvidenceOwner(
                owner_id="runtime_hardening_owner",
                source_contract="tests/tooling/fixtures/security_hardening/runtime_hardening_contract.json",
                summary_implementation_anchor="scripts/build_security_hardening_artifact_contract_summary.py",
                check_implementation_anchors=(
                    "scripts/check_security_hardening_runtime_hardening.py",
                    "scripts/check_objc3c_security_hardening_integration.py",
                    "scripts/check_objc3c_security_hardening_end_to_end.py",
                ),
            ),
        ),
    ),
    "object_model_closure": EvidenceFamily(
        boundary_inventory="tests/tooling/fixtures/object_model_closure/boundary_inventory.json",
        owners=(
            EvidenceOwner(
                owner_id="object_model_semantic_owner",
                source_contract="tests/tooling/fixtures/object_model_closure/realized_object_graph_reflection_semantic_model.json",
                summary_implementation_anchor="scripts/build_object_model_closure_semantic_summary.py",
                check_implementation_anchors=(
                    "scripts/check_object_model_closure_live_property_reflection.py",
                ),
            ),
            EvidenceOwner(
                owner_id="object_model_runtime_owner",
                source_contract="tests/tooling/fixtures/object_model_closure/realized_object_graph_runtime_implementation_contract.json",
                summary_implementation_anchor="scripts/build_object_model_closure_runtime_implementation_summary.py",
                check_implementation_anchors=(
                    "scripts/check_object_model_closure_live_runtime.py",
                    "scripts/check_object_model_closure_realization_lowering.py",
                ),
            ),
            EvidenceOwner(
                owner_id="object_model_artifact_owner",
                source_contract="tests/tooling/fixtures/object_model_closure/object_model_reflection_artifact_runtime_registration_contract.json",
                summary_implementation_anchor="scripts/build_object_model_closure_artifact_registration_summary.py",
                check_implementation_anchors=(
                    "scripts/check_object_model_closure_property_reflection_artifact.py",
                ),
            ),
            EvidenceOwner(
                owner_id="object_model_workload_owner",
                source_contract="tests/tooling/fixtures/object_model_closure/loader_category_protocol_workload_map.json",
                summary_implementation_anchor="scripts/build_object_model_closure_workload_summary.py",
            ),
            EvidenceOwner(
                owner_id="object_model_executable_proof_owner",
                source_contract="tests/tooling/fixtures/object_model_closure/executable_proof_abi_contract.json",
                summary_implementation_anchor="scripts/build_object_model_closure_executable_proof_summary.py",
            ),
        ),
    ),
    "runtime_corrective": EvidenceFamily(
        boundary_inventory="tests/tooling/fixtures/runtime_corrective/boundary_inventory.json",
        owners=(
            EvidenceOwner(
                owner_id="runtime_corrective_dispatch_owner",
                source_contract="tests/tooling/fixtures/runtime_corrective/realized_dispatch_semantic_model.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/runtime_corrective/dispatch_lowering_implementation_contract.json",
                ),
                summary_implementation_anchor="scripts/build_runtime_corrective_dispatch_summary.py",
                check_implementation_anchors=(
                    "scripts/check_runtime_corrective_live_dispatch_runtime.py",
                    "scripts/check_runtime_corrective_dispatch_lowering.py",
                ),
            ),
            EvidenceOwner(
                owner_id="runtime_corrective_native_output_owner",
                source_contract="tests/tooling/fixtures/runtime_corrective/native_output_truth_policy.json",
                summary_implementation_anchor="scripts/build_runtime_corrective_native_output_truth_summary.py",
            ),
            EvidenceOwner(
                owner_id="runtime_corrective_lowering_provenance_owner",
                source_contract="tests/tooling/fixtures/runtime_corrective/lowering_provenance_artifact_contract.json",
                summary_implementation_anchor="scripts/build_runtime_corrective_lowering_provenance_summary.py",
                check_implementation_anchors=(
                    "scripts/check_runtime_corrective_dispatch_lowering.py",
                    "scripts/check_runtime_corrective_synthesized_accessor_lowering.py",
                ),
            ),
            EvidenceOwner(
                owner_id="runtime_corrective_accessor_owner",
                source_contract="tests/tooling/fixtures/runtime_corrective/synthesized_accessor_semantic_model.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/runtime_corrective/synthesized_accessor_lowering_implementation_contract.json",
                ),
                summary_implementation_anchor="scripts/build_runtime_corrective_synthesized_accessor_summary.py",
                check_implementation_anchors=(
                    "scripts/check_runtime_corrective_synthesized_accessor_runtime.py",
                    "scripts/check_runtime_corrective_synthesized_accessor_lowering.py",
                ),
            ),
            EvidenceOwner(
                owner_id="runtime_corrective_executable_proof_owner",
                source_contract="tests/tooling/fixtures/runtime_corrective/executable_proof_abi_contract.json",
                summary_implementation_anchor="scripts/build_runtime_corrective_executable_proof_summary.py",
                check_implementation_anchors=(
                    "scripts/check_runtime_corrective_closeout_gate.py",
                ),
            ),
        ),
    ),
    "metaprogramming_interop_closure": EvidenceFamily(
        boundary_inventory="tests/tooling/fixtures/metaprogramming_interop_closure/boundary_inventory.json",
        owners=(
            EvidenceOwner(
                owner_id="metaprogramming_runtime_owner",
                source_contract="tests/tooling/fixtures/metaprogramming_interop_closure/metaprogramming_runtime_semantic_model.json",
                summary_implementation_anchor="scripts/build_metaprogramming_interop_closure_semantic_summary.py",
                check_implementation_anchors=(
                    "scripts/check_metaprogramming_interop_closure_live_metaprogramming_runtime.py",
                ),
            ),
            EvidenceOwner(
                owner_id="property_behavior_owner",
                source_contract="tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_runtime_materialization_policy.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/metaprogramming_interop_closure/property_behavior_macro_runtime_materialization_implementation_contract.json",
                ),
                summary_implementation_anchor="scripts/build_metaprogramming_interop_closure_property_behavior_policy_summary.py",
            ),
            EvidenceOwner(
                owner_id="interop_runtime_owner",
                source_contract="tests/tooling/fixtures/metaprogramming_interop_closure/interop_runtime_ownership_abi_policy.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/metaprogramming_interop_closure/interop_bridge_cross_module_artifact_implementation_contract.json",
                ),
                summary_implementation_anchor="scripts/build_metaprogramming_interop_closure_interop_policy_summary.py",
                check_implementation_anchors=(
                    "scripts/check_metaprogramming_interop_closure_live_interop_runtime.py",
                    "scripts/check_metaprogramming_interop_closure_interop_lowering.py",
                ),
            ),
            EvidenceOwner(
                owner_id="metaprogramming_lowering_owner",
                source_contract="tests/tooling/fixtures/metaprogramming_interop_closure/lowering_runtime_artifact_contract.json",
                summary_implementation_anchor="scripts/build_metaprogramming_interop_closure_artifact_summary.py",
                check_implementation_anchors=(
                    "scripts/check_metaprogramming_interop_closure_metaprogramming_lowering.py",
                ),
            ),
            EvidenceOwner(
                owner_id="runtime_abi_packaged_proof_owner",
                source_contract="tests/tooling/fixtures/metaprogramming_interop_closure/executable_proof_abi_contract.json",
                supporting_contracts=(
                    "tests/tooling/fixtures/metaprogramming_interop_closure/packaged_interop_proof_contract.json",
                ),
                summary_implementation_anchor="scripts/build_metaprogramming_interop_closure_executable_proof_summary.py",
                check_implementation_anchors=(
                    "scripts/check_metaprogramming_interop_closure_packaged_interop_proof.py",
                ),
            ),
        ),
    ),
}

HARD_CUTOVER_SOURCE_OWNER_CONTRACT = {
    "source_owned": True,
    "report_only_allowed": False,
    "fallback_claims_allowed": False,
    "generated_report_claims_allowed": False,
}


def owner_contract_ids(family_name: str) -> tuple[str, ...]:
    return tuple(owner.owner_id for owner in EVIDENCE_FAMILIES[family_name].owners)


def owner_contract_count(family_name: str) -> int:
    return len(EVIDENCE_FAMILIES[family_name].owners)


def validate_boundary_owner_contracts(
    family_name: str,
    contract: dict[str, Any],
    root: Path,
) -> dict[str, bool]:
    family = EVIDENCE_FAMILIES[family_name]
    owner_contracts = contract.get("source_owner_contracts", {})
    expected_owner_ids = owner_contract_ids(family_name)

    checks = {
        "hard_cutover_source_owner_contract_matches": contract.get("hard_cutover_source_owner_contract")
        == HARD_CUTOVER_SOURCE_OWNER_CONTRACT,
        "source_owner_contract_ids_match": sorted(owner_contracts) == sorted(expected_owner_ids)
        if isinstance(owner_contracts, dict)
        else False,
    }

    if not isinstance(owner_contracts, dict):
        return checks

    for owner in family.owners:
        entry = owner_contracts.get(owner.owner_id, {})
        checks[f"{owner.owner_id}_source_contract_matches"] = (
            entry.get("source_contract") == owner.source_contract
        )
        checks[f"{owner.owner_id}_source_contract_exists"] = (
            root / owner.source_contract
        ).is_file()
        checks[f"{owner.owner_id}_summary_anchor_matches"] = (
            entry.get("summary_implementation_anchor")
            == owner.summary_implementation_anchor
        )
        checks[f"{owner.owner_id}_summary_anchor_exists"] = (
            root / owner.summary_implementation_anchor
        ).is_file()
        checks[f"{owner.owner_id}_check_anchors_match"] = tuple(
            entry.get("check_implementation_anchors", ())
        ) == owner.check_implementation_anchors
        checks[f"{owner.owner_id}_check_anchors_exist"] = all(
            (root / path).is_file() for path in owner.check_implementation_anchors
        )
        checks[f"{owner.owner_id}_supporting_contracts_match"] = tuple(
            entry.get("supporting_contracts", ())
        ) == owner.supporting_contracts
        checks[f"{owner.owner_id}_supporting_contracts_exist"] = all(
            (root / path).is_file() for path in owner.supporting_contracts
        )
        checks[f"{owner.owner_id}_report_only_disallowed"] = (
            entry.get("report_only_allowed") is False
        )
        checks[f"{owner.owner_id}_fallback_claims_disallowed"] = (
            entry.get("fallback_claims_allowed") is False
        )
        checks[f"{owner.owner_id}_generated_report_claims_disallowed"] = (
            entry.get("generated_report_claims_allowed") is False
        )

    return checks
