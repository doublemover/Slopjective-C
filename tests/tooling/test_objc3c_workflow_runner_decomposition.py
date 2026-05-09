from __future__ import annotations

from scripts.objc3c_workflow.action_acceptance import (
    resolve_registered_action as owned_resolve_registered_action,
)
from scripts.objc3c_workflow.action_audience import action_audience
from scripts.objc3c_workflow.action_audience_rules import action_audience as owned_action_audience
from scripts.objc3c_workflow.action_dispatch import resolve_registered_action
from scripts.objc3c_workflow.action_handler_integrity import (
    action_handler_registry_is_complete,
    missing_action_handlers,
    orphan_action_handlers,
)
from scripts.objc3c_workflow.action_handler_groups import ACTION_HANDLER_SECTION_GROUPS
from scripts.objc3c_workflow.action_handlers_distribution_credibility import (
    DISTRIBUTION_CREDIBILITY_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_packaging_channels import (
    PACKAGING_CHANNEL_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_release_foundation import (
    RELEASE_FOUNDATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_release_operations import (
    RELEASE_OPERATIONS_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_reporting_release_channels import (
    REPORTING_RELEASE_CHANNEL_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_acceptance import (
    RUNTIME_ACCEPTANCE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_architecture import (
    RUNTIME_ARCHITECTURE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_runnable_conformance import (
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_handlers_runtime_validation import (
    RUNTIME_VALIDATION_ACTION_HANDLERS,
)
from scripts.objc3c_workflow.action_payload_builder import build_action_payload as owned_build_action_payload
from scripts.objc3c_workflow.action_payload_description import (
    describe_action_payload as owned_describe_action_payload,
)
from scripts.objc3c_workflow.action_payload_enrichment import enrich_action_payload
from scripts.objc3c_workflow.action_payload_fields import build_action_payload
from scripts.objc3c_workflow.action_payload_listing import (
    list_actions_payload as owned_list_actions_payload,
)
from scripts.objc3c_workflow.action_payloads import describe_action_payload, list_actions_payload
from scripts.objc3c_workflow.action_catalog_distribution_credibility import (
    DISTRIBUTION_CREDIBILITY_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_groups import ACTION_CATALOG_SECTION_GROUPS
from scripts.objc3c_workflow.action_catalog_packaging_channels import PACKAGING_CHANNEL_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_release_channels import RELEASE_CHANNEL_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_release_foundation import RELEASE_FOUNDATION_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_release_operations import RELEASE_OPERATIONS_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_runtime_acceptance import RUNTIME_ACCEPTANCE_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_runtime_architecture import RUNTIME_ARCHITECTURE_ACTION_SPECS
from scripts.objc3c_workflow.action_catalog_runtime_runnable_conformance import (
    RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_runtime_runnable_e2e import (
    RUNTIME_RUNNABLE_E2E_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_runtime_validation import RUNTIME_VALIDATION_ACTION_SPECS
from scripts.objc3c_workflow.argument_parser import parse_workflow_args as parse_workflow_args_impl
from scripts.objc3c_workflow.argument_request_model import (
    DescribeActionRequest as ModelDescribeActionRequest,
)
from scripts.objc3c_workflow.argument_requests import (
    DescribeActionRequest as OwnedDescribeActionRequest,
)
from scripts.objc3c_workflow.argument_usage_error import WorkflowUsageError as OwnedWorkflowUsageError
from scripts.objc3c_workflow.arguments import (
    DescribeActionRequest,
    DescribePackageScriptRequest,
    ExecuteActionRequest,
    ListActionsRequest,
    WorkflowUsageError,
    parse_workflow_args,
)
from scripts.objc3c_workflow.command_result_completion import completed_action
from scripts.objc3c_workflow.entrypoint_module import main as module_entrypoint_main
from scripts.objc3c_workflow.entrypoint_script import main as script_entrypoint_main
from scripts.objc3c_workflow.environment import MARKDOWN_GLOBS
from scripts.objc3c_workflow.environment_markdown import MARKDOWN_GLOBS as OWNED_MARKDOWN_GLOBS
from scripts.objc3c_workflow.path_bootstrap import WORKFLOW_IMPORT_ROOTS
from scripts.objc3c_workflow.paths import ROOT, SCRIPT_ROOT, workflow_import_roots
from scripts.objc3c_workflow.registry_schema_index import (
    ACTION_REGISTRY_SCHEMA_ID,
    WORKFLOW_SCHEMA_INDEX_SCHEMA_ID,
    capability_truth_schema_ids,
)
from scripts.objc3c_workflow.registry_lookup import catalog_action_spec as owned_catalog_action_spec
from scripts.objc3c_workflow.registry_store import catalog_action_spec
from scripts.objc3c_workflow.report_output import emit_json as owned_emit_json
from scripts.objc3c_workflow.report_rendering import (
    render_report_json as owned_render_report_json,
)
from scripts.objc3c_workflow.reports import emit_json, render_report_json, write_json_report
from scripts.objc3c_workflow.request_handler_dispatch import (
    dispatch_parsed_workflow_request as owned_dispatch_parsed_workflow_request,
)
from scripts.objc3c_workflow.request_handlers import dispatch_parsed_workflow_request
from scripts.objc3c_workflow.request_parse_dispatch import (
    parse_and_dispatch_workflow_request as owned_parse_and_dispatch_workflow_request,
)
from scripts.objc3c_workflow.request_dispatch import (
    parse_and_dispatch_workflow_request,
)


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
    assert DescribeActionRequest is ModelDescribeActionRequest
    assert WorkflowUsageError is OwnedWorkflowUsageError
    assert dispatch_parsed_workflow_request is owned_dispatch_parsed_workflow_request
    assert parse_and_dispatch_workflow_request is owned_parse_and_dispatch_workflow_request


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

    assert action_audience is owned_action_audience
    assert build_action_payload is owned_build_action_payload
    assert list_actions_payload is owned_list_actions_payload
    assert describe_action_payload is owned_describe_action_payload
    assert describe_action_payload("lint") == enrich_action_payload(
        catalog_action_spec("lint")
    )
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
    assert emit_json is owned_emit_json
    assert render_report_json is owned_render_report_json
    assert render_report_json({"b": 2, "a": 1}) == (
        '{\n  "b": 2,\n  "a": 1\n}\n'
    )
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


def test_release_channel_catalog_aggregates_owner_catalogs() -> None:
    owner_catalogs = (
        RELEASE_FOUNDATION_ACTION_SPECS,
        PACKAGING_CHANNEL_ACTION_SPECS,
        RELEASE_OPERATIONS_ACTION_SPECS,
        DISTRIBUTION_CREDIBILITY_ACTION_SPECS,
    )
    owner_actions = set().union(*(catalog.keys() for catalog in owner_catalogs))

    assert owner_actions == set(RELEASE_CHANNEL_ACTION_SPECS)
    for catalog in owner_catalogs:
        for action, spec in catalog.items():
            assert RELEASE_CHANNEL_ACTION_SPECS[action] is spec


def test_release_channel_handlers_aggregate_owner_handlers() -> None:
    owner_handlers = (
        RELEASE_FOUNDATION_ACTION_HANDLERS,
        PACKAGING_CHANNEL_ACTION_HANDLERS,
        RELEASE_OPERATIONS_ACTION_HANDLERS,
        DISTRIBUTION_CREDIBILITY_ACTION_HANDLERS,
    )
    owner_actions = set().union(*(handlers.keys() for handlers in owner_handlers))

    assert owner_actions == set(REPORTING_RELEASE_CHANNEL_HANDLERS)
    for handlers in owner_handlers:
        for action, handler in handlers.items():
            assert REPORTING_RELEASE_CHANNEL_HANDLERS[action] is handler


def test_runtime_validation_catalog_aggregates_owner_catalogs() -> None:
    owner_catalogs = (
        RUNTIME_ACCEPTANCE_ACTION_SPECS,
        RUNTIME_ARCHITECTURE_ACTION_SPECS,
        RUNTIME_RUNNABLE_CONFORMANCE_ACTION_SPECS,
        RUNTIME_RUNNABLE_E2E_ACTION_SPECS,
    )
    owner_actions = set().union(*(catalog.keys() for catalog in owner_catalogs))

    assert owner_actions == set(RUNTIME_VALIDATION_ACTION_SPECS)
    for catalog in owner_catalogs:
        for action, spec in catalog.items():
            assert RUNTIME_VALIDATION_ACTION_SPECS[action] is spec


def test_runtime_validation_handlers_aggregate_owner_handlers() -> None:
    owner_handlers = (
        RUNTIME_ACCEPTANCE_ACTION_HANDLERS,
        RUNTIME_ARCHITECTURE_ACTION_HANDLERS,
        RUNTIME_RUNNABLE_CONFORMANCE_ACTION_HANDLERS,
        RUNTIME_RUNNABLE_E2E_ACTION_HANDLERS,
    )
    owner_actions = set().union(*(handlers.keys() for handlers in owner_handlers))

    assert owner_actions == set(RUNTIME_VALIDATION_ACTION_HANDLERS)
    for handlers in owner_handlers:
        for action, handler in handlers.items():
            assert RUNTIME_VALIDATION_ACTION_HANDLERS[action] is handler


def test_workflow_path_roots_are_owned_by_package_module() -> None:
    assert workflow_import_roots() == (ROOT, SCRIPT_ROOT)
    assert WORKFLOW_IMPORT_ROOTS == workflow_import_roots()
    assert SCRIPT_ROOT.name == "scripts"
    assert ROOT == SCRIPT_ROOT.parent


def test_workflow_entrypoints_have_separate_owners() -> None:
    assert module_entrypoint_main is not script_entrypoint_main


def test_workflow_environment_facade_exports_owned_markdown_globs() -> None:
    assert MARKDOWN_GLOBS is OWNED_MARKDOWN_GLOBS
