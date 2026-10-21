"""Runtime debug trace payload assembly."""

from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT, display_path, resolve_repo_path

from .contracts import (
    COMPILE_STAGE_TRACE_MODE,
    DEBUG_MAP_CONTRACT_ID,
    EDITOR_SURFACE_CONTRACT_ID,
    RUNTIME_DEBUG_TRACE_CONTRACT_ID,
    RUNTIME_DEBUG_TRACE_SCHEMA_ID,
    RUNTIME_DEBUG_TRACE_SCHEMA_PATH,
    RUNTIME_DEBUG_TRACE_SCHEMA_VERSION,
    RUNTIME_INSPECTOR_CONTRACT_ID,
    TRACE_ACTION,
    TRACE_REPORT_PATH_TEXT,
)
from .source_contracts import load_runtime_trace_source_contracts


def _object_payload(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _list_payload(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _stable_json_bytes(payload: object) -> bytes:
    return json.dumps(
        payload,
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=True,
    ).encode("utf-8")


def stable_digest(payload: object) -> str:
    return hashlib.sha256(_stable_json_bytes(payload)).hexdigest()


def _file_digest(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _path_record(path: Path | str, *, label: str) -> dict[str, Any]:
    resolved = resolve_repo_path(path)
    available = resolved.is_file()
    return {
        "label": label,
        "path": display_path(resolved) if available else str(path).replace("\\", "/"),
        "available": available,
        "size_bytes": resolved.stat().st_size if available else 0,
        "sha256": _file_digest(resolved) if available else "",
    }


def _sanitize_event_id(value: object) -> str:
    text = str(value or "unknown").lower()
    text = re.sub(r"[^a-z0-9]+", "-", text).strip("-")
    return text or "unknown"


def _event(
    *,
    event_id: str,
    phase: str,
    domain: str,
    event_kind: str,
    subject: str,
    attributes: dict[str, Any],
) -> dict[str, Any]:
    return {
        "event_id": event_id,
        "phase": phase,
        "domain": domain,
        "event_kind": event_kind,
        "subject": subject,
        "deterministic": True,
        "attributes": attributes,
    }


def _ordinal(value: object, fallback: int = 999) -> int:
    try:
        return int(value)
    except (TypeError, ValueError):
        return fallback


def _stage_events(stage_trace: dict[str, Any]) -> list[dict[str, Any]]:
    stages = _object_payload(stage_trace.get("stages"))
    ordered = sorted(
        (
            (name, _object_payload(payload))
            for name, payload in stages.items()
        ),
        key=lambda item: (
            _ordinal(item[1].get("stage"), 999),
            str(item[0]),
        ),
    )
    return [
        _event(
            event_id=f"compile.stage.{int(stage.get('stage', 0) or 0):02d}.{_sanitize_event_id(name)}",
            phase="compile",
            domain="compiler",
            event_kind="compile-stage",
            subject=str(name),
            attributes={
                "stage_ordinal": int(stage.get("stage", 0) or 0),
                "attempted": stage.get("attempted") is True,
                "skipped": stage.get("skipped") is True,
                "diagnostics_total": int(stage.get("diagnostics_total", 0) or 0),
                "diagnostics_errors": int(stage.get("diagnostics_errors", 0) or 0),
                "diagnostics_fatals": int(stage.get("diagnostics_fatals", 0) or 0),
            },
        )
        for name, stage in ordered
    ]


def _snapshot_symbol_fields(runtime_inspector: dict[str, Any]) -> list[tuple[str, str]]:
    fields: list[tuple[str, str]] = []
    for key, value in sorted(runtime_inspector.items()):
        if key.endswith("_symbol") and isinstance(value, str) and value:
            fields.append((key, value))
    return fields


def _runtime_model_fields(runtime_inspector: dict[str, Any]) -> list[tuple[str, str]]:
    fields: list[tuple[str, str]] = []
    for key, value in sorted(runtime_inspector.items()):
        if key.endswith("_model") and isinstance(value, str) and value:
            fields.append((key, value))
    return fields


def _runtime_inspector_events(runtime_inspector: dict[str, Any]) -> list[dict[str, Any]]:
    events: list[dict[str, Any]] = []
    object_path = str(runtime_inspector.get("object_path", "") or "")
    if object_path:
        events.append(
            _event(
                event_id="runtime.object-artifact.module",
                phase="runtime-inspection",
                domain="runtime",
                event_kind="runtime-artifact",
                subject="object",
                attributes={
                    "object_path": object_path,
                    "available": runtime_inspector.get("available") is True,
                    "contract_id": str(runtime_inspector.get("contract_id", "") or ""),
                },
            )
        )

    dump_commands = _object_payload(runtime_inspector.get("dump_commands"))
    for name, command in sorted(dump_commands.items()):
        if not command:
            continue
        events.append(
            _event(
                event_id=f"runtime.inspector.command.{_sanitize_event_id(name)}",
                phase="runtime-inspection",
                domain="runtime",
                event_kind="runtime-inspector-command",
                subject=str(name),
                attributes={"command": str(command)},
            )
        )

    for field, symbol in _snapshot_symbol_fields(runtime_inspector):
        events.append(
            _event(
                event_id=f"runtime.snapshot-symbol.{_sanitize_event_id(field)}",
                phase="runtime-inspection",
                domain="runtime",
                event_kind="runtime-snapshot-symbol",
                subject=symbol,
                attributes={"field": field},
            )
        )

    for field, model in _runtime_model_fields(runtime_inspector):
        events.append(
            _event(
                event_id=f"runtime.model.{_sanitize_event_id(field)}",
                phase="runtime-inspection",
                domain="runtime",
                event_kind="runtime-model",
                subject=field,
                attributes={"model": model},
            )
        )
    return events


def _debug_anchor_events(debug_map: dict[str, Any]) -> list[dict[str, Any]]:
    anchors = [
        _object_payload(anchor)
        for anchor in _list_payload(debug_map.get("declaration_breakpoints"))
    ]
    anchors = sorted(
        anchors,
        key=lambda anchor: (
            int(anchor.get("line", 0) or 0),
            int(anchor.get("column", 0) or 0),
            str(anchor.get("kind", "")),
            str(anchor.get("symbol", "")),
        ),
    )
    return [
        _event(
            event_id=(
                "debug.anchor."
                f"{_sanitize_event_id(anchor.get('kind'))}."
                f"{_sanitize_event_id(anchor.get('symbol'))}."
                f"{int(anchor.get('line', 0) or 0)}."
                f"{int(anchor.get('column', 0) or 0)}"
            ),
            phase="debug-map",
            domain="debugger",
            event_kind="debug-anchor",
            subject=str(anchor.get("symbol", "") or ""),
            attributes={
                "kind": str(anchor.get("kind", "") or ""),
                "line": int(anchor.get("line", 0) or 0),
                "column": int(anchor.get("column", 0) or 0),
                "source_map_model": str(debug_map.get("source_map_model", "") or ""),
            },
        )
        for anchor in anchors
    ]


def _reserved_surface_events() -> list[dict[str, Any]]:
    reserved = {
        "full-source-map-publication": "fail-closed until emitted full source-map metadata exists on the canonical toolchain path",
        "lldb-plugin": "reserved; no checked-in LLDB plugin is published by this slice",
        "statement-level-stepping": "fail-closed until native line-table evidence is emitted",
    }
    return [
        _event(
            event_id=f"debug.reserved.{name}",
            phase="reserved",
            domain="debugger",
            event_kind="reserved-surface",
            subject=name,
            attributes={"status": "reserved", "reason": reason},
        )
        for name, reason in sorted(reserved.items())
    ]


def _runtime_trace_contract_events(
    runtime_trace_contracts: dict[str, Any],
) -> list[dict[str, Any]]:
    lanes = [
        _object_payload(lane)
        for lane in _list_payload(runtime_trace_contracts.get("lanes"))
    ]
    ordered = sorted(
        lanes,
        key=lambda lane: (
            str(lane.get("trace_domain", "")),
            str(lane.get("lane_id", "")),
        ),
    )
    return [
        _event(
            event_id=f"runtime.trace-contract.{_sanitize_event_id(lane.get('lane_id'))}",
            phase="runtime-source-contract",
            domain=str(lane.get("trace_domain", "") or "runtime"),
            event_kind=str(lane.get("event_kind", "") or "runtime-source-contract"),
            subject=str(lane.get("lane_id", "") or ""),
            attributes={
                "status": str(lane.get("status", "") or ""),
                "snapshot_header": str(lane.get("snapshot_header", "") or ""),
                "snapshot_type": str(lane.get("snapshot_type", "") or ""),
                "snapshot_symbol": str(lane.get("snapshot_symbol", "") or ""),
                "required_fields": _list_payload(lane.get("required_fields")),
                "source_anchor": str(lane.get("source_anchor", "") or ""),
                "source_path": str(lane.get("source_path", "") or ""),
                "source_line": int(lane.get("source_line", 0) or 0),
                "public_abi": lane.get("public_abi") is True,
            },
        )
        for lane in ordered
    ]


def _assign_ordinals(events: list[dict[str, Any]]) -> list[dict[str, Any]]:
    ordered: list[dict[str, Any]] = []
    for ordinal, event in enumerate(events):
        ordered.append({"ordinal": ordinal, **event})
    return ordered


def _event_counts(events: list[dict[str, Any]]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for event in events:
        kind = str(event.get("event_kind", "") or "unknown")
        counts[kind] = counts.get(kind, 0) + 1
    return dict(sorted(counts.items()))


def _native_debug_info_evidence(debug_map: dict[str, Any]) -> dict[str, Any]:
    evidence = _object_payload(debug_map.get("native_debug_info_evidence"))
    if evidence:
        return evidence
    source_identity = _object_payload(debug_map.get("object_model_source_identity"))
    return _object_payload(source_identity.get("native_debug_info_evidence"))


def _native_debug_info_blocker(debug_map: dict[str, Any]) -> str:
    evidence = _native_debug_info_evidence(debug_map)
    return str(
        evidence.get("fail_closed_reason", "")
        or debug_map.get("stepping_retired_route_reason", "")
        or "native line-table evidence is not emitted on the canonical toolchain path"
    )


def _support_boundary(debug_map: dict[str, Any]) -> dict[str, Any]:
    native_debug_info_evidence = _native_debug_info_evidence(debug_map)
    return {
        "debug_metadata_public_abi": False,
        "runtime_debug_surfaces": "internal-private-testing-snapshots",
        "lldb_plugin_status": "reserved",
        "source_map_supported": debug_map.get("source_map_supported") is True,
        "statement_level_stepping": debug_map.get("statement_level_stepping") is True,
        "statement_level_stepping_status": "fail-closed",
        "native_debug_info_evidence": native_debug_info_evidence,
        "native_debug_info_fail_closed_reason": _native_debug_info_blocker(debug_map),
        "debugger_model": str(debug_map.get("debugger_model", "") or ""),
    }


def _source_contract_rows(
    runtime_trace_contracts: dict[str, Any],
    *domains: str,
) -> list[dict[str, Any]]:
    wanted = set(domains)
    return [
        _object_payload(lane)
        for lane in _list_payload(runtime_trace_contracts.get("lanes"))
        if str(_object_payload(lane).get("trace_domain", "") or "") in wanted
    ]


def _source_anchor_ids(source_rows: list[dict[str, Any]]) -> list[str]:
    anchors = [
        str(row.get("source_anchor", "") or "")
        for row in source_rows
        if str(row.get("source_anchor", "") or "")
    ]
    return sorted(anchors)


def _source_contract_ids(source_rows: list[dict[str, Any]]) -> list[str]:
    return sorted(
        str(row.get("lane_id", "") or "")
        for row in source_rows
        if str(row.get("lane_id", "") or "")
    )


def _has_supported_source_rows(source_rows: list[dict[str, Any]]) -> bool:
    return bool(source_rows) and all(
        row.get("status") == "supported"
        and row.get("deterministic") is True
        and row.get("public_abi") is False
        and bool(row.get("source_anchor"))
        for row in source_rows
    )


def _trace_lane(
    *,
    status: str,
    support_class: str,
    scope: str = "",
    source_rows: list[dict[str, Any]] | None = None,
    source_anchors: list[str] | None = None,
    source_contract_ids: list[str] | None = None,
) -> dict[str, Any]:
    rows = source_rows or []
    anchors = source_anchors if source_anchors is not None else _source_anchor_ids(rows)
    contract_ids = (
        source_contract_ids if source_contract_ids is not None else _source_contract_ids(rows)
    )
    return {
        "status": status,
        "support_class": support_class,
        "scope": scope,
        "source_anchors": anchors,
        "source_contract_ids": contract_ids,
    }


def _trace_lanes(
    runtime_inspector: dict[str, Any],
    debug_map: dict[str, Any],
    runtime_trace_contracts: dict[str, Any],
) -> dict[str, Any]:
    dump_commands = _object_payload(runtime_inspector.get("dump_commands"))
    has_object_symbols = bool(dump_commands.get("object_symbols"))
    has_dispatch_model = bool(runtime_inspector.get("dispatch_cache_observability_model"))
    has_anchors = int(debug_map.get("declaration_breakpoint_anchor_count", 0) or 0) > 0
    task_rows = _source_contract_rows(runtime_trace_contracts, "task")
    actor_rows = _source_contract_rows(runtime_trace_contracts, "actor")
    dispatch_rows = _source_contract_rows(runtime_trace_contracts, "dispatch")
    object_rows = _source_contract_rows(runtime_trace_contracts, "object")
    memory_rows = _source_contract_rows(runtime_trace_contracts, "memory")
    error_rows = _source_contract_rows(runtime_trace_contracts, "error")
    task_actor_rows = [*task_rows, *actor_rows]
    return {
        "object_inspection": _trace_lane(
            status="supported" if runtime_inspector.get("available") is True else "reserved",
            support_class="runtime-inspector-object-artifact",
            scope="object artifact inspection plus runtime-owned object snapshot contract",
            source_rows=object_rows,
        ),
        "message_send_trace": _trace_lane(
            status="supported" if has_object_symbols and has_dispatch_model else "reserved",
            support_class="dispatch-cache-observation-and-object-symbols",
            scope="message-send metadata and cache observation; not an interactive step debugger",
            source_rows=dispatch_rows,
        ),
        "source_to_artifact_mapping": _trace_lane(
            status="supported" if has_anchors else "reserved",
            support_class="manifest-declaration-coordinate-anchors",
            scope="declaration coordinate anchors only; full source-map publication remains reserved",
            source_anchors=["debug-map.declaration-coordinate-anchors"] if has_anchors else [],
            source_contract_ids=[],
        ),
        "task_runtime_trace": _trace_lane(
            status="supported" if _has_supported_source_rows(task_rows) else "reserved",
            support_class="source-owned-task-runtime-snapshot-contract",
            scope="deterministic task/executor snapshot trace contract; not an interactive scheduler debugger",
            source_rows=task_rows,
        ),
        "actor_runtime_trace": _trace_lane(
            status="supported" if _has_supported_source_rows(actor_rows) else "reserved",
            support_class="source-owned-actor-runtime-snapshot-contract",
            scope="deterministic actor/mailbox snapshot trace contract",
            source_rows=actor_rows,
        ),
        "dispatch_runtime_trace": _trace_lane(
            status="supported" if _has_supported_source_rows(dispatch_rows) else "reserved",
            support_class="source-owned-dispatch-snapshot-contract",
            scope="deterministic dispatch/cache/message-send snapshot trace contract",
            source_rows=dispatch_rows,
        ),
        "object_runtime_trace": _trace_lane(
            status="supported" if _has_supported_source_rows(object_rows) else "reserved",
            support_class="source-owned-object-snapshot-contract",
            scope="deterministic object/class/query snapshot trace contract",
            source_rows=object_rows,
        ),
        "memory_runtime_trace": _trace_lane(
            status="supported" if _has_supported_source_rows(memory_rows) else "reserved",
            support_class="source-owned-memory-snapshot-contract",
            scope="deterministic ARC/autoreleasepool snapshot trace contract",
            source_rows=memory_rows,
        ),
        "statement_level_stepping": _trace_lane(
            status="reserved",
            support_class="line-table-evidence-not-emitted",
            scope="statement-level stepping remains fail-closed",
        ),
        "full_source_map_publication": _trace_lane(
            status="reserved",
            support_class="full-source-map-evidence-not-emitted",
            scope="full source-map publication remains fail-closed",
        ),
        "async_task_inspection": _trace_lane(
            status="supported" if _has_supported_source_rows(task_actor_rows) else "reserved",
            support_class="source-owned-task-actor-snapshot-contracts",
            scope="task, executor, and actor source contracts are traceable; no LLDB UI is claimed",
            source_rows=task_actor_rows,
        ),
        "error_unwind_trace": _trace_lane(
            status="supported" if _has_supported_source_rows(error_rows) else "reserved",
            support_class="source-owned-error-bridge-snapshot-contract",
            scope="deterministic thrown-error, bridge, foreign-exception, and catch-filter snapshot trace contract",
            source_rows=error_rows,
        ),
        "lldb_plugin": _trace_lane(
            status="reserved",
            support_class="not-published",
            scope="reserved until a checked-in LLDB plugin is published",
        ),
    }


def _inspection_commands(
    *,
    source_path: str,
    runtime_inspector: dict[str, Any],
    debug_map_path: str,
) -> dict[str, str]:
    dump_commands = _object_payload(runtime_inspector.get("dump_commands"))
    return {
        "runtime_debug_trace": f"npm run objc3c -- {TRACE_ACTION} {source_path}",
        "runtime_inspector": f"npm run objc3c -- inspect-runtime-inspector {source_path}",
        "compile_stage_trace": f"npm run objc3c -- trace-compile-stages {source_path}",
        "editor_tooling": f"npm run objc3c -- inspect-editor-tooling {source_path}",
        "runtime_trace_contract_source": "native/objc3c/src/runtime/debug/runtime_debug_trace_contracts.h",
        "editor_debug_map": f"Get-Content -Raw '{debug_map_path}'" if debug_map_path else "",
        "object_symbols": str(dump_commands.get("object_symbols", "") or ""),
        "object_sections": str(dump_commands.get("object_sections", "") or ""),
    }


def _supported_query(
    *,
    query_id: str,
    surface: str,
    support_class: str,
    public_command: str,
    evidence_input_labels: list[str],
    result_path: str,
    artifact_path: str = "",
    schema_path: str = "",
    source_span_ids: list[str] | None = None,
) -> dict[str, Any]:
    return {
        "query_id": query_id,
        "surface": surface,
        "status": "supported",
        "support_class": support_class,
        "public_command": public_command,
        "evidence_input_labels": evidence_input_labels,
        "result_path": result_path,
        "artifact_path": artifact_path,
        "schema_path": schema_path,
        "source_span_ids": source_span_ids or [],
        "unpublished_reason": "",
    }


def _reserved_query(
    *,
    query_id: str,
    surface: str,
    support_class: str,
    unpublished_reason: str,
    evidence_input_labels: list[str] | None = None,
) -> dict[str, Any]:
    return {
        "query_id": query_id,
        "surface": surface,
        "status": "reserved",
        "support_class": support_class,
        "public_command": "",
        "evidence_input_labels": evidence_input_labels or [],
        "result_path": "",
        "artifact_path": "",
        "schema_path": "",
        "source_span_ids": [],
        "unpublished_reason": unpublished_reason,
    }


def _anchor_span_id(anchor: dict[str, Any]) -> str:
    return (
        "source-span."
        f"{_sanitize_event_id(anchor.get('kind'))}."
        f"{_sanitize_event_id(anchor.get('symbol'))}."
        f"{int(anchor.get('line', 0) or 0)}."
        f"{int(anchor.get('column', 0) or 0)}"
    )


def _compiler_range_for_anchor(anchor: dict[str, Any]) -> dict[str, int]:
    line = int(anchor.get("line", 0) or 0)
    column = int(anchor.get("column", 0) or 0)
    symbol = str(anchor.get("symbol", "") or "")
    end_column = column + max(len(symbol), 1)
    return {
        "line": line,
        "column": column,
        "end_line": line,
        "end_column": end_column,
    }


def _lsp_range_for_compiler_range(compiler_range: dict[str, int]) -> dict[str, Any]:
    line = max(int(compiler_range.get("line", 0) or 0) - 1, 0)
    column = max(int(compiler_range.get("column", 0) or 0) - 1, 0)
    end_line = max(int(compiler_range.get("end_line", 0) or 0) - 1, line)
    end_column = max(int(compiler_range.get("end_column", 0) or 0) - 1, column)
    return {
        "start": {"line": line, "character": column},
        "end": {"line": end_line, "character": end_column},
    }


def _source_span_evidence(
    *,
    source_path: str,
    debug_map: dict[str, Any],
    runtime_inspector: dict[str, Any],
    debug_map_path: str,
) -> list[dict[str, Any]]:
    anchors = [
        _object_payload(anchor)
        for anchor in _list_payload(debug_map.get("declaration_breakpoints"))
    ]
    anchors = sorted(
        anchors,
        key=lambda anchor: (
            int(anchor.get("line", 0) or 0),
            int(anchor.get("column", 0) or 0),
            str(anchor.get("kind", "")),
            str(anchor.get("symbol", "")),
        ),
    )
    object_path = str(runtime_inspector.get("object_path", "") or "")
    object_command = str(
        _object_payload(runtime_inspector.get("dump_commands")).get("object_symbols", "")
        or ""
    )
    return [
        {
            "span_id": _anchor_span_id(anchor),
            "status": "supported",
            "source_path": source_path,
            "symbol": str(anchor.get("symbol", "") or ""),
            "kind": str(anchor.get("kind", "") or ""),
            "compiler_range": (compiler_range := _compiler_range_for_anchor(anchor)),
            "lsp_range": _lsp_range_for_compiler_range(compiler_range),
            "range_model": "compiler-1-based-plus-lsp-zero-based",
            "artifact_path": object_path,
            "artifact_query_command": object_command,
            "evidence_input_labels": ["debug_map", "runtime_inspector"],
            "source_record_path": debug_map_path,
            "public_command": f"npm run objc3c -- {TRACE_ACTION} {source_path}",
            "unsupported_expansion": [
                "statement-level stepping",
                "full source-map publication",
                "LLDB plugin breakpoint synchronization",
            ],
        }
        for anchor in anchors
    ]


def _inspection_queries(
    *,
    source_path: str,
    runtime_inspector: dict[str, Any],
    debug_map: dict[str, Any],
    path_records: dict[str, dict[str, Any]],
    runtime_trace_contracts: dict[str, Any],
    source_span_evidence: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    runtime_inspector_path = str(
        path_records.get("runtime_inspector", {}).get("path", "") or ""
    )
    debug_map_path = str(path_records.get("debug_map", {}).get("path", "") or "")
    trace_path = TRACE_REPORT_PATH_TEXT
    object_path = str(runtime_inspector.get("object_path", "") or "")
    schema_path = display_path(RUNTIME_DEBUG_TRACE_SCHEMA_PATH)
    source_contract_path = str(runtime_trace_contracts.get("source_path", "") or "")
    source_span_ids = [
        str(span.get("span_id", "") or "")
        for span in source_span_evidence
        if str(span.get("span_id", "") or "")
    ]
    native_debug_info_blocker = _native_debug_info_blocker(debug_map)
    return [
        _supported_query(
            query_id="debug.source-to-artifact.declaration-anchors",
            surface="source_to_artifact_mapping",
            support_class="manifest-declaration-coordinate-anchors",
            public_command=f"npm run objc3c -- inspect-editor-tooling {source_path}",
            evidence_input_labels=["debug_map", "editor_surface"],
            result_path=debug_map_path,
            artifact_path=object_path,
            source_span_ids=source_span_ids,
        ),
        _supported_query(
            query_id="runtime.object-inspection.object-symbols",
            surface="object_inspection",
            support_class="runtime-inspector-object-artifact",
            public_command=f"npm run objc3c -- inspect-runtime-inspector {source_path}",
            evidence_input_labels=["runtime_inspector"],
            result_path=runtime_inspector_path,
            artifact_path=object_path,
        ),
        _supported_query(
            query_id="runtime.message-send.dispatch-cache-observation",
            surface="message_send_trace",
            support_class="dispatch-cache-observation-and-object-symbols",
            public_command=f"npm run objc3c -- inspect-runtime-inspector {source_path}",
            evidence_input_labels=["runtime_inspector"],
            result_path=runtime_inspector_path,
            artifact_path=object_path,
        ),
        _supported_query(
            query_id="debug.runtime-trace.composed-event-sequence",
            surface="runtime_debug_trace",
            support_class="deterministic-composed-trace",
            public_command=f"npm run objc3c -- {TRACE_ACTION} {source_path}",
            evidence_input_labels=[
                "compile_stage_trace",
                "debug_map",
                "editor_surface",
                "runtime_inspector",
            ],
            result_path=trace_path,
            schema_path=schema_path,
            source_span_ids=source_span_ids,
        ),
        _supported_query(
            query_id="runtime.source-owned-trace-contracts",
            surface="runtime_trace_contracts",
            support_class="source-owned-runtime-snapshot-contracts",
            public_command=f"npm run objc3c -- {TRACE_ACTION} {source_path}",
            evidence_input_labels=["runtime_trace_contracts"],
            result_path=trace_path,
            artifact_path=source_contract_path,
        ),
        _supported_query(
            query_id="runtime.async-task-inspection.source-contracts",
            surface="async_task_inspection",
            support_class="source-owned-task-actor-snapshot-contracts",
            public_command=f"npm run objc3c -- {TRACE_ACTION} {source_path}",
            evidence_input_labels=["runtime_trace_contracts"],
            result_path=trace_path,
            artifact_path=source_contract_path,
        ),
        _supported_query(
            query_id="runtime.memory-inspection.source-contracts",
            surface="memory_runtime_trace",
            support_class="source-owned-memory-snapshot-contract",
            public_command=f"npm run objc3c -- {TRACE_ACTION} {source_path}",
            evidence_input_labels=["runtime_trace_contracts"],
            result_path=trace_path,
            artifact_path=source_contract_path,
        ),
        _supported_query(
            query_id="runtime.error-unwind-trace.snapshots",
            surface="error_unwind_trace",
            support_class="source-owned-error-bridge-snapshot-contract",
            public_command=f"npm run objc3c -- {TRACE_ACTION} {source_path}",
            evidence_input_labels=["runtime_trace_contracts"],
            result_path=trace_path,
            artifact_path=source_contract_path,
        ),
        _reserved_query(
            query_id="debug.statement-level-stepping.line-table",
            surface="statement_level_stepping",
            support_class="line-table-evidence-not-emitted",
            evidence_input_labels=["debug_map"],
            unpublished_reason=native_debug_info_blocker,
        ),
        _reserved_query(
            query_id="debug.full-source-map.publication",
            surface="full_source_map_publication",
            support_class="full-source-map-evidence-not-emitted",
            evidence_input_labels=["debug_map"],
            unpublished_reason="full source-map metadata is not emitted on the canonical toolchain path",
        ),
        _reserved_query(
            query_id="debug.lldb-plugin.integration",
            surface="lldb_plugin",
            support_class="not-published",
            unpublished_reason="no checked-in LLDB plugin is published by this slice",
        ),
    ]


def _source_mapping(debug_map: dict[str, Any]) -> dict[str, Any]:
    anchors = _list_payload(debug_map.get("declaration_breakpoints"))
    inline_frames = _list_payload(debug_map.get("inline_frames"))
    inline_failures = _list_payload(debug_map.get("inline_frame_failures"))
    span_ids = [
        _anchor_span_id(_object_payload(anchor))
        for anchor in anchors
    ]
    native_debug_info_evidence = _native_debug_info_evidence(debug_map)
    return {
        "model": "manifest-declaration-coordinate-anchors",
        "full_source_map_status": "reserved",
        "statement_level_stepping": debug_map.get("statement_level_stepping") is True,
        "native_debug_info_evidence_id": str(
            native_debug_info_evidence.get("evidence_id", "") or ""
        ),
        "native_debug_info_fail_closed_reason": _native_debug_info_blocker(debug_map),
        "inline_frame_status": str(debug_map.get("inline_frame_status", "") or "reserved"),
        "inline_frame_count": len(inline_frames),
        "inline_frame_failure_count": len(inline_failures),
        "inline_frame_ids": [
            str(_object_payload(frame).get("frame_id", "") or "")
            for frame in inline_frames
            if str(_object_payload(frame).get("frame_id", "") or "")
        ],
        "inline_frame_fail_closed_reason": str(
            debug_map.get("inline_frame_fail_closed_reason", "")
            or "inline-frame source-map records are not emitted on the canonical toolchain path"
        ),
        "declaration_anchor_count": len(anchors),
        "span_evidence_count": len(span_ids),
        "span_evidence_ids": span_ids,
        "anchors": anchors,
    }


def _runtime_inspection_summary(runtime_inspector: dict[str, Any]) -> dict[str, Any]:
    return {
        "contract_id": str(runtime_inspector.get("contract_id", "") or ""),
        "available": runtime_inspector.get("available") is True,
        "object_path": str(runtime_inspector.get("object_path", "") or ""),
        "snapshot_symbols": [
            {"field": field, "symbol": symbol}
            for field, symbol in _snapshot_symbol_fields(runtime_inspector)
        ],
        "runtime_models": [
            {"field": field, "model": model}
            for field, model in _runtime_model_fields(runtime_inspector)
        ],
        "dump_commands": _object_payload(runtime_inspector.get("dump_commands")),
    }


def _support_handoff(
    path_records: dict[str, dict[str, Any]],
    runtime_trace_contracts: dict[str, Any],
    debug_map: dict[str, Any],
) -> dict[str, Any]:
    evidence_ids = [
        "OBJ3-NEXT-023.schema.runtime-debug-trace.v1",
        "OBJ3-NEXT-023.script.trace-runtime-debug",
        "OBJ3-NEXT-023.fixture.runtime-debug-trace",
        "OBJ3-NEXT-023.test.runtime-debug-trace-surface",
        "OBJ3-NEXT-023.command.trace-runtime-debug",
        "OBJ3-NEXT-023.source.runtime-debug-trace-contracts",
    ]
    required_input_labels = [
        "compile_stage_trace",
        "debug_map",
        "editor_surface",
        "runtime_inspector",
    ]
    lanes = [
        _object_payload(lane)
        for lane in _list_payload(runtime_trace_contracts.get("lanes"))
    ]
    all_source_anchors = _source_anchor_ids(lanes)
    task_actor_anchors = _source_anchor_ids(
        [
            lane
            for lane in lanes
            if str(lane.get("trace_domain", "") or "") in {"task", "actor"}
        ]
    )
    memory_anchors = _source_anchor_ids(
        [
            lane
            for lane in lanes
            if str(lane.get("trace_domain", "") or "") == "memory"
        ]
    )
    error_anchors = _source_anchor_ids(
        [
            lane
            for lane in lanes
            if str(lane.get("trace_domain", "") or "") == "error"
        ]
    )
    native_debug_info_blocker = _native_debug_info_blocker(debug_map)
    return {
        "capability_rows": [
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace",
                "status": "supported",
                "evidence_ids": evidence_ids,
                "required_input_labels": required_input_labels,
                "required_inputs_available": all(
                    path_records.get(label, {}).get("available") is True
                    for label in required_input_labels
                ),
                "source_anchors": all_source_anchors,
                "source_contract_id": runtime_trace_contracts.get("contract_id", ""),
                "claim_boundary": "deterministic trace composition from runtime inspector, compile-stage trace, editor surface, and debug map inputs",
            },
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace.lldb_plugin",
                "status": "reserved",
                "evidence_ids": ["OBJ3-NEXT-023.schema.runtime-debug-trace.v1"],
                "unpublished_reason": "no checked-in LLDB plugin is published by this slice",
            },
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace.statement_stepping",
                "status": "reserved",
                "evidence_ids": ["OBJ3-NEXT-023.fixture.runtime-debug-trace"],
                "unpublished_reason": native_debug_info_blocker,
            },
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace.full_source_map",
                "status": "reserved",
                "evidence_ids": ["OBJ3-NEXT-023.fixture.runtime-debug-trace"],
                "unpublished_reason": "full source-map metadata is not emitted on the canonical toolchain path",
            },
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace.async_tasks",
                "status": "supported",
                "evidence_ids": [
                    "OBJ3-NEXT-023.schema.runtime-debug-trace.v1",
                    "OBJ3-NEXT-023.source.runtime-debug-trace-contracts",
                ],
                "source_anchors": task_actor_anchors,
                "source_contract_id": runtime_trace_contracts.get("contract_id", ""),
                "claim_boundary": "task/executor/actor inspection is source-contract backed and emitted as deterministic trace contract rows",
            },
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace.memory",
                "status": "supported",
                "evidence_ids": [
                    "OBJ3-NEXT-023.schema.runtime-debug-trace.v1",
                    "OBJ3-NEXT-023.source.runtime-debug-trace-contracts",
                ],
                "source_anchors": memory_anchors,
                "source_contract_id": runtime_trace_contracts.get("contract_id", ""),
                "claim_boundary": "ARC/autoreleasepool inspection is source-contract backed and emitted as deterministic trace contract rows",
            },
            {
                "capability_id": "objc3c.behavior.runtime.debug_trace.error_unwind",
                "status": "supported",
                "evidence_ids": [
                    "OBJ3-NEXT-023.schema.runtime-debug-trace.v1",
                    "OBJ3-NEXT-023.source.runtime-debug-trace-contracts",
                ],
                "source_anchors": error_anchors,
                "source_contract_id": runtime_trace_contracts.get("contract_id", ""),
                "claim_boundary": "error bridge, foreign exception mapping, and catch-filter inspection is source-contract backed and emitted as deterministic trace contract rows",
            },
        ],
        "evidence_ids": evidence_ids,
    }


def _payload_failures(
    runtime_inspector: dict[str, Any],
    stage_trace: dict[str, Any],
    editor_surface: dict[str, Any],
    debug_map: dict[str, Any],
    initial_failures: list[str],
) -> list[str]:
    failures = list(initial_failures)
    if runtime_inspector.get("contract_id") != RUNTIME_INSPECTOR_CONTRACT_ID:
        failures.append("runtime inspector contract id drifted")
    if stage_trace.get("mode") != COMPILE_STAGE_TRACE_MODE:
        failures.append("compile stage trace mode drifted")
    if editor_surface.get("contract_id") != EDITOR_SURFACE_CONTRACT_ID:
        failures.append("editor surface contract id drifted")
    if debug_map.get("contract_id") != DEBUG_MAP_CONTRACT_ID:
        failures.append("debug map contract id drifted")
    if debug_map.get("statement_level_stepping") is True:
        failures.append("statement-level stepping unexpectedly published")
    return failures


def build_runtime_debug_trace_payload(
    *,
    source_path: str,
    runtime_inspector: dict[str, Any],
    stage_trace: dict[str, Any],
    editor_surface: dict[str, Any],
    debug_map: dict[str, Any],
    input_paths: dict[str, Path | str],
    steps: list[dict[str, Any]] | None = None,
    initial_failures: list[str] | None = None,
    runtime_trace_contracts: dict[str, Any] | None = None,
) -> dict[str, Any]:
    source_contracts = runtime_trace_contracts or load_runtime_trace_source_contracts()
    failures = _payload_failures(
        runtime_inspector,
        stage_trace,
        editor_surface,
        debug_map,
        list(initial_failures or []),
    )
    events = _assign_ordinals(
        [
            *_stage_events(stage_trace),
            *_runtime_inspector_events(runtime_inspector),
            *_debug_anchor_events(debug_map),
            *_runtime_trace_contract_events(source_contracts),
            *_reserved_surface_events(),
        ]
    )
    trace_digest = stable_digest(
        {
            "source_path": source_path,
            "events": events,
            "support_boundary": _support_boundary(debug_map),
            "trace_lanes": _trace_lanes(runtime_inspector, debug_map, source_contracts),
            "runtime_trace_contracts": source_contracts,
        }
    )
    path_records = {
        label: _path_record(path, label=label)
        for label, path in sorted(input_paths.items())
    }
    debug_map_path = str(path_records.get("debug_map", {}).get("path", "") or "")
    source_span_evidence = _source_span_evidence(
        source_path=source_path,
        debug_map=debug_map,
        runtime_inspector=runtime_inspector,
        debug_map_path=debug_map_path,
    )
    return {
        "contract_id": RUNTIME_DEBUG_TRACE_CONTRACT_ID,
        "schema_id": RUNTIME_DEBUG_TRACE_SCHEMA_ID,
        "schema_version": RUNTIME_DEBUG_TRACE_SCHEMA_VERSION,
        "schema_path": display_path(RUNTIME_DEBUG_TRACE_SCHEMA_PATH),
        "source_path": source_path,
        "ok": not failures,
        "failures": failures,
        "trace_model": "deterministic-runtime-inspector-and-editor-debug-artifact-trace",
        "inputs": path_records,
        "artifacts": {
            "report_path": TRACE_REPORT_PATH_TEXT,
            "runtime_inspector_contract_id": str(runtime_inspector.get("contract_id", "") or ""),
            "compile_stage_trace_mode": str(stage_trace.get("mode", "") or ""),
            "editor_surface_contract_id": str(editor_surface.get("contract_id", "") or ""),
            "debug_map_contract_id": str(debug_map.get("contract_id", "") or ""),
        },
        "source_mapping": _source_mapping(debug_map),
        "source_span_evidence": source_span_evidence,
        "runtime_inspection": _runtime_inspection_summary(runtime_inspector),
        "runtime_trace_contracts": source_contracts,
        "trace_lanes": _trace_lanes(runtime_inspector, debug_map, source_contracts),
        "inspection_queries": _inspection_queries(
            source_path=source_path,
            runtime_inspector=runtime_inspector,
            debug_map=debug_map,
            path_records=path_records,
            runtime_trace_contracts=source_contracts,
            source_span_evidence=source_span_evidence,
        ),
        "support_boundary": _support_boundary(debug_map),
        "event_sequence": events,
        "event_counts": _event_counts(events),
        "determinism": {
            "ordering": "compile-stage-order-then-runtime-inspector-then-debug-anchor-then-source-contracts-then-reserved-surface",
            "trace_digest": trace_digest,
            "digest_excludes": ["steps.duration_ms", "steps.stdout_snippet", "steps.stderr_snippet"],
        },
        "inspection_commands": _inspection_commands(
            source_path=source_path,
            runtime_inspector=runtime_inspector,
            debug_map_path=debug_map_path,
        ),
        "support_handoff": _support_handoff(path_records, source_contracts, debug_map),
        "steps": steps or [],
    }
