from __future__ import annotations

from scripts.objc3c_workflow.cli_dispatch import (
    CLI_DISPATCH_CONTRACT_ID,
    cli_dispatch_contract_payload,
)


def test_cli_dispatch_publishes_owner_contract() -> None:
    contract = cli_dispatch_contract_payload()

    assert CLI_DISPATCH_CONTRACT_ID == "objc3c-workflow-cli-dispatch-v1"
    assert contract["contract_id"] == CLI_DISPATCH_CONTRACT_ID
    assert contract["argv_contract_id"] == "objc3c-workflow-cli-argv-normalization-v1"
    assert contract["dispatches_normalized_argv_only"] is True
    assert contract["retired_direct_action_dispatch_allowed"] is False
