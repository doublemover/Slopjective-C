from __future__ import annotations

import hashlib
import re
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT, display_path, resolve_repo_path

from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.paths import EditorToolingPaths


ARTIFACT_EXPLANATIONS = {
    "summary": "compile summary with stage status and emitted artifact paths",
    "diagnostics": "structured diagnostics for editor squiggles and fix-it routing",
    "manifest": "manifest-backed declarations and source graph inputs for navigation",
    "ir": "lowered LLVM IR text emitted by the canonical frontend path",
    "object": "native object artifact exposed through runtime inspector commands",
    "runtime_metadata_binary": "runtime metadata import payload emitted by lowering",
}

TEXT_ARTIFACT_KINDS = {"summary", "diagnostics", "manifest", "ir"}
SOURCE_GRAPH_KEYS = (
    "source_graph",
    "sourceGraph",
    "source_graph_readiness",
    "frontend_source_graph",
)


def _summary_paths(summary: dict[str, Any]) -> dict[str, Any]:
    paths = summary.get("paths", {})
    return paths if isinstance(paths, dict) else {}


def _runtime_inspector(summary: dict[str, Any]) -> dict[str, Any]:
    runtime_inspector = summary.get("runtime_inspector", {})
    return runtime_inspector if isinstance(runtime_inspector, dict) else {}


def _dump_commands(summary: dict[str, Any]) -> dict[str, Any]:
    observability = summary.get("observability", {})
    if not isinstance(observability, dict):
        return {}
    commands = observability.get("dump_commands", {})
    return commands if isinstance(commands, dict) else {}


def _runtime_dump_commands(summary: dict[str, Any]) -> dict[str, Any]:
    commands = _runtime_inspector(summary).get("dump_commands", {})
    return commands if isinstance(commands, dict) else {}


def _stable_digest_for_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _artifact_record(kind: str, path_text: str | None) -> dict[str, Any]:
    raw_path = str(path_text or "")
    if not raw_path:
        return {
            "kind": kind,
            "path": "",
            "available": False,
            "size_bytes": 0,
            "sha256": "",
            "explanation": ARTIFACT_EXPLANATIONS[kind],
            "retired_route_reason": f"compile summary did not publish a {kind} artifact path",
        }

    resolved = resolve_repo_path(raw_path)
    available = resolved.is_file()
    return {
        "kind": kind,
        "path": display_path(resolved) if available else raw_path.replace("\\", "/"),
        "available": available,
        "size_bytes": resolved.stat().st_size if available else 0,
        "sha256": _stable_digest_for_file(resolved) if available else "",
        "explanation": ARTIFACT_EXPLANATIONS[kind],
        "retired_route_reason": ""
        if available
        else f"published {kind} artifact path is not present on disk",
    }


def _read_text(record: dict[str, Any]) -> str:
    if record.get("available") is not True:
        return ""
    return resolve_repo_path(str(record["path"])).read_text(encoding="utf-8", errors="replace")


def _severity_counts(entries: list[Any]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for entry in entries:
        if not isinstance(entry, dict):
            continue
        severity = str(entry.get("severity", "unknown") or "unknown")
        counts[severity] = counts.get(severity, 0) + 1
    return dict(sorted(counts.items()))


def _diagnostic_codes(entries: list[Any]) -> list[str]:
    codes = {
        str(entry.get("code", ""))
        for entry in entries
        if isinstance(entry, dict) and entry.get("code")
    }
    return sorted(codes)


def _diagnostics_payload(
    record: dict[str, Any],
    diagnostics: dict[str, Any],
) -> dict[str, Any]:
    entries = diagnostics.get("diagnostics", [])
    entries = entries if isinstance(entries, list) else []
    return {
        "available": record["available"],
        "path": record["path"],
        "diagnostic_count": len(entries),
        "severity_counts": _severity_counts(entries),
        "diagnostic_codes": _diagnostic_codes(entries),
        "retired_route_reason": record["retired_route_reason"],
    }


def _symbol_kind_counts(symbols: list[dict[str, Any]]) -> dict[str, int]:
    counts: dict[str, int] = {}
    for symbol in symbols:
        kind = str(symbol.get("kind", "unknown") or "unknown")
        counts[kind] = counts.get(kind, 0) + 1
    return dict(sorted(counts.items()))


def _find_source_graph_fields(manifest: dict[str, Any]) -> dict[str, Any]:
    found: dict[str, Any] = {}
    for key in SOURCE_GRAPH_KEYS:
        if key in manifest:
            found[key] = manifest[key]
    frontend = manifest.get("frontend", {})
    pipeline = frontend.get("pipeline", {}) if isinstance(frontend, dict) else {}
    if isinstance(pipeline, dict):
        for key in SOURCE_GRAPH_KEYS:
            if key in pipeline:
                found[f"frontend.pipeline.{key}"] = pipeline[key]
    return found


def _manifest_payload(
    record: dict[str, Any],
    manifest: dict[str, Any],
    symbols: list[dict[str, Any]],
) -> dict[str, Any]:
    source_graph_fields = _find_source_graph_fields(manifest)
    module_name = str(manifest.get("module", "") or "")
    return {
        "available": record["available"],
        "path": record["path"],
        "module": module_name,
        "declaration_count": len(symbols),
        "symbol_kind_counts": _symbol_kind_counts(symbols),
        "source_graph_field_count": len(source_graph_fields),
        "source_graph_fields": sorted(source_graph_fields),
        "retired_route_reason": record["retired_route_reason"],
    }


def _ir_payload(record: dict[str, Any]) -> dict[str, Any]:
    text = _read_text(record)
    lines = text.splitlines()
    return {
        "available": record["available"],
        "path": record["path"],
        "line_count": len(lines),
        "function_definition_count": sum(1 for line in lines if re.match(r"^\s*define\b", line)),
        "external_declaration_count": sum(1 for line in lines if re.match(r"^\s*declare\b", line)),
        "global_record_count": sum(1 for line in lines if re.match(r"^\s*@", line)),
        "retired_route_reason": record["retired_route_reason"],
    }


def _source_graph_payload(
    manifest_payload: dict[str, Any],
    symbols: list[dict[str, Any]],
    workspace_index: dict[str, Any],
    source_index: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
) -> dict[str, Any]:
    if isinstance(source_graph, dict) and source_graph.get("contract_id") == "objc3c.developer.tooling.source.graph.v1":
        return {
            "available": source_graph.get("available") is True,
            "graph_inputs": list(source_graph.get("evidence", {}).get("source_truth_inputs", []))
            if isinstance(source_graph.get("evidence"), dict)
            else [],
            "declaration_node_count": int(source_graph.get("declaration_node_count", 0) or 0),
            "workspace_package_count": int(source_graph.get("package_provenance", {}).get("package_count", 0) or 0)
            if isinstance(source_graph.get("package_provenance"), dict)
            else int(workspace_index.get("package_count", 0) or 0),
            "workspace_index_digest": str(workspace_index.get("workspace_index_digest", "") or ""),
            "source_declaration_count": int(source_index.get("declaration_count", 0) or 0)
            if isinstance(source_index, dict)
            else 0,
            "source_reference_count": int(source_index.get("reference_count", 0) or 0)
            if isinstance(source_index, dict)
            else 0,
            "source_index_digest": str(source_index.get("source_index_digest", "") or "")
            if isinstance(source_index, dict)
            else "",
            "source_graph_digest": str(source_graph.get("source_graph_digest", "") or ""),
            "retired_route_reason": str(source_graph.get("retired_route_reason", "") or ""),
        }
    workspace_available = workspace_index.get("available") is True
    source_index = source_index or {}
    source_index_available = source_index.get("available") is True
    available = manifest_payload["available"] and (
        bool(symbols)
        or int(manifest_payload.get("source_graph_field_count", 0) or 0) > 0
        or workspace_available
        or source_index_available
    )
    graph_inputs = ["manifest-declarations"] if symbols else []
    if int(manifest_payload.get("source_graph_field_count", 0) or 0) > 0:
        graph_inputs.append("manifest-source-graph-fields")
    if workspace_available:
        graph_inputs.append("workspace-index-packages")
    if source_index_available:
        graph_inputs.append("source-derived-editor-index")
    digest_input = {
        "symbols": [
            {
                "name": symbol.get("name"),
                "kind": symbol.get("kind"),
                "line": symbol.get("line"),
                "column": symbol.get("column"),
            }
            for symbol in symbols
        ],
        "workspace_index_digest": workspace_index.get("workspace_index_digest", ""),
        "source_index_digest": source_index.get("source_index_digest", ""),
        "source_graph_fields": manifest_payload.get("source_graph_fields", []),
    }
    digest = hashlib.sha256(
        repr(digest_input).encode("utf-8")
    ).hexdigest() if available else ""
    return {
        "available": available,
        "graph_inputs": graph_inputs,
        "declaration_node_count": len(symbols),
        "workspace_package_count": int(workspace_index.get("package_count", 0) or 0),
        "workspace_index_digest": str(workspace_index.get("workspace_index_digest", "") or ""),
        "source_declaration_count": int(source_index.get("declaration_count", 0) or 0),
        "source_reference_count": int(source_index.get("reference_count", 0) or 0),
        "source_index_digest": str(source_index.get("source_index_digest", "") or ""),
        "source_graph_digest": digest,
        "retired_route_reason": ""
        if available
        else "manifest did not publish declarations, source graph fields, or workspace index inputs",
    }


def _object_payload(
    record: dict[str, Any],
    runtime_commands: dict[str, Any],
) -> dict[str, Any]:
    object_symbols = str(runtime_commands.get("object_symbols", "") or "")
    object_sections = str(runtime_commands.get("object_sections", "") or "")
    return {
        "available": record["available"],
        "path": record["path"],
        "size_bytes": record["size_bytes"],
        "sha256": record["sha256"],
        "object_symbol_inventory_command": object_symbols,
        "object_section_inventory_command": object_sections,
        "inspection_ready": bool(record["available"] and object_symbols and object_sections),
        "retired_route_reason": record["retired_route_reason"]
        if not record["available"]
        else "",
    }


def _runtime_imports_payload(
    record: dict[str, Any],
    summary: dict[str, Any],
) -> dict[str, Any]:
    runtime_inspector = _runtime_inspector(summary)
    runtime_commands = _runtime_dump_commands(summary)
    runtime_imports = summary.get("runtime_imports", [])
    runtime_imports = runtime_imports if isinstance(runtime_imports, list) else []
    available = record["available"] or bool(runtime_imports) or runtime_inspector.get("available") is True
    return {
        "available": available,
        "runtime_metadata_binary_path": record["path"],
        "runtime_metadata_binary_present": record["available"],
        "runtime_import_count": len(runtime_imports),
        "runtime_imports": runtime_imports,
        "runtime_inspector_available": runtime_inspector.get("available") is True,
        "runtime_inspector_contract_id": str(runtime_inspector.get("contract_id", "") or ""),
        "runtime_dump_commands": {
            key: str(value)
            for key, value in sorted(runtime_commands.items())
            if value
        },
        "retired_route_reason": ""
        if available
        else "compile emitted no runtime metadata import artifact or runtime inspector surface",
    }


def _inspection_commands(
    records: dict[str, dict[str, Any]],
    summary: dict[str, Any],
) -> dict[str, str]:
    commands: dict[str, str] = {}
    summary_commands = _dump_commands(summary)
    runtime_commands = _runtime_dump_commands(summary)
    for kind, record in records.items():
        if record.get("available") is not True:
            continue
        if kind in summary_commands and summary_commands[kind]:
            commands[kind] = str(summary_commands[kind])
        elif kind in TEXT_ARTIFACT_KINDS:
            commands[kind] = f"Get-Content -Raw '{record['path']}'"
    if records["object"].get("available") is True:
        for command_name in ("object_symbols", "object_sections"):
            command = runtime_commands.get(command_name)
            if command:
                commands[command_name] = str(command)
    return dict(sorted(commands.items()))


def build_artifact_inspector_payload(
    paths: EditorToolingPaths,
    inputs: EditorToolingInputs,
    symbols: list[dict[str, Any]],
    workspace_index: dict[str, Any],
    source_index: dict[str, Any] | None = None,
    source_graph: dict[str, Any] | None = None,
) -> dict[str, Any]:
    summary_paths = _summary_paths(inputs.summary)
    records = {
        "summary": _artifact_record("summary", display_path(paths.compile_summary)),
        "diagnostics": _artifact_record("diagnostics", inputs.diagnostics_path_text),
        "manifest": _artifact_record("manifest", inputs.manifest_path_text),
        "ir": _artifact_record("ir", str(summary_paths.get("ir", "") or "")),
        "object": _artifact_record("object", inputs.object_path_text),
        "runtime_metadata_binary": _artifact_record(
            "runtime_metadata_binary",
            str(summary_paths.get("runtime_metadata_binary", "") or ""),
        ),
    }
    available_kinds = [kind for kind, record in records.items() if record["available"]]
    unsupported_kinds = [kind for kind, record in records.items() if not record["available"]]
    manifest = _manifest_payload(records["manifest"], inputs.manifest, symbols)
    runtime_commands = _runtime_dump_commands(inputs.summary)
    supported = bool(available_kinds)
    return {
        "contract_id": "objc3c.developer.tooling.artifact.inspector.v1",
        "source_path": paths.source.display_path,
        "supported": supported,
        "support_class": "compile-artifact-inspector" if supported else "fail-closed",
        "artifact_records": [records[kind] for kind in sorted(records)],
        "inspected_artifact_kinds": sorted(available_kinds),
        "unsupported_artifact_kinds": sorted(unsupported_kinds),
        "diagnostics": _diagnostics_payload(records["diagnostics"], inputs.diagnostics),
        "manifest": manifest,
        "ir": _ir_payload(records["ir"]),
        "object": _object_payload(records["object"], runtime_commands),
        "runtime_imports": _runtime_imports_payload(
            records["runtime_metadata_binary"],
            inputs.summary,
        ),
        "source_graph": _source_graph_payload(
            manifest,
            symbols,
            workspace_index,
            source_index,
            source_graph,
        ),
        "source_index": source_index or {},
        "inspection_commands": _inspection_commands(records, inputs.summary),
        "retired_route_reason": ""
        if supported
        else "compile produced no inspectable editor-facing artifacts",
    }
