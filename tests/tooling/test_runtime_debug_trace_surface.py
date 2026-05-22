from __future__ import annotations

import json
from copy import deepcopy
from pathlib import Path

from scripts.objc3c_runtime_debug_trace.payload import (
    build_runtime_debug_trace_payload,
)
from scripts.objc3c_runtime_debug_trace.source_contracts import (
    load_runtime_trace_source_contracts,
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
    for event_kind in [
        "runtime-actor-snapshot",
        "runtime-dispatch-snapshot",
        "runtime-memory-snapshot",
        "runtime-object-snapshot",
        "runtime-task-snapshot",
    ]:
        assert payload["event_counts"][event_kind] == contract["expected_event_counts"][event_kind]
    assert payload["determinism"]["trace_digest"] == second_payload["determinism"]["trace_digest"]
    assert [event["ordinal"] for event in payload["event_sequence"]] == list(
        range(len(payload["event_sequence"]))
    )
    assert all(event["deterministic"] is True for event in payload["event_sequence"])
    assert payload["source_mapping"]["span_evidence_count"] == contract["expected_source_span_evidence_count"]
    assert len(payload["source_span_evidence"]) == contract["expected_source_span_evidence_count"]


def test_runtime_debug_trace_lanes_do_not_overpublish_debugger_support() -> None:
    contract = load_json(FIXTURE_ROOT / "contract.json")
    payload = fixture_payload()
    trace_lanes = payload["trace_lanes"]

    for lane in contract["expected_supported_lanes"]:
        assert trace_lanes[lane]["status"] == "supported"
        if trace_lanes[lane]["source_contract_ids"]:
            assert trace_lanes[lane]["source_anchors"]
    for lane in contract["expected_reserved_lanes"]:
        assert trace_lanes[lane]["status"] == "reserved"
    assert payload["support_boundary"]["debug_metadata_public_abi"] is False
    assert payload["support_boundary"]["statement_level_stepping"] is False
    assert payload["support_boundary"]["native_debug_info_fail_closed_reason"] == (
        "native object lacks debug info and debug line-table sections"
    )
    assert payload["source_mapping"]["full_source_map_status"] == "reserved"
    assert payload["source_mapping"]["native_debug_info_evidence_id"] == (
        "runtime-debug-trace.native-debug-info.fixture-object-section-probe"
    )
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
    assert queries["debug.statement-level-stepping.line-table"]["unpublished_reason"] == (
        "native object lacks debug info and debug line-table sections"
    )
    assert (
        queries["debug.runtime-trace.composed-event-sequence"]["schema_path"]
        == "schemas/objc3c-runtime-debug-trace-v1.schema.json"
    )
    assert (
        queries["runtime.source-owned-trace-contracts"]["artifact_path"]
        == "native/objc3c/src/runtime/debug/runtime_debug_trace_contracts.h"
    )
    source_span_ids = {span["span_id"] for span in payload["source_span_evidence"]}
    for query_id in contract["expected_source_span_query_ids"]:
        assert set(queries[query_id]["source_span_ids"]) == source_span_ids


def test_runtime_debug_trace_source_span_evidence_binds_ranges_to_artifacts() -> None:
    payload = fixture_payload()
    spans = {
        span["symbol"]: span
        for span in payload["source_span_evidence"]
    }

    assert set(spans) == {"ArtifactWidget", "globalCounter", "main"}
    assert spans["main"]["compiler_range"] == {
        "line": 9,
        "column": 1,
        "end_line": 9,
        "end_column": 5,
    }
    assert spans["main"]["lsp_range"] == {
        "start": {"line": 8, "character": 0},
        "end": {"line": 8, "character": 4},
    }
    for span in spans.values():
        assert span["status"] == "supported"
        assert span["artifact_path"].endswith("module.obj")
        assert span["artifact_query_command"].startswith("llvm-nm ")
        assert span["public_command"].startswith("npm run objc3c -- trace-runtime-debug")
        assert {"debug_map", "runtime_inspector"}.issubset(span["evidence_input_labels"])
        assert "statement-level stepping" in span["unsupported_expansion"]


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
    for capability_id in contract["expected_supported_support_handoff_ids"]:
        assert rows[capability_id]["status"] == "supported"
        assert rows[capability_id]["source_anchors"]
        assert (
            rows[capability_id]["source_contract_id"]
            == "objc3.runtime.debug.trace.source-contracts.v1"
        )
    for capability_id in contract["expected_reserved_support_handoff_ids"]:
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


def test_runtime_debug_trace_query_validation_rejects_missing_source_span_evidence() -> None:
    payload = fixture_payload()
    query = next(
        query
        for query in payload["inspection_queries"]
        if query["query_id"] == "debug.source-to-artifact.declaration-anchors"
    )
    query["source_span_ids"] = []

    failures = validate_runtime_debug_trace_payload(payload)

    assert (
        "supported source/artifact inspection query lacks source span evidence: "
        "debug.source-to-artifact.declaration-anchors"
    ) in failures


def test_runtime_debug_trace_validation_rejects_invalid_source_span_range() -> None:
    payload = fixture_payload()
    payload["source_span_evidence"][0]["compiler_range"]["line"] = 0

    failures = validate_runtime_debug_trace_payload(payload)

    assert (
        "source span evidence has invalid line: "
        f"{payload['source_span_evidence'][0]['span_id']}"
    ) in failures


def test_runtime_debug_trace_source_contracts_are_runtime_owned_and_private() -> None:
    payload = fixture_payload()
    contracts = payload["runtime_trace_contracts"]
    lanes = {lane["trace_domain"]: lane for lane in contracts["lanes"]}
    source = (ROOT / contracts["source_path"]).read_text(encoding="utf-8")

    assert contracts == load_runtime_trace_source_contracts()
    assert contracts["contract_id"] == "objc3.runtime.debug.trace.source-contracts.v1"
    assert contracts["source_path"] == "native/objc3c/src/runtime/debug/runtime_debug_trace_contracts.h"
    assert set(lanes) == {"actor", "dispatch", "error", "memory", "object", "task"}
    for domain, lane in lanes.items():
        assert lane["lane_id"] == f"runtime.{domain}.snapshot"
        assert lane["status"] == "supported"
        assert lane["deterministic"] is True
        assert lane["public_abi"] is False
        assert lane["source_anchor"] in source
        assert lane["snapshot_symbol"].startswith("objc3_runtime_copy_")
        assert lane["required_fields"]


def test_runtime_debug_trace_validation_rejects_missing_source_contracts() -> None:
    payload = fixture_payload()
    del payload["runtime_trace_contracts"]

    failures = validate_runtime_debug_trace_payload(payload)

    assert "runtime debug trace payload missing field: runtime_trace_contracts" in failures
    assert "runtime trace source contracts are missing" in failures


def test_runtime_debug_trace_validation_rejects_stale_source_anchor() -> None:
    payload = fixture_payload()
    payload["runtime_trace_contracts"] = deepcopy(payload["runtime_trace_contracts"])
    payload["runtime_trace_contracts"]["lanes"][0]["source_anchor"] = (
        "OBJ3-NEXT-023.runtime-debug-trace.missing"
    )

    failures = validate_runtime_debug_trace_payload(payload)

    assert (
        "supported runtime trace contract missing live source anchor: "
        f"{payload['runtime_trace_contracts']['lanes'][0]['lane_id']}"
    ) in failures


def test_runtime_debug_trace_validation_rejects_stale_input_evidence() -> None:
    payload = fixture_payload()
    payload["inputs"] = deepcopy(payload["inputs"])
    payload["inputs"]["debug_map"]["sha256"] = "0" * 64

    failures = validate_runtime_debug_trace_payload(payload)

    assert "input evidence digest is stale: debug_map" in failures


def test_runtime_debug_trace_validation_rejects_supported_row_without_source_anchor() -> None:
    payload = fixture_payload()
    rows = payload["support_handoff"]["capability_rows"]
    row = next(
        row
        for row in rows
        if row["capability_id"] == "objc3c.behavior.runtime.debug_trace.memory"
    )
    row["source_anchors"] = []

    failures = validate_runtime_debug_trace_payload(payload)

    assert (
        "supported runtime debug trace row lacks source anchors: "
        "objc3c.behavior.runtime.debug_trace.memory"
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
    assert "runtime_trace_contracts" in schema["required"]
    assert "runtime_trace_contracts" in schema["properties"]
    assert '"error"' in (ROOT / "schemas" / "objc3c-runtime-debug-trace-v1.schema.json").read_text(
        encoding="utf-8"
    )
    assert "trace-runtime-debug" in ACTION_SPECS
    assert "trace-runtime-debug" in ACTION_HANDLERS
    assert ACTION_SPECS["trace-runtime-debug"].pass_through_args is True


def test_runtime_debug_trace_async_task_support_row_matches_payload() -> None:
    matrix = load_json(ROOT / "docs" / "support" / "capability_matrix.json")
    rows = {row["id"]: row for row in matrix["capabilities"]}
    async_row = rows["runtime.debug-trace.async-tasks"]
    structured_row = rows["runtime.debug-trace.structured-inspection"]
    evidence_paths = {item["path"] for item in async_row["evidence"]}
    payload = fixture_payload()
    handoff_rows = {
        row["capability_id"]: row
        for row in payload["support_handoff"]["capability_rows"]
    }

    assert async_row["state"] == "implemented"
    assert async_row["support_claims"] == [
        "objc3c.behavior.runtime.debug_trace.async_tasks"
    ]
    assert "native/objc3c/src/runtime/debug/runtime_debug_trace_contracts.h" in evidence_paths
    assert "scripts/objc3c_runtime_debug_trace/source_contracts.py" in evidence_paths
    assert "async/actor trace contract rows" in structured_row["summary"]
    assert "async task inspection" not in structured_row["summary"]
    assert (
        handoff_rows["objc3c.behavior.runtime.debug_trace.async_tasks"]["status"]
        == "supported"
    )


def test_runtime_debug_trace_error_unwind_support_row_matches_payload() -> None:
    matrix = load_json(ROOT / "docs" / "support" / "capability_matrix.json")
    rows = {row["id"]: row for row in matrix["capabilities"]}
    error_row = rows["runtime.debug-trace.error-unwind"]
    structured_row = rows["runtime.debug-trace.structured-inspection"]
    evidence_paths = {item["path"] for item in error_row["evidence"]}
    payload = fixture_payload()
    trace_lanes = payload["trace_lanes"]
    queries = {query["query_id"]: query for query in payload["inspection_queries"]}
    handoff_rows = {
        row["capability_id"]: row
        for row in payload["support_handoff"]["capability_rows"]
    }

    assert error_row["state"] == "implemented"
    assert error_row["support_claims"] == [
        "objc3c.behavior.runtime.debug_trace.error_unwind"
    ]
    assert "native/objc3c/src/runtime/errors/error_bridge_snapshot_contracts.h" in evidence_paths
    assert "native/objc3c/src/runtime/debug/runtime_debug_trace_contracts.h" in evidence_paths
    assert "error/bridge trace contract rows" in structured_row["summary"]
    assert trace_lanes["error_unwind_trace"]["status"] == "supported"
    assert trace_lanes["error_unwind_trace"]["source_contract_ids"] == [
        "runtime.error.snapshot"
    ]
    assert (
        queries["runtime.error-unwind-trace.snapshots"]["status"]
        == "supported"
    )
    assert (
        handoff_rows["objc3c.behavior.runtime.debug_trace.error_unwind"]["status"]
        == "supported"
    )
