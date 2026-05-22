from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from format_objc3c_source import build_format_summary_for_source
from objc3c_tooling.paths import display_path

from objc3c_editor_tooling.artifact_inspector import build_artifact_inspector_payload
from objc3c_editor_tooling.diagnostic_bridge import build_lsp_diagnostic_transport
from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.paths import EditorToolingPaths
from objc3c_editor_tooling.source_index import build_source_graph, build_source_index
from objc3c_editor_tooling.validation import diagnostics_entries, source_graph_consumer_status
from objc3c_editor_tooling.workspace_index import (
    build_workspace_index,
    document_symbol_records,
)

OBJECT_MODEL_SOURCE_IDENTITY_CONTRACT_ID = "objc3c.object_model.production.source_identity.v1"
OBJECT_MODEL_SOURCE_IDENTITY_KINDS = ("class", "category", "protocol", "property", "ivar", "method")


@dataclass(frozen=True)
class EditorToolingModel:
    language_server: dict[str, Any]
    navigation: dict[str, Any]
    workspace_index: dict[str, Any]
    artifact_inspector: dict[str, Any]
    formatter: dict[str, Any]
    formatted_source_text: str
    debug: dict[str, Any]
    source_index: dict[str, Any]
    source_graph: dict[str, Any]
    symbols: list[dict[str, Any]]


def extract_symbols(manifest: dict[str, Any]) -> list[dict[str, Any]]:
    symbol_groups = [
        ("globals", "global"),
        ("functions", "function"),
        ("interfaces", "interface"),
        ("implementations", "implementation"),
        ("protocols", "protocol"),
        ("categories", "category"),
    ]
    symbols: list[dict[str, Any]] = []
    for key, kind in symbol_groups:
        entries = manifest.get(key, [])
        if not isinstance(entries, list):
            continue
        for entry in entries:
            if not isinstance(entry, dict):
                continue
            name = entry.get("name")
            if key == "implementations":
                name = entry.get("class_name")
            if key == "categories":
                class_name = entry.get("class_name", "")
                category_name = entry.get("category_name", "")
                if class_name or category_name:
                    name = f"{class_name}({category_name})"
            if not isinstance(name, str) or not name:
                continue
            line = int(entry.get("line", 0) or 0)
            column = int(entry.get("column", 0) or 0)
            symbols.append(
                {
                    "name": name,
                    "kind": kind,
                    "line": line,
                    "column": column,
                    "end_line": line,
                    "end_column": column + len(name),
                }
            )
    symbols.sort(key=lambda symbol: (symbol["line"], symbol["column"], symbol["kind"], symbol["name"]))
    return symbols


def build_language_server_payload(
    summary: dict[str, Any],
    manifest_path_text: str | None,
    symbols: list[dict[str, Any]],
    workspace_index: dict[str, Any] | None = None,
    *,
    source_path: str = "",
    diagnostic_entries: list[Any] | None = None,
    source_index: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
) -> dict[str, Any]:
    manifest_available = bool(manifest_path_text)
    workspace_index_available = (
        bool(workspace_index)
        and workspace_index.get("available") is True
        and int(workspace_index.get("package_count", 0) or 0) > 1
    )
    diagnostic_transport = build_lsp_diagnostic_transport(
        source_path,
        diagnostic_entries or [],
    )
    code_action_available = int(diagnostic_transport["code_action_count"]) > 0
    source_index_available = bool(source_index) and source_index.get("available") is True
    hover_available = source_index_available and int(source_index.get("declaration_count", 0) or 0) > 0
    capability_evidence = {
        "publishDiagnostics": ["diagnostics-json"],
        "documentSymbol": ["compile-manifest-declaration-coordinates"],
        "workspaceSymbol": [
            "compile-manifest-declaration-coordinates",
            "workspace-semantic-index-guardrails",
        ],
        "definition": ["compile-manifest-declaration-coordinates"],
        "hover": [
            "compile-manifest-declaration-coordinates",
            "source-derived-editor-index",
        ],
        "references": [
            "compiler-source-graph-artifact",
            "semantic-reference-closure",
        ],
        "rename": [
            "compiler-source-graph-artifact",
            "semantic-reference-closure",
            "rename-safety-diagnostics",
        ],
        "semanticTokens": [
            "compiler-source-graph-artifact",
            "semantic-token-payload",
        ],
        "codeAction": ["diagnostics-json-fixits"],
    }
    references_status = source_graph_consumer_status(
        source_graph,
        "references",
        "not published; compiler-owned semantic reference closure is not available",
    )
    rename_status = source_graph_consumer_status(
        source_graph,
        "rename",
        "not published; safe rename requires compiler-owned semantic references and rename safety diagnostics",
    )
    semantic_tokens_status = source_graph_consumer_status(
        source_graph,
        "semanticTokens",
        "not published; semantic tokens require compiler-owned token classification",
    )
    capability_statuses = {
        "publishDiagnostics": {
            "supported": True,
            "support_class": "authoritative",
            "evidence": "diagnostics-json",
            "evidence_ids": capability_evidence["publishDiagnostics"],
            "fail_closed": False,
        },
        "documentSymbol": {
            "supported": manifest_available,
            "support_class": "manifest-backed" if manifest_available else "fail-closed",
            "evidence_ids": capability_evidence["documentSymbol"] if manifest_available else [],
            "fail_closed": not manifest_available,
            "unpublished_reason": "" if manifest_available else "disabled until compile emits manifest declarations",
        },
        "workspaceSymbol": {
            "supported": manifest_available and workspace_index_available,
            "support_class": "workspace-index-backed"
            if manifest_available and workspace_index_available
            else "fail-closed",
            "evidence_ids": capability_evidence["workspaceSymbol"]
            if manifest_available and workspace_index_available
            else [],
            "fail_closed": not (manifest_available and workspace_index_available),
            "unpublished_reason": ""
            if manifest_available and workspace_index_available
            else "disabled until compile emits manifest declarations and workspace package index guardrails pass",
        },
        "definition": {
            "supported": manifest_available and bool(symbols),
            "support_class": "manifest-backed" if manifest_available and symbols else "fail-closed",
            "evidence_ids": capability_evidence["definition"] if manifest_available and symbols else [],
            "fail_closed": not (manifest_available and bool(symbols)),
            "unpublished_reason": "" if manifest_available and symbols else "disabled until compile emits declaration coordinates",
        },
        "hover": {
            "supported": hover_available,
            "support_class": "source-index-backed" if hover_available else "fail-closed",
            "evidence_ids": capability_evidence["hover"] if hover_available else [],
            "fail_closed": not hover_available,
            "unpublished_reason": ""
            if hover_available
            else "disabled until the source index has manifest-backed declarations",
        },
        "references": references_status,
        "rename": rename_status,
        "semanticTokens": semantic_tokens_status,
        "codeAction": {
            "supported": code_action_available,
            "support_class": "diagnostics-fixit-backed"
            if code_action_available
            else "fail-closed",
            "evidence": "diagnostics-json-fixits" if code_action_available else "",
            "evidence_ids": capability_evidence["codeAction"] if code_action_available else [],
            "fail_closed": not code_action_available,
            "unpublished_reason": ""
            if code_action_available
            else "disabled until diagnostics emit machine-applicable fix-its",
        },
        "statementLevelStepping": {
            "supported": False,
            "support_class": "fail-closed-unpublished",
            "evidence_ids": [],
            "fail_closed": True,
            "unpublished_reason": "not published; statement stepping remains fail-closed pending line-table evidence",
        },
    }
    supported_capabilities = [
        capability_id
        for capability_id, status in capability_statuses.items()
        if status["supported"] is True
    ]
    unpublished_capabilities = [
        capability_id
        for capability_id, status in capability_statuses.items()
        if status["supported"] is False
    ]
    return {
        "contract_id": "objc3c.developer.tooling.language.server.capability.surface.v1",
        "summary_status_name": summary.get("observability", {}).get("status_name", ""),
        "manifest_backed_navigation": manifest_available,
        "workspace_index_backed_navigation": workspace_index_available,
        "diagnostic_transport": diagnostic_transport,
        "source_index_backed_hover": hover_available,
        "source_index_digest": str(source_index.get("source_index_digest", "") or "")
        if isinstance(source_index, dict)
        else "",
        "source_graph_backed_references": references_status["supported"] is True,
        "source_graph_digest": str(source_graph.get("source_graph_digest", "") or "")
        if isinstance(source_graph, dict)
        else "",
        "capability_evidence_roots": capability_evidence,
        "publication_boundary": "diagnostics, compile-owned declaration coordinates, source-index hover, workspace guardrails, source-graph-backed consumers, and diagnostic fix-its publish positive LSP rows only when their evidence is complete",
        "supported_capability_ids": supported_capabilities,
        "unpublished_capability_ids": unpublished_capabilities,
        "capability_statuses": capability_statuses,
    }


def build_navigation_payload(
    source_display: str,
    module_name: str,
    manifest_path_text: str | None,
    symbols: list[dict[str, Any]],
    workspace_index: dict[str, Any],
    source_index: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
) -> dict[str, Any]:
    document_symbols = document_symbol_records(source_display, module_name, symbols)
    workspace_symbols = [
        *document_symbols,
        *workspace_index.get("package_symbols", []),
    ]
    definition_targets = [
        {
            "name": record["name"],
            "kind": record["kind"],
            "target_uri": record["definition"]["target_uri"],
            "target_range": record["definition"]["target_range"],
            "target_compiler_range": record["definition"]["target_compiler_range"],
        }
        for record in document_symbols
    ]
    source_index = source_index or {}
    hover_targets = [
        {
            "name": declaration["symbol"],
            "kind": declaration["kind"],
            "contents": declaration["hover"]["contents"],
            "target_uri": declaration["definition"]["target_uri"],
            "target_range": declaration["definition"]["target_range"],
            "target_compiler_range": declaration["definition"]["target_compiler_range"],
        }
        for declaration in source_index.get("declarations", [])
        if isinstance(declaration, dict)
    ]
    return {
        "contract_id": "objc3c.developer.tooling.navigation.index.v1",
        "source_path": source_display,
        "available": bool(manifest_path_text),
        "manifest_path": manifest_path_text,
        "symbol_count": len(symbols),
        "supported_symbol_kinds": sorted({symbol["kind"] for symbol in symbols}),
        "symbols": symbols,
        "document_symbols": document_symbols,
        "workspace_symbols": workspace_symbols,
        "definition_targets": definition_targets,
        "hover_targets": hover_targets,
        "source_index": source_index,
        "source_graph_digest": str(source_graph.get("source_graph_digest", "") or "")
        if isinstance(source_graph, dict)
        else "",
        "source_graph_navigation": source_graph.get("navigation_consumers", {})
        if isinstance(source_graph, dict)
        else {},
        "workspace_index": workspace_index,
        "retired_route_reason": "" if manifest_path_text else "compile produced no manifest-backed declaration surface",
    }


def _as_dict(value: Any) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _as_list(value: Any) -> list[Any]:
    return value if isinstance(value, list) else []


def _safe_int(value: Any) -> int:
    if isinstance(value, bool):
        return 0
    try:
        return int(value or 0)
    except (TypeError, ValueError):
        return 0


def _safe_text(value: Any) -> str:
    return value if isinstance(value, str) else ""


def _first_text(record: dict[str, Any], *keys: str) -> str:
    for key in keys:
        value = record.get(key)
        if isinstance(value, str) and value:
            return value
    return ""


def _identity_slug(*parts: str) -> str:
    raw = ".".join(part for part in parts if part)
    if not raw:
        return "anonymous"
    normalized = "".join(char.lower() if char.isalnum() else "_" for char in raw)
    slug = "_".join(part for part in normalized.split("_") if part)
    return slug or "anonymous"


def _selector_symbol(selector: str) -> str:
    if not selector:
        return ""
    normalized = "".join(char if char.isalnum() else "_" for char in selector)
    while "__" in normalized:
        normalized = normalized.replace("__", "_")
    return normalized or ""


def _method_native_symbol(record: dict[str, Any]) -> str:
    owner_name = _first_text(record, "owner_name", "class_name", "declaring_type", "type_name")
    selector = _first_text(record, "selector", "method_name", "name")
    if not owner_name or not selector:
        return ""
    owner_symbol = owner_name.split("(", 1)[0]
    method_kind = "class" if record.get("is_class_method") is True else "instance"
    selector_symbol = _selector_symbol(selector)
    if not owner_symbol or not selector_symbol:
        return ""
    return f"objc3_method_{owner_symbol}_{method_kind}_{selector_symbol}"


def _runtime_identity_display_name(kind: str, record: dict[str, Any]) -> str:
    if kind == "category":
        class_name = _first_text(record, "class_name", "owner_name")
        category_name = _first_text(record, "category_name", "name")
        if class_name or category_name:
            return f"{class_name}({category_name})"
    if kind == "property":
        owner_name = _first_text(record, "owner_name", "class_name")
        property_name = _first_text(record, "property_name", "name")
        return f"{owner_name}.{property_name}" if owner_name and property_name else property_name
    if kind == "ivar":
        owner_name = _first_text(record, "owner_name", "class_name")
        ivar_name = _first_text(record, "ivar_name", "property_name", "name")
        return f"{owner_name}.{ivar_name}" if owner_name and ivar_name else ivar_name
    if kind == "method":
        owner_name = _first_text(record, "owner_name", "class_name")
        selector = _first_text(record, "selector", "method_name", "name")
        prefix = "+" if record.get("is_class_method") is True else "-"
        return f"{prefix}[{owner_name} {selector}]" if owner_name and selector else selector
    return _first_text(record, "name", "class_name", "owner_name")


def _source_map_record_kind(identity_kind: str) -> str:
    if identity_kind in {"class", "category", "protocol"}:
        return "declaration"
    if identity_kind == "property":
        return "property-access"
    if identity_kind == "ivar":
        return "generated-accessor"
    return "method"


def _record_line_column(record: dict[str, Any]) -> tuple[int, int]:
    line = _safe_int(record.get("line"))
    column = _safe_int(record.get("column"))
    if column == 0:
        column = _safe_int(record.get("start_column"))
    return line, column


def _append_source_identity_record(
    records: list[dict[str, Any]],
    rows: list[dict[str, Any]],
    *,
    identity_kind: str,
    manifest_section: str,
    manifest_record_index: int,
    record: dict[str, Any],
    source_path: str,
) -> None:
    display_name = _runtime_identity_display_name(identity_kind, record)
    if not display_name:
        return
    line, column = _record_line_column(record)
    record_slug = _identity_slug(identity_kind, manifest_section, display_name, str(manifest_record_index))
    source_map_record_id = f"object-model.source-map.{record_slug}"
    line_row_id = f"object-model.native-line-table.{record_slug}"
    selector = _first_text(record, "selector", "method_name", "name") if identity_kind == "method" else ""
    source_map_record = {
        "source_map_record_id": source_map_record_id,
        "runtime_identity_kind": identity_kind,
        "source_map_record_kind": _source_map_record_kind(identity_kind),
        "source_truth_kind": "canonical-frontend-runtime-metadata-manifest",
        "source_path": source_path,
        "line": line,
        "column": column,
        "end_line": line,
        "end_column": column + max(len(display_name), 1),
        "display_name": display_name,
        "owner_name": _first_text(record, "owner_name", "class_name", "declaring_type", "type_name"),
        "selector": selector,
        "property_name": _first_text(record, "property_name", "name") if identity_kind == "property" else "",
        "ivar_name": _first_text(record, "ivar_name", "property_name", "name") if identity_kind == "ivar" else "",
        "manifest_section": manifest_section,
        "manifest_record_index": manifest_record_index,
        "native_line_table_row_id": line_row_id,
    }
    records.append(source_map_record)
    rows.append(
        {
            "row_id": line_row_id,
            "source_map_record_id": source_map_record_id,
            "runtime_identity_kind": identity_kind,
            "source_path": source_path,
            "line": line,
            "column": column,
            "native_line_table_model": "compiler-manifest-coordinate-projection",
            "native_debug_info_emitted": False,
            "expected_native_symbol": _method_native_symbol(record) if identity_kind == "method" else "",
        }
    )


def _iter_manifest_identity_records(manifest: dict[str, Any]) -> list[tuple[str, str, int, dict[str, Any]]]:
    runtime_records = _as_dict(manifest.get("runtime_metadata_source_records"))
    groups: tuple[tuple[str, str, list[Any]], ...] = (
        ("class", "interfaces", _as_list(manifest.get("interfaces"))),
        ("category", "categories", _as_list(manifest.get("categories"))),
        ("protocol", "protocols", _as_list(manifest.get("protocols"))),
        ("property", "runtime_metadata_source_records.properties", _as_list(runtime_records.get("properties"))),
        ("ivar", "runtime_metadata_source_records.ivars", _as_list(runtime_records.get("ivars"))),
        ("method", "runtime_metadata_source_records.methods", _as_list(runtime_records.get("methods"))),
    )
    entries: list[tuple[str, str, int, dict[str, Any]]] = []
    for identity_kind, section, records in groups:
        for index, record in enumerate(records):
            if isinstance(record, dict):
                entries.append((identity_kind, section, index, record))
    return entries


def build_object_model_source_identity_payload(
    manifest: dict[str, Any] | None,
    source_graph: dict[str, Any] | None,
    source_path: str,
) -> dict[str, Any]:
    manifest = _as_dict(manifest)
    source_graph = _as_dict(source_graph)
    manifest_source = _safe_text(manifest.get("source"))
    resolved_source_path = source_path or manifest_source
    records: list[dict[str, Any]] = []
    rows: list[dict[str, Any]] = []
    for identity_kind, section, index, record in _iter_manifest_identity_records(manifest):
        _append_source_identity_record(
            records,
            rows,
            identity_kind=identity_kind,
            manifest_section=section,
            manifest_record_index=index,
            record=record,
            source_path=resolved_source_path,
        )
    row_ids = {row["row_id"] for row in rows}
    stepping_candidates = [
        {
            "source_map_record_id": record["source_map_record_id"],
            "native_line_table_row_id": record["native_line_table_row_id"],
            "owner_name": record["owner_name"],
            "selector": record["selector"],
            "source_path": record["source_path"],
            "line": record["line"],
            "column": record["column"],
            "runtime_debug_trace_step_id": f"object-model.step.{_identity_slug(record['owner_name'], record['selector'])}",
            "status": "source-identity-ready-stepping-blocked",
            "blocked_by": [
                "native-debug-line-emission",
                "runtime-debug-trace-statement-stepping-integration",
            ],
        }
        for record in records
        if record["runtime_identity_kind"] == "method"
        and record["native_line_table_row_id"] in row_ids
    ]
    identity_kinds = sorted({record["runtime_identity_kind"] for record in records})
    supported = bool(records and rows and resolved_source_path)
    return {
        "contract_id": OBJECT_MODEL_SOURCE_IDENTITY_CONTRACT_ID,
        "supported": supported,
        "source_path": resolved_source_path,
        "source_graph_digest": _safe_text(source_graph.get("source_graph_digest")),
        "source_truth_model": "canonical-frontend-manifest-runtime-metadata-source-records",
        "runs_on_canonical_frontend_manifest": bool(manifest),
        "source_map_records_supported": bool(records),
        "native_line_table_projection_supported": bool(rows),
        "method_stepping_candidates_supported": bool(stepping_candidates),
        "full_source_map_publication": False,
        "runtime_debug_trace_statement_stepping": False,
        "native_debug_info_emitted": False,
        "source_map_record_count": len(records),
        "native_line_table_row_count": len(rows),
        "stepping_candidate_count": len(stepping_candidates),
        "identity_kinds": identity_kinds,
        "required_identity_kinds_present": [
            kind for kind in OBJECT_MODEL_SOURCE_IDENTITY_KINDS if kind in identity_kinds
        ],
        "source_map_records": records,
        "native_line_table_rows": rows,
        "stepping_candidates": stepping_candidates,
        "fail_closed_boundaries": [
            {
                "capability_id": "fullSourceMapPublication",
                "status": "reserved",
                "fail_closed": True,
                "unpublished_reason": "full source-map metadata is not emitted on the canonical toolchain path",
            },
            {
                "capability_id": "statementLevelStepping",
                "status": "reserved",
                "fail_closed": True,
                "unpublished_reason": "runtime-debug trace statement stepping is not integrated with emitted native debug info",
            },
        ],
    }


def build_debug_payload(
    summary: dict[str, Any],
    object_path_text: str | None,
    symbols: list[dict[str, Any]],
    *,
    manifest: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
    source_path: str = "",
) -> dict[str, Any]:
    runtime_inspector = summary.get("runtime_inspector", {})
    dump_commands = runtime_inspector.get("dump_commands", {}) if isinstance(runtime_inspector, dict) else {}
    object_symbols = dump_commands.get("object_symbols", "") if isinstance(dump_commands, dict) else ""
    object_sections = dump_commands.get("object_sections", "") if isinstance(dump_commands, dict) else ""
    declaration_breakpoints = [
        {
            "symbol": symbol["name"],
            "kind": symbol["kind"],
            "line": symbol["line"],
            "column": symbol["column"],
        }
        for symbol in symbols
    ]
    supported = bool(object_path_text) or bool(declaration_breakpoints)
    evidence_roots = ["compile-manifest-declaration-coordinates"] if declaration_breakpoints else []
    if object_path_text and object_symbols:
        evidence_roots.append("runtime-inspector-object-symbol-inventory")
    object_model_source_identity = build_object_model_source_identity_payload(
        manifest,
        source_graph,
        source_path,
    )
    if object_model_source_identity["supported"]:
        evidence_roots.append("object-model-production-source-identity")
    return {
        "contract_id": "objc3c.developer.tooling.debug.map.surface.v1",
        "supported": supported,
        "support_class": "declaration-breakpoint-preview" if supported else "fail-closed",
        "debugger_model": "declaration-breakpoint-and-object-symbol-inspection",
        "source_map_supported": False,
        "source_map_model": "declaration-coordinate-only",
        "statement_level_stepping": False,
        "stepping_retired_route_reason": "statement-level stepping remains fail-closed until emitted line-table evidence exists on the canonical toolchain path",
        "object_artifact_present": bool(object_path_text),
        "object_path": object_path_text,
        "declaration_breakpoint_anchor_count": len(declaration_breakpoints),
        "declaration_breakpoints": declaration_breakpoints,
        "object_section_inventory_command": object_sections,
        "object_symbol_inventory_command": object_symbols,
        "runtime_debug_trace_command": "npm run objc3c -- trace-runtime-debug",
        "runtime_debug_trace_path": "tmp/reports/objc3c-public-workflow/runtime-debug-trace.json",
        "runtime_debug_trace_schema": "schemas/objc3c-runtime-debug-trace-v1.schema.json",
        "runtime_debug_trace_model": "deterministic-runtime-inspector-and-editor-debug-artifact-trace",
        "runtime_inspector_contract_id": runtime_inspector.get("contract_id", "") if isinstance(runtime_inspector, dict) else "",
        "artifact_inspection_ready": bool(object_path_text and object_symbols),
        "object_model_source_identity": object_model_source_identity,
        "evidence_roots": evidence_roots,
        "reserved_capability_rows": [
            {
                "capability_id": "statementLevelStepping",
                "status": "reserved",
                "fail_closed": True,
                "unpublished_reason": "line-table evidence is not emitted on the canonical toolchain path",
            },
            {
                "capability_id": "fullSourceMapPublication",
                "status": "reserved",
                "fail_closed": True,
                "unpublished_reason": "full source-map metadata is not emitted on the canonical toolchain path",
            },
        ],
        "retired_route_reason": "" if supported else "compile produced no object artifact or declaration coordinates for preview debug anchors",
    }


def build_editor_tooling_model(paths: EditorToolingPaths, inputs: EditorToolingInputs) -> EditorToolingModel:
    symbols = extract_symbols(inputs.manifest)
    diagnostic_entries = diagnostics_entries(inputs.diagnostics)
    module_name = str(inputs.manifest.get("module") or paths.source.path.stem)
    summary_paths = inputs.summary.get("paths", {})
    source_index = build_source_index(
        source_path=paths.source.display_path,
        module_name=module_name,
        source_text=inputs.source_text,
        manifest_path=inputs.manifest_path_text,
        symbols=symbols,
        diagnostics=diagnostic_entries,
        artifact_paths=summary_paths if isinstance(summary_paths, dict) else {},
    )
    workspace_index = build_workspace_index(
        paths.source.display_path,
        module_name,
        inputs.manifest_path_text,
        symbols,
        source_index,
    )
    source_graph = build_source_graph(
        source_path=paths.source.display_path,
        module_name=module_name,
        manifest_path=inputs.manifest_path_text,
        manifest=inputs.manifest,
        symbols=symbols,
        source_index=source_index,
        workspace_index=workspace_index,
    )
    formatted_text, formatter = build_format_summary_for_source(
        paths.source.display_path,
        inputs.source_text,
        display_path(paths.formatted_source),
    )
    return EditorToolingModel(
        language_server=build_language_server_payload(
            inputs.summary,
            inputs.manifest_path_text,
            symbols,
            workspace_index,
            source_path=paths.source.display_path,
            diagnostic_entries=diagnostic_entries,
            source_index=source_index,
            source_graph=source_graph,
        ),
        navigation=build_navigation_payload(
            paths.source.display_path,
            module_name,
            inputs.manifest_path_text,
            symbols,
            workspace_index,
            source_index,
            source_graph,
        ),
        workspace_index=workspace_index,
        artifact_inspector=build_artifact_inspector_payload(
            paths,
            inputs,
            symbols,
            workspace_index,
            source_index,
            source_graph,
        ),
        formatter=formatter,
        formatted_source_text=formatted_text,
        debug=build_debug_payload(
            inputs.summary,
            inputs.object_path_text,
            symbols,
            manifest=inputs.manifest,
            source_graph=source_graph,
            source_path=paths.source.display_path,
        ),
        source_index=source_index,
        source_graph=source_graph,
        symbols=symbols,
    )


def diagnostics_summary(inputs: EditorToolingInputs) -> dict[str, Any]:
    entries = diagnostics_entries(inputs.diagnostics)
    return {
        "status_name": inputs.summary.get("observability", {}).get("status_name", ""),
        "total": len(entries),
        "entries": entries,
    }
