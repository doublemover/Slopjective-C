from __future__ import annotations

import json
from pathlib import Path

from scripts.objc3c_runtime_debug_trace.payload import (
    build_runtime_debug_trace_payload,
)
from scripts.objc3c_runtime_debug_trace.validation import (
    validate_runtime_debug_trace_payload,
)
from scripts.objc3c_shared.schema_registry import schema_path, validate_registered_schema
from scripts.objc3c_workflow.action_catalog import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS


ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling" / "runtime_debug_trace"


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def fixture_payload() -> dict[str, object]:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    return build_fixture_payload(
        source_path=str(contract["source_path"]),
        input_paths={
            "runtime_inspector": FIXTURE_ROOT / "runtime-inspector.json",
            "compile_stage_trace": FIXTURE_ROOT / "compile-stage-trace.json",
            "editor_surface": FIXTURE_ROOT / "editor-surface.json",
            "debug_map": FIXTURE_ROOT / "debug-map.json",
        },
    )


def build_fixture_payload(
    *,
    source_path: str,
    input_paths: dict[str, Path | str],
) -> dict[str, object]:
    return build_runtime_debug_trace_payload(
        source_path=source_path,
        runtime_inspector=load_json(FIXTURE_ROOT / "runtime-inspector.json"),
        stage_trace=load_json(FIXTURE_ROOT / "compile-stage-trace.json"),
        editor_surface=load_json(FIXTURE_ROOT / "editor-surface.json"),
        debug_map=load_json(FIXTURE_ROOT / "debug-map.json"),
        input_paths=input_paths,
        steps=[],
    )


def test_runtime_debug_trace_fixture_builds_deterministic_payload() -> None:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    payload = fixture_payload()
    second_payload = fixture_payload()

    assert validate_runtime_debug_trace_payload(payload) == []
    assert payload["contract_id"] == "objc3c.runtime.debug.trace.v1"
    assert payload["ok"] is True
    assert payload["event_counts"]["compile-stage"] == contract["expected_event_counts"]["compile-stage"]
    assert payload["event_counts"]["debug-anchor"] == contract["expected_event_counts"]["debug-anchor"]
    assert payload["event_counts"]["reserved-surface"] == contract["expected_event_counts"]["reserved-surface"]
    assert payload["determinism"]["trace_digest"] == second_payload["determinism"]["trace_digest"]
    assert [event["ordinal"] for event in payload["event_sequence"]] == list(
        range(len(payload["event_sequence"]))
    )
    assert all(event["deterministic"] is True for event in payload["event_sequence"])


def test_runtime_debug_trace_lanes_do_not_overpublish_debugger_support() -> None:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    payload = fixture_payload()
    trace_lanes = payload["trace_lanes"]

    for lane in contract["expected_supported_lanes"]:
        assert trace_lanes[lane]["status"] == "supported"
    for lane in contract["expected_reserved_lanes"]:
        assert trace_lanes[lane]["status"] == "reserved"
    assert payload["support_boundary"]["debug_metadata_public_abi"] is False
    assert payload["support_boundary"]["statement_level_stepping"] is False
    assert payload["source_mapping"]["full_source_map_status"] == "reserved"
    assert payload["inspection_commands"]["runtime_debug_trace"].startswith(
        "npm run objc3c -- trace-runtime-debug"
    )


def test_runtime_debug_trace_inspection_queries_are_public_and_fail_closed() -> None:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    payload = fixture_payload()
    queries = {
        query["query_id"]: query
        for query in payload["inspection_queries"]
    }

    for query_id in contract["expected_supported_inspection_queries"]:
        query = queries[query_id]
        assert query["status"] == "supported"
        assert query["public_command"].startswith("npm run objc3c -- ")
        assert query["evidence_input_labels"]
        assert query["result_path"]
    for query_id in contract["expected_reserved_inspection_queries"]:
        query = queries[query_id]
        assert query["status"] == "reserved"
        assert query["public_command"] == ""
        assert query["unpublished_reason"]
    assert (
        queries["debug.runtime-trace.composed-event-sequence"]["schema_path"]
        == "schemas/objc3c-runtime-debug-trace-v1.schema.json"
    )


def test_runtime_debug_trace_support_handoff_ids_are_explicit() -> None:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    payload = fixture_payload()
    handoff_ids = {
        row["capability_id"]
        for row in payload["support_handoff"]["capability_rows"]
    }
    rows = {
        row["capability_id"]: row
        for row in payload["support_handoff"]["capability_rows"]
    }

    assert set(contract["required_support_handoff_ids"]).issubset(handoff_ids)
    assert "OBJ3-NEXT-023.schema.runtime-debug-trace.v1" in payload["support_handoff"]["evidence_ids"]
    assert rows["objc3c.behavior.runtime.debug_trace"]["required_inputs_available"] is True
    assert rows["objc3c.behavior.runtime.debug_trace"]["required_input_labels"] == [
        "compile_stage_trace",
        "debug_map",
        "editor_surface",
        "runtime_inspector",
    ]
    for capability_id in contract["required_support_handoff_ids"]:
        if capability_id == "objc3c.behavior.runtime.debug_trace":
            continue
        assert rows[capability_id]["status"] == "reserved"
        assert rows[capability_id]["unpublished_reason"]


def test_runtime_debug_trace_supported_row_fails_closed_without_replayable_inputs() -> None:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    payload = build_fixture_payload(
        source_path=str(contract["source_path"]),
        input_paths={
            "runtime_inspector": FIXTURE_ROOT / "runtime-inspector.json",
            "compile_stage_trace": FIXTURE_ROOT / "compile-stage-trace.json",
            "editor_surface": FIXTURE_ROOT / "editor-surface.json",
            "debug_map": FIXTURE_ROOT / "missing-debug-map.json",
        },
    )

    failures = validate_runtime_debug_trace_payload(payload)

    assert "supported runtime debug trace row has unavailable input: debug_map" in failures


def test_runtime_debug_trace_query_validation_rejects_private_supported_commands() -> None:
    payload = fixture_payload()
    query = next(
        query
        for query in payload["inspection_queries"]
        if query["query_id"] == "debug.runtime-trace.composed-event-sequence"
    )
    query["public_command"] = "python scripts/build_objc3c_runtime_debug_trace.py"

    failures = validate_runtime_debug_trace_payload(payload)

    assert (
        "supported inspection query lacks public command: "
        "debug.runtime-trace.composed-event-sequence"
    ) in failures


def test_runtime_debug_trace_schema_and_public_action_are_registered() -> None:
    schema = load_json(ROOT / "schemas" / "objc3c-runtime-debug-trace-v1.schema.json")

    assert schema["$id"].endswith("objc3c-runtime-debug-trace-v1.schema.json")
    assert schema_path("objc3c-runtime-debug-trace-v1").as_posix().endswith(
        "schemas/objc3c-runtime-debug-trace-v1.schema.json"
    )
    validate_registered_schema(fixture_payload(), "objc3c-runtime-debug-trace-v1")
    assert "event_sequence" in schema["required"]
    assert "trace_lanes" in schema["properties"]
    assert "inspection_queries" in schema["properties"]
    assert "support_boundary" in schema["properties"]
    assert "trace-runtime-debug" in ACTION_SPECS
    assert "trace-runtime-debug" in ACTION_HANDLERS
    assert ACTION_SPECS["trace-runtime-debug"].pass_through_args is True
