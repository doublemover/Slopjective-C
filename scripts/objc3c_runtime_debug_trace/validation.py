"""Structural validation for runtime debug trace payloads."""

from __future__ import annotations

import hashlib
from typing import Any

from objc3c_tooling.paths import ROOT, resolve_repo_path

from .contracts import RUNTIME_DEBUG_TRACE_CONTRACT_ID
from .source_contracts import RUNTIME_TRACE_SOURCE_CONTRACT_ID

_REQUIRED_TOP_LEVEL_FIELDS = {
    "contract_id",
    "schema_id",
    "schema_version",
    "schema_path",
    "source_path",
    "ok",
    "failures",
    "trace_model",
    "inputs",
    "artifacts",
    "source_mapping",
    "source_span_evidence",
    "runtime_inspection",
    "runtime_trace_contracts",
    "trace_lanes",
    "inspection_queries",
    "support_boundary",
    "event_sequence",
    "event_counts",
    "determinism",
    "inspection_commands",
    "support_handoff",
    "steps",
}


def _expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _digest(path: object) -> str:
    resolved = resolve_repo_path(str(path))
    digest = hashlib.sha256()
    with resolved.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _validate_input_evidence(payload: dict[str, Any], failures: list[str]) -> None:
    inputs = payload.get("inputs", {})
    if not isinstance(inputs, dict):
        failures.append("input evidence records are missing")
        return
    for label, record_value in sorted(inputs.items()):
        record = _object(record_value)
        if record.get("available") is not True:
            continue
        path = resolve_repo_path(str(record.get("path", "") or ""))
        if not path.is_file():
            failures.append(f"available input evidence is missing: {label}")
            continue
        if int(record.get("size_bytes", -1) or -1) != path.stat().st_size:
            failures.append(f"input evidence size is stale: {label}")
        expected_digest = str(record.get("sha256", "") or "")
        if len(expected_digest) != 64 or _digest(path) != expected_digest:
            failures.append(f"input evidence digest is stale: {label}")


def _source_contract_path(raw_path: object) -> object:
    text = str(raw_path or "")
    if text.startswith("runtime/"):
        return ROOT / "native" / "objc3c" / "src" / text
    return text


def _validate_runtime_trace_source_contracts(
    payload: dict[str, Any],
    failures: list[str],
) -> tuple[set[str], set[str]]:
    contracts = payload.get("runtime_trace_contracts", {})
    if not isinstance(contracts, dict) or not contracts:
        failures.append("runtime trace source contracts are missing")
        return set(), set()

    _expect(
        contracts.get("contract_id") == RUNTIME_TRACE_SOURCE_CONTRACT_ID,
        "runtime trace source contract id drifted",
        failures,
    )
    source_path = str(contracts.get("source_path", "") or "")
    implementation_path = str(contracts.get("implementation_path", "") or "")
    if not source_path:
        failures.append("runtime trace source contracts missing source path")
    if not implementation_path:
        failures.append("runtime trace source contracts missing implementation path")

    source_text = ""
    for key, digest_key in (
        ("source_path", "source_sha256"),
        ("implementation_path", "implementation_sha256"),
    ):
        path_text = str(contracts.get(key, "") or "")
        if not path_text:
            continue
        path = resolve_repo_path(path_text)
        if not path.is_file():
            failures.append(f"runtime trace source contract file missing: {path_text}")
            continue
        if _digest(path) != str(contracts.get(digest_key, "") or ""):
            failures.append(f"runtime trace source contract digest is stale: {path_text}")
        if key == "source_path":
            source_text = path.read_text(encoding="utf-8")

    lanes = _list(contracts.get("lanes"))
    if int(contracts.get("lane_count", -1) or -1) != len(lanes):
        failures.append("runtime trace source contract lane count drifted")
    _expect(bool(lanes), "runtime trace source contract lanes are missing", failures)

    source_anchors: set[str] = set()
    lane_ids: set[str] = set()
    expected_domains = {"task", "actor", "dispatch", "object", "memory", "error"}
    observed_domains: set[str] = set()
    for lane_value in lanes:
        lane = _object(lane_value)
        lane_id = str(lane.get("lane_id", "") or "")
        trace_domain = str(lane.get("trace_domain", "") or "")
        source_anchor = str(lane.get("source_anchor", "") or "")
        snapshot_header = str(lane.get("snapshot_header", "") or "")
        snapshot_symbol = str(lane.get("snapshot_symbol", "") or "")
        required_fields = _list(lane.get("required_fields"))
        lane_ids.add(lane_id)
        observed_domains.add(trace_domain)
        if source_anchor:
            source_anchors.add(source_anchor)
        _expect(bool(lane_id), "runtime trace source contract lane missing id", failures)
        _expect(bool(trace_domain), f"runtime trace source contract lane missing domain: {lane_id}", failures)
        _expect(
            lane.get("status") == "supported",
            f"runtime trace source contract lane must be supported: {lane_id}",
            failures,
        )
        _expect(
            lane.get("deterministic") is True,
            f"runtime trace source contract lane must be deterministic: {lane_id}",
            failures,
        )
        _expect(
            lane.get("public_abi") is False,
            f"runtime trace source contract lane must stay private ABI: {lane_id}",
            failures,
        )
        _expect(
            bool(source_anchor) and source_anchor in source_text,
            f"supported runtime trace contract missing live source anchor: {lane_id}",
            failures,
        )
        _expect(
            isinstance(required_fields, list) and bool(required_fields),
            f"runtime trace source contract lane missing required fields: {lane_id}",
            failures,
        )
        if snapshot_header:
            snapshot_path = resolve_repo_path(_source_contract_path(snapshot_header))
            if not snapshot_path.is_file():
                failures.append(
                    f"runtime trace source contract snapshot header missing: {lane_id}"
                )
            else:
                snapshot_text = snapshot_path.read_text(encoding="utf-8")
                _expect(
                    snapshot_symbol in snapshot_text,
                    f"runtime trace source contract snapshot symbol missing: {lane_id}",
                    failures,
                )
                for field in required_fields:
                    _expect(
                        str(field) in snapshot_text,
                        f"runtime trace source contract required field missing: {lane_id}.{field}",
                        failures,
                    )
    missing_domains = expected_domains - observed_domains
    for domain in sorted(missing_domains):
        failures.append(f"runtime trace source contract domain missing: {domain}")
    return source_anchors, lane_ids


def _validate_source_span_evidence(
    payload: dict[str, Any],
    failures: list[str],
) -> set[str]:
    spans = payload.get("source_span_evidence", [])
    if not isinstance(spans, list):
        failures.append("source span evidence must be a list")
        return set()
    span_ids: set[str] = set()
    for index, span_value in enumerate(spans):
        span = _object(span_value)
        span_id = str(span.get("span_id", "") or "")
        span_ids.add(span_id)
        _expect(bool(span_id), f"source span evidence missing span id: {index}", failures)
        _expect(
            span.get("status") == "supported",
            f"source span evidence must be supported: {span_id}",
            failures,
        )
        _expect(
            bool(str(span.get("source_path", "") or "")),
            f"source span evidence missing source path: {span_id}",
            failures,
        )
        _expect(
            bool(str(span.get("artifact_path", "") or "")),
            f"source span evidence missing artifact path: {span_id}",
            failures,
        )
        _expect(
            str(span.get("public_command", "") or "").startswith("npm run objc3c -- "),
            f"source span evidence lacks public command: {span_id}",
            failures,
        )
        compiler_range = _object(span.get("compiler_range"))
        lsp_range = _object(span.get("lsp_range"))
        line = int(compiler_range.get("line", 0) or 0)
        column = int(compiler_range.get("column", 0) or 0)
        end_line = int(compiler_range.get("end_line", 0) or 0)
        end_column = int(compiler_range.get("end_column", 0) or 0)
        _expect(line >= 1, f"source span evidence has invalid line: {span_id}", failures)
        _expect(column >= 1, f"source span evidence has invalid column: {span_id}", failures)
        _expect(end_line >= line, f"source span evidence has invalid end line: {span_id}", failures)
        _expect(
            end_column > column,
            f"source span evidence has invalid end column: {span_id}",
            failures,
        )
        _expect(
            _object(lsp_range.get("start")).get("line") == line - 1,
            f"source span evidence LSP start line drifted: {span_id}",
            failures,
        )
        _expect(
            _object(lsp_range.get("start")).get("character") == column - 1,
            f"source span evidence LSP start column drifted: {span_id}",
            failures,
        )
        evidence_input_labels = span.get("evidence_input_labels", [])
        _expect(
            isinstance(evidence_input_labels, list)
            and {"debug_map", "runtime_inspector"}.issubset(
                {str(label) for label in evidence_input_labels}
            ),
            f"source span evidence missing required input labels: {span_id}",
            failures,
        )
        unsupported_expansion = span.get("unsupported_expansion", [])
        _expect(
            isinstance(unsupported_expansion, list)
            and "statement-level stepping" in unsupported_expansion,
            f"source span evidence must preserve stepping boundary: {span_id}",
            failures,
        )
    source_mapping = payload.get("source_mapping", {})
    if isinstance(source_mapping, dict):
        mapped_span_ids = {
            str(value)
            for value in _list(source_mapping.get("span_evidence_ids"))
            if str(value)
        }
        _expect(
            mapped_span_ids == {span_id for span_id in span_ids if span_id},
            "source mapping span evidence ids drifted",
            failures,
        )
        _expect(
            int(source_mapping.get("span_evidence_count", -1) or -1)
            == len([span_id for span_id in span_ids if span_id]),
            "source mapping span evidence count drifted",
            failures,
        )
    return {span_id for span_id in span_ids if span_id}


def validate_runtime_debug_trace_payload(payload: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    for field in sorted(_REQUIRED_TOP_LEVEL_FIELDS):
        _expect(field in payload, f"runtime debug trace payload missing field: {field}", failures)
    _expect(
        payload.get("contract_id") == RUNTIME_DEBUG_TRACE_CONTRACT_ID,
        "runtime debug trace contract id drifted",
        failures,
    )
    _expect(payload.get("schema_version") == 1, "schema version drifted", failures)
    events = payload.get("event_sequence", [])
    _expect(isinstance(events, list) and bool(events), "event sequence is empty", failures)
    if isinstance(events, list):
        _expect(
            [event.get("ordinal") for event in events if isinstance(event, dict)]
            == list(range(len(events))),
            "event ordinals are not deterministic and contiguous",
            failures,
        )
        _expect(
            all(
                isinstance(event, dict) and event.get("deterministic") is True
                for event in events
            ),
            "all runtime debug trace events must be deterministic",
            failures,
        )
    event_counts = payload.get("event_counts", {})
    _expect(
        isinstance(event_counts, dict)
        and int(event_counts.get("compile-stage", 0) or 0) >= 5,
        "compile-stage events missing from runtime debug trace",
        failures,
    )
    source_anchors, source_lane_ids = _validate_runtime_trace_source_contracts(
        payload,
        failures,
    )
    source_span_ids = _validate_source_span_evidence(payload, failures)
    _validate_input_evidence(payload, failures)
    for event_kind in [
        "runtime-actor-snapshot",
        "runtime-dispatch-snapshot",
        "runtime-error-snapshot",
        "runtime-memory-snapshot",
        "runtime-object-snapshot",
        "runtime-task-snapshot",
    ]:
        _expect(
            isinstance(event_counts, dict)
            and int(event_counts.get(event_kind, 0) or 0) == 1,
            f"{event_kind} event missing from runtime debug trace",
            failures,
        )
    determinism = payload.get("determinism", {})
    digest = determinism.get("trace_digest", "") if isinstance(determinism, dict) else ""
    _expect(isinstance(digest, str) and len(digest) == 64, "trace digest missing", failures)
    support_boundary = payload.get("support_boundary", {})
    if isinstance(support_boundary, dict):
        _expect(
            support_boundary.get("debug_metadata_public_abi") is False,
            "debug metadata must not be promoted as public ABI",
            failures,
        )
        _expect(
            support_boundary.get("statement_level_stepping") is False,
            "statement-level stepping must stay fail-closed",
            failures,
        )
    source_mapping = payload.get("source_mapping", {})
    if isinstance(source_mapping, dict):
        _expect(
            source_mapping.get("full_source_map_status") == "reserved",
            "full source-map publication must stay reserved",
            failures,
        )
        _expect(
            source_mapping.get("inline_frame_status") in {"supported", "reserved", "rejected"},
            "inline-frame status is invalid",
            failures,
        )
        if source_mapping.get("inline_frame_status") != "supported":
            _expect(
                int(source_mapping.get("inline_frame_count", -1)) == 0,
                "inline frames must stay empty while inline-frame support is not published",
                failures,
            )
            _expect(
                bool(source_mapping.get("inline_frame_fail_closed_reason")),
                "inline-frame reserved state must carry a fail-closed reason",
                failures,
            )
    trace_lanes = payload.get("trace_lanes", {})
    if isinstance(trace_lanes, dict):
        _expect(
            trace_lanes.get("lldb_plugin", {}).get("status") == "reserved",
            "LLDB plugin lane must stay reserved",
            failures,
        )
        _expect(
            trace_lanes.get("async_task_inspection", {}).get("status") == "supported",
            "async task lane must be source-contract backed",
            failures,
        )
        _expect(
            trace_lanes.get("error_unwind_trace", {}).get("status") == "supported",
            "error unwind lane must be source-contract backed",
            failures,
        )
        _expect(
            trace_lanes.get("statement_level_stepping", {}).get("status") == "reserved",
            "statement-level stepping lane must stay reserved",
            failures,
        )
        _expect(
            trace_lanes.get("full_source_map_publication", {}).get("status") == "reserved",
            "full source-map lane must stay reserved",
            failures,
        )
        for lane_id in [
            "actor_runtime_trace",
            "dispatch_runtime_trace",
            "error_unwind_trace",
            "memory_runtime_trace",
            "object_runtime_trace",
            "task_runtime_trace",
        ]:
            lane = _object(trace_lanes.get(lane_id))
            _expect(
                lane.get("status") == "supported",
                f"source-owned runtime trace lane missing: {lane_id}",
                failures,
            )
        for lane_id, lane_value in sorted(trace_lanes.items()):
            lane = _object(lane_value)
            if lane.get("status") != "supported":
                continue
            source_contract_ids = [
                str(value)
                for value in _list(lane.get("source_contract_ids"))
                if str(value)
            ]
            lane_source_anchors = [
                str(value)
                for value in _list(lane.get("source_anchors"))
                if str(value)
            ]
            if source_contract_ids:
                _expect(
                    bool(lane_source_anchors),
                    f"supported runtime trace lane lacks source anchors: {lane_id}",
                    failures,
                )
            for source_contract_id in source_contract_ids:
                _expect(
                    source_contract_id in source_lane_ids,
                    f"supported runtime trace lane references unknown source contract: {lane_id}",
                    failures,
                )
            for source_anchor in lane_source_anchors:
                if source_anchor.startswith("OBJ3-NEXT-023."):
                    _expect(
                        source_anchor in source_anchors,
                        f"supported runtime trace lane source anchor is stale: {lane_id}",
                        failures,
                    )
    inspection_queries = payload.get("inspection_queries", [])
    _expect(
        isinstance(inspection_queries, list) and bool(inspection_queries),
        "inspection queries are missing",
        failures,
    )
    if isinstance(inspection_queries, list):
        queries_by_id: dict[str, dict[str, Any]] = {}
        for query in inspection_queries:
            if not isinstance(query, dict):
                failures.append("inspection query must be an object")
                continue
            query_id = str(query.get("query_id", "") or "")
            status = str(query.get("status", "") or "")
            queries_by_id[query_id] = query
            _expect(bool(query_id), "inspection query missing query id", failures)
            _expect(
                status in {"supported", "reserved", "rejected", "internal"},
                f"inspection query has invalid status: {query_id}",
                failures,
            )
            if status == "supported":
                public_command = str(query.get("public_command", "") or "")
                evidence_input_labels = query.get("evidence_input_labels", [])
                query_source_span_ids = [
                    str(value)
                    for value in _list(query.get("source_span_ids"))
                    if str(value)
                ]
                _expect(
                    public_command.startswith("npm run objc3c -- "),
                    f"supported inspection query lacks public command: {query_id}",
                    failures,
                )
                _expect(
                    isinstance(evidence_input_labels, list) and bool(evidence_input_labels),
                    f"supported inspection query lacks evidence inputs: {query_id}",
                    failures,
                )
                _expect(
                    bool(str(query.get("result_path", "") or "")),
                    f"supported inspection query lacks result path: {query_id}",
                    failures,
                )
                if query.get("surface") in {
                    "source_to_artifact_mapping",
                    "runtime_debug_trace",
                }:
                    _expect(
                        bool(query_source_span_ids),
                        f"supported source/artifact inspection query lacks source span evidence: {query_id}",
                        failures,
                    )
                for source_span_id in query_source_span_ids:
                    _expect(
                        source_span_id in source_span_ids,
                        f"supported inspection query references stale source span: {query_id}",
                        failures,
                    )
            if status == "reserved":
                _expect(
                    bool(str(query.get("unpublished_reason", "") or "")),
                    f"reserved inspection query missing unpublished reason: {query_id}",
                    failures,
                )
        expected_supported_queries = {
            "debug.source-to-artifact.declaration-anchors",
            "runtime.object-inspection.object-symbols",
            "runtime.message-send.dispatch-cache-observation",
            "debug.runtime-trace.composed-event-sequence",
            "runtime.source-owned-trace-contracts",
            "runtime.async-task-inspection.source-contracts",
            "runtime.error-unwind-trace.snapshots",
            "runtime.memory-inspection.source-contracts",
        }
        for query_id in expected_supported_queries:
            _expect(
                queries_by_id.get(query_id, {}).get("status") == "supported",
                f"expected supported inspection query missing: {query_id}",
                failures,
            )
        expected_reserved_queries = {
            "debug.statement-level-stepping.line-table",
            "debug.full-source-map.publication",
            "debug.lldb-plugin.integration",
        }
        for query_id in expected_reserved_queries:
            _expect(
                queries_by_id.get(query_id, {}).get("status") == "reserved",
                f"expected reserved inspection query missing: {query_id}",
                failures,
            )
    inputs = payload.get("inputs", {})
    input_records = inputs if isinstance(inputs, dict) else {}
    support_handoff = payload.get("support_handoff", {})
    rows = support_handoff.get("capability_rows", []) if isinstance(support_handoff, dict) else []
    if isinstance(rows, list):
        for row in rows:
            if not isinstance(row, dict):
                failures.append("support handoff row must be an object")
                continue
            capability_id = str(row.get("capability_id", "") or "")
            status = str(row.get("status", "") or "")
            evidence_ids = row.get("evidence_ids", [])
            _expect(bool(capability_id), "support handoff row missing capability id", failures)
            _expect(
                status in {"supported", "reserved", "rejected"},
                f"support handoff row has invalid status: {capability_id}",
                failures,
            )
            _expect(
                isinstance(evidence_ids, list) and bool(evidence_ids),
                f"support handoff row missing evidence ids: {capability_id}",
                failures,
            )
            if status == "supported":
                required_labels = row.get("required_input_labels", [])
                source_anchor_values = [
                    str(value)
                    for value in _list(row.get("source_anchors"))
                    if str(value)
                ]
                _expect(
                    (isinstance(required_labels, list) and bool(required_labels))
                    or bool(source_anchor_values),
                    f"supported runtime debug trace row missing evidence anchors: {capability_id}",
                    failures,
                )
                if capability_id.startswith("objc3c.behavior.runtime.debug_trace"):
                    _expect(
                        bool(source_anchor_values),
                        f"supported runtime debug trace row lacks source anchors: {capability_id}",
                        failures,
                    )
                for source_anchor in source_anchor_values:
                    _expect(
                        source_anchor in source_anchors,
                        f"supported runtime debug trace row has stale source anchor: {capability_id}",
                        failures,
                    )
                if row.get("source_contract_id"):
                    _expect(
                        row.get("source_contract_id") == RUNTIME_TRACE_SOURCE_CONTRACT_ID,
                        f"supported runtime debug trace row source contract id drifted: {capability_id}",
                        failures,
                    )
                required_label_values = required_labels if isinstance(required_labels, list) else []
                for label in required_label_values:
                    record = input_records.get(str(label), {})
                    available = isinstance(record, dict) and record.get("available") is True
                    _expect(
                        available,
                        f"supported runtime debug trace row has unavailable input: {label}",
                        failures,
                    )
                if required_label_values:
                    _expect(
                        row.get("required_inputs_available") is True,
                        f"supported runtime debug trace row did not confirm input availability: {capability_id}",
                        failures,
                    )
            if status == "reserved":
                _expect(
                    bool(row.get("unpublished_reason")),
                    f"reserved runtime debug trace row missing unpublished reason: {capability_id}",
                    failures,
                )
    return failures
