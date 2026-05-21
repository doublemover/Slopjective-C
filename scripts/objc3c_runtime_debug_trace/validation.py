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
    return failures
