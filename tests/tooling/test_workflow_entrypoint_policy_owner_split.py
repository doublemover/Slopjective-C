from __future__ import annotations

from collections.abc import Sequence

from scripts.objc3c_workflow.entrypoint_policy import (
    WORKFLOW_ENTRYPOINT_POLICY_CONTRACT_ID,
    WORKFLOW_ENTRYPOINT_POLICY_OWNER,
    WORKFLOW_MODULE_ENTRYPOINT_OWNER,
    WORKFLOW_SCRIPT_ENTRYPOINT_OWNER,
    dispatch_entrypoint,
    entrypoint_policy_contract_payload,
)


def test_workflow_entrypoint_policy_owns_dispatch_bridge() -> None:
    calls: list[Sequence[str] | None] = []

    def dispatcher(argv: Sequence[str] | None = None) -> int:
        calls.append(argv)
        return 42

    assert WORKFLOW_ENTRYPOINT_POLICY_OWNER == "objc3c-workflow-entrypoint-policy"
    assert WORKFLOW_ENTRYPOINT_POLICY_CONTRACT_ID == (
        "objc3c-workflow-entrypoint-policy-v1"
    )
    assert WORKFLOW_MODULE_ENTRYPOINT_OWNER == "objc3c-workflow-module-entrypoint"
    assert WORKFLOW_SCRIPT_ENTRYPOINT_OWNER == "objc3c-workflow-script-entrypoint"
    contract = entrypoint_policy_contract_payload()
    assert contract["policy_owner"] == WORKFLOW_ENTRYPOINT_POLICY_OWNER
    assert contract["retired_entrypoint_alternates_allowed"] is False
    assert dispatch_entrypoint(dispatcher, ["lint"]) == 42
    assert calls == [["lint"]]
