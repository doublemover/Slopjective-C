from __future__ import annotations

from runtime_runnable_action_support import (
    RUNTIME_CLOSURE_CLAIM_KIND,
    RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES,
    RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS,
    RUNTIME_CLOSURE_OWNER_METADATA,
    RUNTIME_CLOSURE_PUBLICATION_MODE,
    runtime_closure_forbidden_claim_contracts,
    runtime_runnable_conformance,
    runtime_runnable_e2e,
)


def test_runtime_closure_runnable_actions_publish_owner_contract_metadata() -> None:
    groups = {
        group.action: group
        for group in (
            *runtime_runnable_conformance.RUNTIME_RUNNABLE_CONFORMANCE_ACTION_GROUPS,
            *runtime_runnable_e2e.RUNTIME_RUNNABLE_E2E_ACTION_GROUPS,
        )
    }

    for action, (
        owner_contract,
        boundary_inventory,
        executable_proof_contract,
    ) in RUNTIME_CLOSURE_OWNER_METADATA.items():
        group = groups[action]

        assert group.owner_contract == owner_contract
        assert group.boundary_inventory == boundary_inventory
        assert group.executable_proof_contract == executable_proof_contract
        assert group.claim_kind == RUNTIME_CLOSURE_CLAIM_KIND
        assert group.claim_publication_mode == RUNTIME_CLOSURE_PUBLICATION_MODE
        assert set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES).issubset(
            group.forbidden_claim_shapes
        )
        assert set(RUNTIME_CLOSURE_HARD_CUTOVER_REQUIREMENTS).issubset(
            group.hard_cutover_requirements
        )


def test_runtime_closure_forbidden_claim_contracts_are_owner_typed() -> None:
    contracts = runtime_closure_forbidden_claim_contracts()
    by_shape = {contract["shape"]: contract for contract in contracts}

    assert set(by_shape) == set(RUNTIME_CLOSURE_FORBIDDEN_CLAIM_SHAPES)
    assert by_shape["fallback-runtime-behavior"]["policy_field"] == (
        "fallback_runtime_semantics_allowed"
    )
    assert by_shape["report-only-runtime-closure"]["policy_field"] == (
        "report_only_executable_proof_claims_allowed"
    )
    assert by_shape["compatibility-shim-runtime-closure"]["policy_field"] == (
        "compatibility_runtime_semantics_allowed"
    )
    assert by_shape["wrapper-only-runnable-action"]["policy_field"] == (
        "wrapper_only_runnable_actions_allowed"
    )
    assert by_shape["public-runtime-abi-widening-without-source-owner"][
        "policy_field"
    ] == "public_claims_require_executable_proof"
    assert by_shape["generated-report-only-source-truth"]["policy_field"] == (
        "generated_reports_are_source"
    )
    for contract in contracts:
        assert contract["owner"] == "runtime-closure-owner-contract"
        assert contract["failure_mode"] == "fail-closed"
