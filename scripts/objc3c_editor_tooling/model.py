from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from format_objc3c_source import build_format_summary_for_source
from objc3c_tooling.paths import display_path

from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.paths import EditorToolingPaths
from objc3c_editor_tooling.validation import diagnostics_entries


@dataclass(frozen=True)
class EditorToolingModel:
    language_server: dict[str, Any]
    navigation: dict[str, Any]
    formatter: dict[str, Any]
    formatted_source_text: str
    debug: dict[str, Any]
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
) -> dict[str, Any]:
    manifest_available = bool(manifest_path_text)
    supported_capabilities = [
        "publishDiagnostics",
        "documentSymbol" if manifest_available else None,
        "workspaceSymbol" if manifest_available else None,
        "definition" if manifest_available and symbols else None,
    ]
    supported_capabilities = [capability for capability in supported_capabilities if capability is not None]
    unpublished_capabilities = [
        "references",
        "rename",
        "semanticTokens",
        "codeAction",
        "statementLevelStepping",
    ]
    capability_statuses = {
        "publishDiagnostics": {
            "supported": True,
            "support_class": "authoritative",
            "evidence": "diagnostics-json",
        },
        "documentSymbol": {
            "supported": manifest_available,
            "support_class": "manifest-backed" if manifest_available else "fail-closed",
            "unpublished_reason": "" if manifest_available else "disabled until compile emits manifest declarations",
        },
        "workspaceSymbol": {
            "supported": manifest_available,
            "support_class": "manifest-backed" if manifest_available else "fail-closed",
            "unpublished_reason": "" if manifest_available else "disabled until compile emits manifest declarations",
        },
        "definition": {
            "supported": manifest_available and bool(symbols),
            "support_class": "manifest-backed" if manifest_available and symbols else "fail-closed",
            "unpublished_reason": "" if manifest_available and symbols else "disabled until compile emits declaration coordinates",
        },
        "references": {
            "supported": False,
            "support_class": "unpublished",
            "unpublished_reason": "not published; use documentSymbol/workspaceSymbol and definition on compile-owned declarations",
        },
        "rename": {
            "supported": False,
            "support_class": "unpublished",
            "unpublished_reason": "not published; canonical compile graph has no rename contract yet",
        },
        "semanticTokens": {
            "supported": False,
            "support_class": "unpublished",
            "unpublished_reason": "not published; no semantic token contract is emitted on the canonical toolchain path",
        },
        "codeAction": {
            "supported": False,
            "support_class": "unpublished",
            "unpublished_reason": "not published; diagnostics remain actionable only through compile output and operator guidance",
        },
        "statementLevelStepping": {
            "supported": False,
            "support_class": "unpublished",
            "unpublished_reason": "not published; statement stepping remains fail-closed pending line-table evidence",
        },
    }
    return {
        "contract_id": "objc3c.developer.tooling.language.server.capability.surface.v1",
        "summary_status_name": summary.get("observability", {}).get("status_name", ""),
        "manifest_backed_navigation": manifest_available,
        "supported_capability_ids": supported_capabilities,
        "unpublished_capability_ids": unpublished_capabilities,
        "capability_statuses": capability_statuses,
    }


def build_navigation_payload(source_display: str, manifest_path_text: str | None, symbols: list[dict[str, Any]]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.navigation.index.v1",
        "source_path": source_display,
        "available": bool(manifest_path_text),
        "manifest_path": manifest_path_text,
        "symbol_count": len(symbols),
        "supported_symbol_kinds": sorted({symbol["kind"] for symbol in symbols}),
        "symbols": symbols,
        "retired_route_reason": "" if manifest_path_text else "compile produced no manifest-backed declaration surface",
    }


def build_debug_payload(
    summary: dict[str, Any],
    object_path_text: str | None,
    symbols: list[dict[str, Any]],
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
        "runtime_inspector_contract_id": runtime_inspector.get("contract_id", "") if isinstance(runtime_inspector, dict) else "",
        "artifact_inspection_ready": bool(object_path_text and object_symbols),
        "retired_route_reason": "" if supported else "compile produced no object artifact or declaration coordinates for preview debug anchors",
    }


def build_editor_tooling_model(paths: EditorToolingPaths, inputs: EditorToolingInputs) -> EditorToolingModel:
    symbols = extract_symbols(inputs.manifest)
    formatted_text, formatter = build_format_summary_for_source(
        paths.source.display_path,
        inputs.source_text,
        display_path(paths.formatted_source),
    )
    return EditorToolingModel(
        language_server=build_language_server_payload(inputs.summary, inputs.manifest_path_text, symbols),
        navigation=build_navigation_payload(paths.source.display_path, inputs.manifest_path_text, symbols),
        formatter=formatter,
        formatted_source_text=formatted_text,
        debug=build_debug_payload(inputs.summary, inputs.object_path_text, symbols),
        symbols=symbols,
    )


def diagnostics_summary(inputs: EditorToolingInputs) -> dict[str, Any]:
    entries = diagnostics_entries(inputs.diagnostics)
    return {
        "status_name": inputs.summary.get("observability", {}).get("status_name", ""),
        "total": len(entries),
        "entries": entries,
    }
