from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance import (
    resolve_registered_action as owned_resolve_registered_action,
)
from scripts.objc3c_workflow.action_dispatch import resolve_registered_action
from scripts.objc3c_workflow.action_handler_integrity import (
    action_handler_registry_is_complete,
    missing_action_handlers,
    orphan_action_handlers,
)
from scripts.objc3c_workflow.action_handler_groups import ACTION_HANDLER_SECTION_GROUPS
from scripts.objc3c_workflow.action_payloads import describe_action_payload, list_actions_payload
from scripts.objc3c_workflow.action_catalog_groups import ACTION_CATALOG_SECTION_GROUPS
from scripts.objc3c_workflow.argument_parser import parse_workflow_args as parse_workflow_args_impl
from scripts.objc3c_workflow.argument_requests import (
    DescribeActionRequest as OwnedDescribeActionRequest,
)
from scripts.objc3c_workflow.arguments import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowUsageError,
    parse_workflow_args,
)
from scripts.objc3c_workflow.command_result_completion import completed_action
from scripts.objc3c_workflow.path_bootstrap import WORKFLOW_IMPORT_ROOTS
from scripts.objc3c_workflow.paths import ROOT, SCRIPT_ROOT, workflow_import_roots
from scripts.objc3c_workflow.registry_schema_index import (
    ACTION_REGISTRY_SCHEMA_ID,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
    capability_truth_schema_ids,
)
from scripts.objc3c_workflow.registry_lookup import catalog_action_spec as owned_catalog_action_spec
from scripts.objc3c_workflow.registry_store import catalog_action_spec
from scripts.objc3c_workflow.reports import emit_json, write_json_report


def test_workflow_argument_parser_models_public_requests() -> None:
    assert isinstance(parse_workflow_args(["--list-json"]), ListActionsRequest)
    assert parse_workflow_args(["--describe", "lint"]) == DescribeActionRequest("lint")
    assert parse_workflow_args(["--describe-script", "objc3c"]) == DescribePackageScriptRequest("objc3c")
    assert parse_workflow_args(["compile-objc3c", "sample.objc3"]) == ExecuteActionRequest(
        "compile-objc3c",
        ["sample.objc3"],
    )
    assert parse_workflow_args is parse_workflow_args_impl
    assert DescribeActionRequest is OwnedDescribeActionRequest


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

    assert payload["schema_id"] == ACTION_REGISTRY_SCHEMA_ID
    assert payload["package_bridge_count"] == 1
    assert payload["public_action_count"] == payload["action_count"]
    assert payload["internal_action_count"] == 0
    assert describe_action_payload("lint")["runner_path"] == payload["runner_path"]


def test_action_registry_payload_publishes_schema_index_and_capability_truth() -> None:
    payload = list_actions_payload()
    schema_index = payload["schema_index"]
    lint_payload = describe_action_payload("lint")

    assert schema_index["schema_id"] == WORKFLOW_SCHEMA_INDEX_SCHEMA_ID
    assert set(schema_index["capability_truth_schema_ids"]) == set(capability_truth_schema_ids())
    indexed_schema_ids = {entry["schema_id"] for entry in schema_index["schemas"]}
    indexed_payload_surfaces = {
        entry["schema_id"]: entry["payload_surface"] for entry in schema_index["schemas"]
    }
    assert {
        "objc3c-capability-matrix-v1",
        "objc3c-capability-evidence-map-v1",
        ACTION_REGISTRY_SCHEMA_ID,
    }.issubset(indexed_schema_ids)
    assert indexed_payload_surfaces[WORKFLOW_SCHEMA_INDEX_SCHEMA_ID] == (
        "embedded in npm run objc3c -- --list-json"
    )
    assert payload["capability_truth"]["machine_readable"] is True
    assert payload["capability_truth"]["action_payload_schema_ref"] == (
        f"{ACTION_REGISTRY_SCHEMA_ID}#/$defs/action"
    )
    assert lint_payload["payload_schema_ref"] == f"{ACTION_REGISTRY_SCHEMA_ID}#/$defs/action"
    assert lint_payload["registry_schema_id"] == ACTION_REGISTRY_SCHEMA_ID
    assert lint_payload["capability_truth"]["schema_ids"] == capability_truth_schema_ids()


def test_workflow_report_json_helpers_emit_canonical_json(tmp_path, capsys) -> None:
    assert emit_json({"b": 2, "a": 1}) == 0
    assert capsys.readouterr().out == '{\n  "b": 2,\n  "a": 1\n}\n'

    report_path = write_json_report(tmp_path / "report.json", {"b": 2, "a": 1})

    assert report_path.read_text(encoding="utf-8") == '{\n  "b": 2,\n  "a": 1\n}\n'


def test_dispatch_resolution_returns_metadata_without_running_handlers() -> None:
    assert resolve_registered_action is owned_resolve_registered_action
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
    assert len(ACTION_CATALOG_SECTION_GROUPS) == 5
    assert len(ACTION_HANDLER_SECTION_GROUPS) == 5
    assert catalog_action_spec is owned_catalog_action_spec
    assert action_handler_registry_is_complete()
    assert missing_action_handlers() == []
    assert orphan_action_handlers() == []


def test_workflow_path_roots_are_owned_by_package_module() -> None:
    assert workflow_import_roots() == (ROOT, SCRIPT_ROOT)
    assert WORKFLOW_IMPORT_ROOTS == workflow_import_roots()
    assert SCRIPT_ROOT.name == "scripts"
    assert ROOT == SCRIPT_ROOT.parent
