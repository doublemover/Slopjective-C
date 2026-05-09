from __future__ import annotations

from scripts.objc3c_workflow.action_dispatch import resolve_registered_action
from scripts.objc3c_workflow.action_integrity import (
    action_handler_registry_is_complete,
    missing_action_handlers,
    orphan_action_handlers,
)
from scripts.objc3c_workflow.action_payloads import describe_action_payload, list_actions_payload
from scripts.objc3c_workflow.arguments import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowUsageError,
    parse_workflow_args,
)
from scripts.objc3c_workflow.command_result import completed_action
from scripts.objc3c_workflow.paths import ROOT, SCRIPT_ROOT, workflow_import_roots


def test_workflow_argument_parser_models_public_requests() -> None:
    assert isinstance(parse_workflow_args(["--list-json"]), ListActionsRequest)
    assert parse_workflow_args(["--describe", "lint"]) == DescribeActionRequest("lint")
    assert parse_workflow_args(["--describe-script", "objc3c"]) == DescribePackageScriptRequest("objc3c")
    assert parse_workflow_args(["compile-objc3c", "sample.objc3"]) == ExecuteActionRequest(
        "compile-objc3c",
        ["sample.objc3"],
    )


def test_workflow_argument_parser_reports_usage_without_dispatching() -> None:
    try:
        parse_workflow_args([])
    except WorkflowUsageError as exc:
        assert "npm run objc3c -- <action>" in exc.message
        assert exc.exit_code == 2
    else:
        raise AssertionError("empty workflow arguments should fail before dispatch")


def test_action_payloads_keep_single_public_package_bridge() -> None:
    payload = list_actions_payload()

    assert payload["package_bridge_count"] == 1
    assert payload["public_action_count"] == payload["action_count"]
    assert payload["internal_action_count"] == 0
    assert describe_action_payload("lint")["runner_path"] == payload["runner_path"]


def test_dispatch_resolution_returns_metadata_without_running_handlers() -> None:
    accepted = resolve_registered_action("compile-objc3c", ["sample.objc3"])
    rejected = resolve_registered_action("lint", ["unexpected"])
    unknown = resolve_registered_action("missing-action", [])
    completed = completed_action("lint", 0, 0)

    assert accepted.accepted is True
    assert accepted.pass_through_arg_count == 1
    assert rejected.accepted is False
    assert rejected.status == "extra-arguments-rejected"
    assert unknown.status == "unknown-action"
    assert completed.to_payload()["status"] == "completed"


def test_handler_registry_matches_action_catalog() -> None:
    assert action_handler_registry_is_complete()
    assert missing_action_handlers() == []
    assert orphan_action_handlers() == []


def test_workflow_path_roots_are_owned_by_package_module() -> None:
    assert workflow_import_roots() == (ROOT, SCRIPT_ROOT)
    assert SCRIPT_ROOT.name == "scripts"
    assert ROOT == SCRIPT_ROOT.parent
