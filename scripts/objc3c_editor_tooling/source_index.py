from __future__ import annotations

import hashlib
import json
import re
from typing import Any


IDENTIFIER_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
IMPORT_RE = re.compile(
    r"^\s*(?:@import|import)\s+([A-Za-z_][A-Za-z0-9_.]*)\s*;",
    re.MULTILINE,
)


def _stable_digest(payload: object) -> str:
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode(
        "utf-8"
    )
    return hashlib.sha256(encoded).hexdigest()


def _source_location(
    source_path: str,
    *,
    line: int,
    column: int,
    end_line: int,
    end_column: int,
) -> dict[str, Any]:
    start_line = max(line - 1, 0)
    start_column = max(column - 1, 0)
    return {
        "uri": source_path,
        "source_path": source_path,
        "compiler_range": {
            "line": line,
            "column": column,
            "end_line": end_line,
            "end_column": end_column,
        },
        "range": {
            "start": {"line": start_line, "character": start_column},
            "end": {
                "line": max(end_line - 1, start_line),
                "character": max(end_column - 1, start_column),
            },
        },
        "range_model": "compiler-1-based-plus-lsp-zero-based",
    }


def _symbol_location(source_path: str, symbol: dict[str, Any]) -> dict[str, Any]:
    line = int(symbol.get("line", 0) or 0)
    column = int(symbol.get("column", 0) or 0)
    name = str(symbol.get("name", "") or "")
    end_line = int(symbol.get("end_line", line) or line)
    end_column = int(symbol.get("end_column", column + len(name)) or column + len(name))
    return _source_location(
        source_path,
        line=line,
        column=column,
        end_line=end_line,
        end_column=end_column,
    )


def _line_start_offsets(source_text: str) -> list[int]:
    offsets = [0]
    for match in re.finditer(r"\n", source_text):
        offsets.append(match.end())
    return offsets


def _line_column_for_offset(offsets: list[int], offset: int) -> tuple[int, int]:
    line_index = 0
    for index, line_start in enumerate(offsets):
        if line_start > offset:
            break
        line_index = index
    return line_index + 1, offset - offsets[line_index] + 1


def _skip_mask(source_text: str) -> list[bool]:
    mask = [False] * len(source_text)
    i = 0
    quote: str | None = None
    escaped = False
    in_line_comment = False
    in_block_comment = False
    while i < len(source_text):
        ch = source_text[i]
        nxt = source_text[i + 1] if i + 1 < len(source_text) else ""
        if ch == "\n":
            in_line_comment = False
            escaped = False if quote is None else escaped
            i += 1
            continue
        if in_line_comment:
            mask[i] = True
            i += 1
            continue
        if in_block_comment:
            mask[i] = True
            if ch == "*" and nxt == "/":
                if i + 1 < len(mask):
                    mask[i + 1] = True
                in_block_comment = False
                i += 2
                continue
            i += 1
            continue
        if quote is not None:
            mask[i] = True
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == quote:
                quote = None
            i += 1
            continue
        if ch == "/" and nxt == "/":
            mask[i] = True
            if i + 1 < len(mask):
                mask[i + 1] = True
            in_line_comment = True
            i += 2
            continue
        if ch == "/" and nxt == "*":
            mask[i] = True
            if i + 1 < len(mask):
                mask[i + 1] = True
            in_block_comment = True
            i += 2
            continue
        if ch in {'"', "'"}:
            mask[i] = True
            quote = ch
            i += 1
            continue
        i += 1
    return mask


def _declaration_records(
    source_path: str,
    module_name: str,
    symbols: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for symbol in symbols:
        name = str(symbol.get("name", "") or "")
        kind = str(symbol.get("kind", "") or "")
        if not name or not kind:
            continue
        location = _symbol_location(source_path, symbol)
        records.append(
            {
                "symbol": name,
                "kind": kind,
                "module": module_name,
                "location": location,
                "definition": {
                    "target_uri": source_path,
                    "target_range": location["range"],
                    "target_compiler_range": location["compiler_range"],
                },
                "hover": {
                    "contents": f"{kind} {name}",
                    "module": module_name,
                    "source": "compile-manifest-declaration-coordinate",
                },
                "origin": "compile-manifest",
            }
        )
    return sorted(
        records,
        key=lambda record: (
            int(record["location"]["compiler_range"]["line"]),
            int(record["location"]["compiler_range"]["column"]),
            str(record["kind"]),
            str(record["symbol"]),
        ),
    )


def _reference_records(
    source_path: str,
    module_name: str,
    source_text: str,
    declarations: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    names = {str(record["symbol"]) for record in declarations if record.get("symbol")}
    if not names:
        return []
    declaration_positions = {
        (
            str(record["symbol"]),
            int(record["location"]["compiler_range"]["line"]),
            int(record["location"]["compiler_range"]["column"]),
        )
        for record in declarations
    }
    offsets = _line_start_offsets(source_text)
    mask = _skip_mask(source_text)
    records: list[dict[str, Any]] = []
    for match in IDENTIFIER_RE.finditer(source_text):
        token = match.group(0)
        if token not in names or any(mask[match.start() : match.end()]):
            continue
        line, column = _line_column_for_offset(offsets, match.start())
        is_definition = (token, line, column) in declaration_positions
        location = _source_location(
            source_path,
            line=line,
            column=column,
            end_line=line,
            end_column=column + len(token),
        )
        records.append(
            {
                "symbol": token,
                "module": module_name,
                "location": location,
                "reference_kind": "definition" if is_definition else "lexical-reference",
                "definition_reference": is_definition,
                "origin": "source-lexical-index",
            }
        )
    return sorted(
        records,
        key=lambda record: (
            str(record["symbol"]),
            int(record["location"]["compiler_range"]["line"]),
            int(record["location"]["compiler_range"]["column"]),
            str(record["reference_kind"]),
        ),
    )


def _import_records(source_path: str, source_text: str) -> list[dict[str, Any]]:
    offsets = _line_start_offsets(source_text)
    records: list[dict[str, Any]] = []
    for match in IMPORT_RE.finditer(source_text):
        module_name = match.group(1)
        line, column = _line_column_for_offset(offsets, match.start(1))
        records.append(
            {
                "module": module_name,
                "location": _source_location(
                    source_path,
                    line=line,
                    column=column,
                    end_line=line,
                    end_column=column + len(module_name),
                ),
                "origin": "source-import-statement",
            }
        )
    return records


def _diagnostic_records(
    source_path: str,
    diagnostics: list[Any],
) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    for entry in diagnostics:
        if not isinstance(entry, dict):
            continue
        line = int(entry.get("line", 0) or 0)
        column = int(entry.get("column", 0) or 0)
        code = str(entry.get("code", "") or "")
        if line <= 0 or column <= 0 or not code:
            continue
        records.append(
            {
                "code": code,
                "severity": str(entry.get("severity", "unknown") or "unknown"),
                "message": str(entry.get("message", "") or ""),
                "location": _source_location(
                    source_path,
                    line=line,
                    column=column,
                    end_line=line,
                    end_column=column + 1,
                ),
                "origin": "diagnostics-json",
            }
        )
    return sorted(
        records,
        key=lambda record: (
            int(record["location"]["compiler_range"]["line"]),
            int(record["location"]["compiler_range"]["column"]),
            str(record["code"]),
        ),
    )


def _artifact_records(artifact_paths: dict[str, Any] | None) -> list[dict[str, str]]:
    if not isinstance(artifact_paths, dict):
        return []
    records: list[dict[str, str]] = []
    for kind, path_text in sorted(artifact_paths.items()):
        path = str(path_text or "")
        if not path:
            continue
        records.append(
            {
                "kind": str(kind),
                "path": path.replace("\\", "/"),
                "origin": "compile-summary-paths",
            }
        )
    return records


def build_source_index(
    *,
    source_path: str,
    module_name: str,
    source_text: str,
    manifest_path: str | None,
    symbols: list[dict[str, Any]],
    diagnostics: list[Any],
    artifact_paths: dict[str, Any] | None = None,
) -> dict[str, Any]:
    declarations = _declaration_records(source_path, module_name, symbols)
    references = _reference_records(source_path, module_name, source_text, declarations)
    imports = _import_records(source_path, source_text)
    diagnostic_anchors = _diagnostic_records(source_path, diagnostics)
    emitted_artifacts = _artifact_records(artifact_paths)
    digest_input = {
        "source_path": source_path,
        "module_name": module_name,
        "manifest_path": manifest_path or "",
        "declarations": declarations,
        "references": references,
        "imports": imports,
        "diagnostics": diagnostic_anchors,
        "emitted_artifacts": emitted_artifacts,
    }
    available = bool(manifest_path and declarations)
    return {
        "contract_id": "objc3c.developer.tooling.source.index.v1",
        "source_path": source_path,
        "module_name": module_name,
        "available": available,
        "source_truth_model": "source-text-plus-compile-manifest-diagnostics-and-summary-paths",
        "manifest_path": manifest_path or "",
        "declaration_count": len(declarations),
        "reference_count": len(references),
        "import_count": len(imports),
        "diagnostic_anchor_count": len(diagnostic_anchors),
        "emitted_artifact_count": len(emitted_artifacts),
        "declarations": declarations,
        "references": references,
        "imports": imports,
        "diagnostic_anchors": diagnostic_anchors,
        "emitted_artifacts": emitted_artifacts,
        "source_index_digest": _stable_digest(digest_input),
        "deterministic_ordering": "source-location-then-kind-then-symbol",
        "retired_route_reason": ""
        if available
        else "compile produced no manifest-backed declaration surface",
    }

