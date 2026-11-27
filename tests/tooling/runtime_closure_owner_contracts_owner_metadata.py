from __future__ import annotations

from scripts.runtime_closure_owner_contracts import (
    REQUIRED_CLOSURE_PUBLICATION_CONTRACT,
    REQUIRED_OWNER_POLICY,
    REQUIRED_OWNER_ROLES,
    runtime_closure_owner_summary,
)

from runtime_closure_owner_contracts_support import runtime_closure_owner_fixtures


def assert_runtime_closure_owner_metadata(owner_contract: dict) -> None:
    assert REQUIRED_OWNER_POLICY.items() <= owner_contract["owner_policy"].items()
    assert owner_contract["owner_policy"]["behavior_owner_split_required"] is True
    assert (
        owner_contract["closure_publication_contract"]
        == REQUIRED_CLOSURE_PUBLICATION_CONTRACT
    )
    assert set(owner_contract["role_contracts"]) == {*REQUIRED_OWNER_ROLES, "provenance_owner"}


def runtime_closure_owner_summary_keeps_publication_policy_visible() -> None:
    for _, _, owner_contract, _ in runtime_closure_owner_fixtures():
        summary = runtime_closure_owner_summary(owner_contract)

        assert summary["owner_roles"] == sorted([*REQUIRED_OWNER_ROLES, "provenance_owner"])
        assert summary["owner_role_count"] == len(REQUIRED_OWNER_ROLES) + 1
        assert summary["evidence_log_allowed"] is False
        assert summary["retired_route_allowed"] is False
        assert summary["missing_artifact_behavior"] == "fail-closed"
        assert (
            summary["claim_publication_mode"]
            == "checked-in-owner-contract-plus-executable-proof"
        )
        assert summary["wrapper_only_runnable_actions_allowed"] is False
