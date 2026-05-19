from __future__ import annotations

from scripts.objc3c_workflow.cli_argv import (
    CLI_ARGV_NORMALIZATION_CONTRACT_ID,
    process_argv,
    workflow_argv,
    workflow_argv_contract_payload,
)
from scripts.objc3c_workflow.cli_argv_source import (
    CLI_ARGV_SOURCE_CONTRACT_ID,
    process_argv as owned_process_argv,
    process_argv_contract_payload,
)


def test_cli_argv_facade_uses_process_argv_owner() -> None:
    assert process_argv is owned_process_argv
    assert workflow_argv(["lint", "--dry"]) == ["lint", "--dry"]
    assert process_argv_contract_payload()["contract_id"] == CLI_ARGV_SOURCE_CONTRACT_ID
    contract = workflow_argv_contract_payload()
    assert contract["contract_id"] == CLI_ARGV_NORMALIZATION_CONTRACT_ID
    assert contract["argv_source_contract_id"] == CLI_ARGV_SOURCE_CONTRACT_ID
    assert contract["explicit_argv_is_copied"] is True
    assert contract["retired_passthrough_mutation_allowed"] is False
