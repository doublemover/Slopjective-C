from __future__ import annotations

from scripts.objc3c_workflow.argument_option_contracts import (
    ARGUMENT_OPTION_CONTRACT_ID,
    ARGUMENT_OPTION_OWNER_SURFACE,
    DESCRIBE_ACTION_OPTION,
    DESCRIBE_PACKAGE_SCRIPT_OPTION,
    LIST_ACTIONS_OPTION,
    workflow_argument_option,
    workflow_argument_option_usage,
    workflow_argument_options_payload,
)
from scripts.objc3c_workflow.argument_parser import parse_workflow_args
from scripts.objc3c_workflow.argument_request_model import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ListActionsRequest,
)
from scripts.objc3c_workflow.argument_usage import usage_contract_payload, usage_text
from scripts.objc3c_workflow.argument_usage_error import WorkflowUsageError


def test_argument_options_publish_owned_parser_contract() -> None:
    payload = workflow_argument_options_payload()

    assert ARGUMENT_OPTION_CONTRACT_ID == "objc3c-workflow-argument-options-v1"
    assert ARGUMENT_OPTION_OWNER_SURFACE == (
        "scripts/objc3c_workflow/argument_option_contracts.py"
    )
    assert payload["execute_action_passthrough"] is True
    assert payload["retired_option_metadata_allowed"] is False
    assert [option["token"] for option in payload["options"]] == [
        LIST_ACTIONS_OPTION,
        DESCRIBE_ACTION_OPTION,
        DESCRIBE_PACKAGE_SCRIPT_OPTION,
    ]
    assert workflow_argument_option(LIST_ACTIONS_OPTION).required_argument_count == 0
    assert workflow_argument_option(DESCRIBE_ACTION_OPTION).required_argument_count == 1
    assert workflow_argument_option_usage(DESCRIBE_PACKAGE_SCRIPT_OPTION) == (
        "usage: npm run objc3c -- <action> --describe-script <package-script>"
    )


def test_argument_parser_uses_owned_option_contracts() -> None:
    assert parse_workflow_args([LIST_ACTIONS_OPTION]) == ListActionsRequest()
    assert parse_workflow_args([DESCRIBE_ACTION_OPTION, "lint"]) == (
        DescribeActionRequest("lint")
    )
    assert parse_workflow_args([DESCRIBE_PACKAGE_SCRIPT_OPTION, "objc3c"]) == (
        DescribePackageScriptRequest("objc3c")
    )

    try:
        parse_workflow_args([LIST_ACTIONS_OPTION, "extra"])
    except WorkflowUsageError as exc:
        assert exc.message == "usage: npm run objc3c -- <action> --list-json"
    else:
        raise AssertionError("list option arity must fail closed")


def test_argument_usage_text_is_owned_by_option_contracts() -> None:
    payload = usage_contract_payload()

    assert payload["contract_id"] == "objc3c-workflow-argument-usage-v1"
    assert payload["option_contract_id"] == ARGUMENT_OPTION_CONTRACT_ID
    assert payload["retired_option_metadata_allowed"] is False
    assert usage_text() == (
        "usage: npm run objc3c -- <action> <action> [args...]\n"
        "       npm run objc3c -- <action> --list-json\n"
        "       npm run objc3c -- <action> --describe <action>\n"
        "       npm run objc3c -- <action> --describe-script <package-script>"
    )
