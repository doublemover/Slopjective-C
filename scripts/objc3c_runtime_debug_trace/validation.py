"""Structural validation for runtime debug trace payloads."""

from __future__ import annotations

from typing import Any

from .contracts import RUNTIME_DEBUG_TRACE_CONTRACT_ID


def _expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def validate_runtime_debug_trace_payload(payload: dict[str, Any]) -> list[str]:
    failures: list[str] = []
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
    trace_lanes = payload.get("trace_lanes", {})
    if isinstance(trace_lanes, dict):
        _expect(
            trace_lanes.get("lldb_plugin", {}).get("status") == "reserved",
            "LLDB plugin lane must stay reserved",
            failures,
        )
        _expect(
            trace_lanes.get("async_task_inspection", {}).get("status") == "reserved",
            "async task lane must stay reserved until runtime snapshots are published",
            failures,
        )
        _expect(
            trace_lanes.get("error_unwind_trace", {}).get("status") == "reserved",
            "error unwind lane must stay reserved until runtime snapshots are published",
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
                _expect(
                    isinstance(required_labels, list) and bool(required_labels),
                    f"supported runtime debug trace row missing required input labels: {capability_id}",
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
