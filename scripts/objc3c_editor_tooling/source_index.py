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


SOURCE_GRAPH_FIELD_NAMES = (
    "source_graph",
    "sourceGraph",
    "source_graph_readiness",
    "frontend_source_graph",
    "objc_executable_metadata_source_graph",
)


SEMANTIC_REFERENCE_KEYS = (
    "references",
    "semantic_references",
    "reference_edges",
    "declaration_reference_edges",
)


def _stable_id(prefix: str, payload: object) -> str:
    return f"{prefix}:{_stable_digest(payload)[:24]}"


def _definition_target(location: dict[str, Any]) -> dict[str, Any]:
    return {
        "target_uri": str(location["uri"]),
        "target_range": location["range"],
        "target_compiler_range": location["compiler_range"],
    }


def _range_sort_key(location: dict[str, Any]) -> tuple[int, int, int, int]:
    compiler_range = location.get("compiler_range", {})
    return (
        int(compiler_range.get("line", 0) or 0),
        int(compiler_range.get("column", 0) or 0),
        int(compiler_range.get("end_line", 0) or 0),
        int(compiler_range.get("end_column", 0) or 0),
    )


def _graph_location_for_package(package: dict[str, Any]) -> dict[str, Any]:
    definition = package.get("definition")
    if isinstance(definition, dict) and "compiler_range" in definition:
        return definition
    source_path = str(package.get("source_path", "") or "package")
    module_name = str(package.get("module_name", "") or package.get("package_id", "") or "package")
    return _source_location(
        source_path,
        line=1,
        column=1,
        end_line=1,
        end_column=max(len(module_name), 1) + 1,
    )


def _graph_node(
    *,
    symbol_kind: str,
    display_name: str,
    canonical_name: str,
    owning_module: str,
    owning_package: str,
    location: dict[str, Any],
    visibility: str,
    source_truth_kind: str,
    evidence_ids: list[str],
    evidence_boundary: str,
    generated: bool = False,
    source_owned: bool = True,
    semantic_type_summary: str = "",
    package_readonly: bool = False,
) -> dict[str, Any]:
    node_id = _stable_id(
        "sgn",
        {
            "kind": symbol_kind,
            "canonical": canonical_name,
            "module": owning_module,
            "package": owning_package,
            "range": location.get("compiler_range", {}),
        },
    )
    rename_blockers: list[str] = []
    if generated:
        rename_blockers.append("generated-symbol")
    if package_readonly:
        rename_blockers.append("readonly-package-boundary")
    if visibility == "public":
        rename_blockers.append("public-abi-rename-not-enabled")
    if symbol_kind in {"module", "package", "stdlib-symbol", "package-symbol"}:
        rename_blockers.append("symbol-kind-not-editor-renamable")
    rename_blockers.append("semantic-reference-closure-missing")
    rename_blockers = sorted(set(rename_blockers))
    return {
        "node_id": node_id,
        "stable_id_version": "source-graph-node-v1",
        "symbol_kind": symbol_kind,
        "display_name": display_name,
        "canonical_name": canonical_name,
        "owning_module": owning_module,
        "owning_package": owning_package,
        "location": location,
        "definition_target": _definition_target(location),
        "semantic_type_summary": semantic_type_summary,
        "visibility": visibility,
        "generated": generated,
        "source_owned": source_owned,
        "package_readonly": package_readonly,
        "rename_eligible": False,
        "rename_blockers": rename_blockers,
        "evidence_ids": evidence_ids,
        "evidence_boundary": evidence_boundary,
        "source_truth_kind": source_truth_kind,
    }


def _graph_edge(
    *,
    edge_kind: str,
    source_node: dict[str, Any],
    target_node: dict[str, Any],
    source_range: dict[str, Any] | None = None,
    target_range: dict[str, Any] | None = None,
    access: str = "none",
    mutation: str = "none",
    rename_safe: bool = False,
    package_boundary: str = "same-package",
    generated: bool = False,
    source_owned: bool = True,
    semantic_proof: bool = True,
    evidence_ids: list[str] | None = None,
    evidence_boundary: str = "",
) -> dict[str, Any]:
    source_location = source_range or source_node["location"]
    target_location = target_range or target_node["location"]
    edge_id = _stable_id(
        "sge",
        {
            "kind": edge_kind,
            "source": source_node["node_id"],
            "target": target_node["node_id"],
            "range": source_location.get("compiler_range", {}),
            "semantic_proof": semantic_proof,
        },
    )
    return {
        "edge_id": edge_id,
        "edge_kind": edge_kind,
        "source_node_id": source_node["node_id"],
        "target_node_id": target_node["node_id"],
        "source_range": source_location,
        "target_range": target_location,
        "access": access,
        "mutation": mutation,
        "rename_safe": rename_safe,
        "package_boundary": package_boundary,
        "generated": generated,
        "source_owned": source_owned,
        "semantic_proof": semantic_proof,
        "evidence_ids": evidence_ids or [],
        "evidence_boundary": evidence_boundary,
    }


def _walk_source_graph_fields(payload: Any, path: str = "") -> list[tuple[str, Any]]:
    found: list[tuple[str, Any]] = []
    if isinstance(payload, dict):
        for key, value in payload.items():
            child_path = f"{path}.{key}" if path else str(key)
            if str(key) in SOURCE_GRAPH_FIELD_NAMES or "source_graph" in str(key):
                found.append((child_path, value))
            if isinstance(value, (dict, list)):
                found.extend(_walk_source_graph_fields(value, child_path))
    elif isinstance(payload, list):
        for index, value in enumerate(payload):
            if isinstance(value, (dict, list)):
                found.extend(_walk_source_graph_fields(value, f"{path}[{index}]"))
    return found


def _source_graph_field_summaries(manifest: dict[str, Any]) -> tuple[list[dict[str, Any]], dict[str, Any]]:
    raw_fields: dict[str, Any] = {}
    for path, payload in _walk_source_graph_fields(manifest):
        raw_fields[path] = payload
    summaries: list[dict[str, Any]] = []
    for path, payload in sorted(raw_fields.items()):
        if isinstance(payload, dict):
            node_hint = len(payload.get("nodes", [])) if isinstance(payload.get("nodes"), list) else 0
            edge_hint = len(payload.get("edges", [])) if isinstance(payload.get("edges"), list) else 0
            payload_type = "object"
            semantic_reference_keys = [
                key for key in SEMANTIC_REFERENCE_KEYS if isinstance(payload.get(key), list)
            ]
        elif isinstance(payload, list):
            node_hint = 0
            edge_hint = len(payload)
            payload_type = "array"
            semantic_reference_keys = []
        else:
            text = str(payload)
            node_hint = text.count("node=") + text.count("interface=") + text.count("implementation=")
            edge_hint = text.count("edge=")
            payload_type = type(payload).__name__
            semantic_reference_keys = []
        summaries.append(
            {
                "path": path,
                "payload_type": payload_type,
                "payload_digest": _stable_digest(payload),
                "node_count_hint": node_hint,
                "edge_count_hint": edge_hint,
                "semantic_reference_keys": semantic_reference_keys,
            }
        )
    return summaries, raw_fields


def _has_semantic_reference_closure(raw_fields: dict[str, Any]) -> bool:
    for payload in raw_fields.values():
        if not isinstance(payload, dict):
            continue
        if any(isinstance(payload.get(key), list) for key in SEMANTIC_REFERENCE_KEYS):
            return True
        capabilities = payload.get("capabilities")
        if isinstance(capabilities, dict) and capabilities.get("semantic_references") is True:
            return True
    return False


def _semantic_token_type(symbol_kind: str) -> str:
    token_types = {
        "module": "namespace",
        "package": "namespace",
        "function": "function",
        "global": "variable",
        "parameter": "parameter",
        "local": "variable",
        "interface": "class",
        "implementation": "class",
        "category": "class",
        "protocol": "interface",
        "method": "method",
        "class-method": "method",
        "property": "property",
        "ivar": "property",
        "selector": "enumMember",
        "generated-accessor": "method",
        "stdlib-symbol": "variable",
        "package-symbol": "namespace",
    }
    return token_types.get(symbol_kind, "variable")


def _semantic_token_for_node(node: dict[str, Any], *, semantic_proof: bool) -> dict[str, Any]:
    modifiers = ["declaration"] if semantic_proof else ["unverified-reference"]
    if node.get("generated") is True:
        modifiers.append("generated")
    visibility = str(node.get("visibility", "") or "")
    if visibility:
        modifiers.append(visibility)
    return {
        "node_id": node["node_id"],
        "token_type": _semantic_token_type(str(node["symbol_kind"])),
        "token_modifiers": sorted(set(modifiers)),
        "range": node["location"]["range"],
        "compiler_range": node["location"]["compiler_range"],
        "semantic_proof": semantic_proof,
        "evidence_ids": list(node.get("evidence_ids", [])),
    }


def _diagnostic(code: str, message: str, *, severity: str = "info", node_id: str = "") -> dict[str, Any]:
    return {
        "code": code,
        "severity": severity,
        "message": message,
        "node_id": node_id,
        "source": "objc3c-source-graph",
    }


def _compiler_graph_edges_from_manifest(
    raw_fields: dict[str, Any],
    nodes_by_display: dict[str, list[dict[str, Any]]],
) -> list[dict[str, Any]]:
    edges: list[dict[str, Any]] = []
    for path, payload in sorted(raw_fields.items()):
        if not isinstance(payload, dict) or not isinstance(payload.get("edges"), list):
            continue
        for raw_edge in payload["edges"]:
            if not isinstance(raw_edge, str) or "->" not in raw_edge:
                continue
            source_name, target_name = [part.strip() for part in raw_edge.split("->", 1)]
            source_matches = nodes_by_display.get(source_name, [])
            target_matches = nodes_by_display.get(target_name, [])
            if len(source_matches) != 1 or len(target_matches) != 1:
                continue
            edges.append(
                _graph_edge(
                    edge_kind="compiler-source-graph-edge",
                    source_node=source_matches[0],
                    target_node=target_matches[0],
                    semantic_proof=True,
                    evidence_ids=[path],
                    evidence_boundary=(
                        "compiler-emitted source_graph edge imported as structural graph evidence; "
                        "not sufficient by itself for editor reference or rename publication"
                    ),
                )
            )
    return edges


def _reference_candidates(
    source_index: dict[str, Any],
    nodes_by_display: dict[str, list[dict[str, Any]]],
) -> tuple[list[dict[str, Any]], list[dict[str, Any]]]:
    candidates: list[dict[str, Any]] = []
    edges: list[dict[str, Any]] = []
    for reference in source_index.get("references", []):
        if not isinstance(reference, dict) or reference.get("definition_reference") is True:
            continue
        symbol = str(reference.get("symbol", "") or "")
        location = reference.get("location")
        if not symbol or not isinstance(location, dict):
            continue
        matches = nodes_by_display.get(symbol, [])
        target_node = matches[0] if len(matches) == 1 else None
        candidate = {
            "symbol": symbol,
            "location": location,
            "target_node_id": target_node["node_id"] if target_node else "",
            "candidate_status": "single-target-unverified"
            if target_node
            else ("ambiguous-unverified" if matches else "unresolved-unverified"),
            "semantic_proof": False,
            "origin": str(reference.get("origin", "source-lexical-index") or "source-lexical-index"),
            "fail_closed_reason": (
                "lexical source index candidate is not an authoritative semantic reference"
            ),
        }
        candidates.append(candidate)
        if target_node is not None:
            pseudo_source = {
                **target_node,
                "location": location,
            }
            edges.append(
                _graph_edge(
                    edge_kind="declaration-to-reference-candidate",
                    source_node=target_node,
                    target_node=pseudo_source,
                    source_range=target_node["location"],
                    target_range=location,
                    access="unknown",
                    mutation="unknown",
                    rename_safe=False,
                    semantic_proof=False,
                    evidence_ids=["source-lexical-index"],
                    evidence_boundary=(
                        "lexical reference candidate only; native semantic reference closure is required "
                        "before editor references or rename can use this edge"
                    ),
                )
            )
    return sorted(
        candidates,
        key=lambda candidate: (str(candidate["symbol"]), _range_sort_key(candidate["location"])),
    ), edges


def _source_graph_diagnostics(
    *,
    nodes: list[dict[str, Any]],
    reference_candidates: list[dict[str, Any]],
    semantic_reference_closure: bool,
) -> list[dict[str, Any]]:
    diagnostics: list[dict[str, Any]] = []
    if not semantic_reference_closure:
        diagnostics.append(
            _diagnostic(
                "O3SG_REFERENCE_SEMANTIC_PROOF_MISSING",
                "native compiler semantic reference closure is not present; references, rename, and semantic tokens fail closed",
                severity="warning",
            )
        )
        diagnostics.append(
            _diagnostic(
                "O3SG_RENAME_SEMANTIC_PROOF_MISSING",
                "safe rename requires compiler-owned declaration-to-reference closure and collision scopes",
                severity="warning",
            )
        )
    if reference_candidates:
        diagnostics.append(
            _diagnostic(
                "O3SG_REFERENCE_LEXICAL_CANDIDATE_UNPROVEN",
                "source-index lexical matches are exported only as non-authoritative candidates",
                severity="info",
            )
        )
    readonly_nodes = [node for node in nodes if node.get("package_readonly") is True]
    if readonly_nodes:
        diagnostics.append(
            _diagnostic(
                "O3SG_RENAME_READONLY_PACKAGE",
                "rename edits crossing readonly stdlib/showcase package boundaries fail closed",
                severity="info",
                node_id=str(readonly_nodes[0].get("node_id", "")),
            )
        )
    public_nodes = [node for node in nodes if node.get("visibility") == "public"]
    if public_nodes:
        diagnostics.append(
            _diagnostic(
                "O3SG_RENAME_PUBLIC_ABI_DISALLOWED",
                "public ABI symbols are not editor-renamable without an ABI governance approval payload",
                severity="info",
                node_id=str(public_nodes[0].get("node_id", "")),
            )
        )
    names: dict[str, int] = {}
    for node in nodes:
        key = str(node.get("display_name", "") or "")
        if key:
            names[key] = names.get(key, 0) + 1
    if any(count > 1 for count in names.values()):
        diagnostics.append(
            _diagnostic(
                "O3SG_RENAME_MULTIPLE_SEMANTIC_NODES",
                "cursor rename must resolve to exactly one semantic node before edits are planned",
                severity="info",
            )
        )
    return diagnostics


def build_source_graph(
    *,
    source_path: str,
    module_name: str,
    manifest_path: str | None,
    manifest: dict[str, Any],
    symbols: list[dict[str, Any]],
    source_index: dict[str, Any],
    workspace_index: dict[str, Any],
) -> dict[str, Any]:
    local_package_id = f"source:{source_path}"
    compiler_field_summaries, raw_compiler_fields = _source_graph_field_summaries(manifest)
    semantic_reference_closure = _has_semantic_reference_closure(raw_compiler_fields)
    nodes: list[dict[str, Any]] = []

    module_location = _source_location(
        source_path,
        line=1,
        column=1,
        end_line=1,
        end_column=max(len(module_name), 1) + 1,
    )
    module_node = _graph_node(
        symbol_kind="module",
        display_name=module_name,
        canonical_name=f"module:{module_name}",
        owning_module=module_name,
        owning_package=local_package_id,
        location=module_location,
        visibility="internal",
        source_truth_kind="tooling-module-root",
        evidence_ids=["source-path", "compile-manifest-module"],
        evidence_boundary="module root synthesized from compile manifest/source path for graph anchoring",
    )
    nodes.append(module_node)

    declaration_nodes: list[dict[str, Any]] = []
    for declaration in source_index.get("declarations", []):
        if not isinstance(declaration, dict):
            continue
        symbol = str(declaration.get("symbol", "") or "")
        kind = str(declaration.get("kind", "") or "")
        location = declaration.get("location")
        if not symbol or not kind or not isinstance(location, dict):
            continue
        declaration_node = _graph_node(
            symbol_kind=kind,
            display_name=symbol,
            canonical_name=f"{module_name}::{kind}:{symbol}@{location['compiler_range']['line']}:{location['compiler_range']['column']}",
            owning_module=module_name,
            owning_package=local_package_id,
            location=location,
            visibility="public" if kind in {"global", "function", "interface", "protocol", "category"} else "internal",
            source_truth_kind="compile-manifest-declaration-coordinate",
            semantic_type_summary=str(declaration.get("hover", {}).get("contents", "") or ""),
            evidence_ids=["compile-manifest-declaration-coordinates"],
            evidence_boundary="compiler manifest declaration coordinates are semantic declarations, not reference closure",
        )
        declaration_nodes.append(declaration_node)
        nodes.append(declaration_node)

    package_nodes: list[dict[str, Any]] = []
    for package in workspace_index.get("packages", []):
        if not isinstance(package, dict):
            continue
        package_id = str(package.get("package_id", "") or "")
        if not package_id:
            continue
        package_kind = str(package.get("package_kind", "") or "")
        package_node = _graph_node(
            symbol_kind="package",
            display_name=str(package.get("module_name", "") or package_id),
            canonical_name=f"package:{package_id}",
            owning_module=str(package.get("module_name", "") or module_name),
            owning_package=package_id,
            location=_graph_location_for_package(package),
            visibility="internal" if package_kind == "primary-source" else "public",
            source_truth_kind=str(package.get("source_authority", "") or "workspace-index"),
            evidence_ids=["workspace-semantic-index-guardrails"],
            evidence_boundary="workspace package provenance comes from checked workspace/package surfaces",
            source_owned=package_kind == "primary-source",
            package_readonly=package_kind != "primary-source",
        )
        package_nodes.append(package_node)
        nodes.append(package_node)

    nodes_by_display: dict[str, list[dict[str, Any]]] = {}
    for node in nodes:
        nodes_by_display.setdefault(str(node["display_name"]), []).append(node)

    edges: list[dict[str, Any]] = []
    for declaration_node in declaration_nodes:
        edges.append(
            _graph_edge(
                edge_kind="module-declaration",
                source_node=module_node,
                target_node=declaration_node,
                semantic_proof=True,
                evidence_ids=["compile-manifest-declaration-coordinates"],
                evidence_boundary="compile manifest owns declaration membership in the current module",
            )
        )
        edges.append(
            _graph_edge(
                edge_kind="declaration-to-reference",
                source_node=declaration_node,
                target_node=declaration_node,
                access="definition",
                mutation="none",
                rename_safe=False,
                semantic_proof=True,
                evidence_ids=["compile-manifest-declaration-coordinates"],
                evidence_boundary="definition self-reference only; not a complete reference set",
            )
        )

    for edge in workspace_index.get("cross_package_edges", []):
        if not isinstance(edge, dict):
            continue
        from_package = str(edge.get("from_package_id", "") or "")
        to_package = str(edge.get("to_package_id", "") or "")
        source_matches = [node for node in package_nodes if node["owning_package"] == from_package]
        target_matches = [node for node in package_nodes if node["owning_package"] == to_package]
        if len(source_matches) == 1 and len(target_matches) == 1:
            edges.append(
                _graph_edge(
                    edge_kind="package-dependency",
                    source_node=source_matches[0],
                    target_node=target_matches[0],
                    package_boundary="cross-package",
                    semantic_proof=True,
                    evidence_ids=["workspace-semantic-index-guardrails"],
                    evidence_boundary=str(edge.get("source_authority", "") or "workspace package edge"),
                )
            )

    edges.extend(_compiler_graph_edges_from_manifest(raw_compiler_fields, nodes_by_display))
    reference_candidates, candidate_edges = _reference_candidates(source_index, nodes_by_display)
    edges.extend(candidate_edges)

    nodes = sorted(
        nodes,
        key=lambda node: (
            str(node["symbol_kind"]),
            str(node["canonical_name"]),
            _range_sort_key(node["location"]),
            str(node["node_id"]),
        ),
    )
    edges = sorted(
        edges,
        key=lambda edge: (
            str(edge["edge_kind"]),
            str(edge["source_node_id"]),
            str(edge["target_node_id"]),
            _range_sort_key(edge["source_range"]),
            str(edge["edge_id"]),
        ),
    )
    diagnostics = _source_graph_diagnostics(
        nodes=nodes,
        reference_candidates=reference_candidates,
        semantic_reference_closure=semantic_reference_closure,
    )
    semantic_tokens = [_semantic_token_for_node(node, semantic_proof=True) for node in declaration_nodes]
    semantic_tokens = sorted(
        semantic_tokens,
        key=lambda token: (
            int(token["compiler_range"]["line"]),
            int(token["compiler_range"]["column"]),
            str(token["token_type"]),
            str(token["node_id"]),
        ),
    )

    consumer_evidence = ["compiler-source-graph-artifact", "compile-manifest-declaration-coordinates"]
    references_status = {
        "supported": semantic_reference_closure,
        "support_class": "source-graph-backed" if semantic_reference_closure else "source-graph-fail-closed",
        "evidence_ids": consumer_evidence if semantic_reference_closure else [],
        "fail_closed": not semantic_reference_closure,
        "unpublished_reason": ""
        if semantic_reference_closure
        else "native compiler semantic reference closure is missing; lexical candidates are not authoritative",
    }
    rename_status = {
        "supported": False,
        "support_class": "source-graph-fail-closed",
        "evidence_ids": [],
        "fail_closed": True,
        "unpublished_reason": (
            "safe rename remains disabled until semantic references, collision scopes, selector disambiguation, "
            "and package write-boundary proofs are emitted"
        ),
    }
    semantic_tokens_status = {
        "supported": semantic_reference_closure,
        "support_class": "source-graph-backed" if semantic_reference_closure else "source-graph-fail-closed",
        "evidence_ids": consumer_evidence if semantic_reference_closure else [],
        "fail_closed": not semantic_reference_closure,
        "unpublished_reason": ""
        if semantic_reference_closure
        else "declaration token anchors exist, but full semantic token publication requires compiler-owned reference classification",
    }
    graph_inputs = [
        "compile-manifest-declaration-coordinates",
        "workspace-semantic-index-guardrails",
        "source-index-unverified-reference-candidates",
    ]
    if compiler_field_summaries:
        graph_inputs.append("manifest-source-graph-fields")
    available = bool(manifest_path and declaration_nodes)
    digest_input = {
        "source_path": source_path,
        "module_name": module_name,
        "manifest_path": manifest_path or "",
        "nodes": nodes,
        "edges": edges,
        "reference_candidates": reference_candidates,
        "compiler_fields": compiler_field_summaries,
        "source_index_digest": source_index.get("source_index_digest", ""),
        "workspace_index_digest": workspace_index.get("workspace_index_digest", ""),
    }
    return {
        "contract_id": "objc3c.developer.tooling.source.graph.v1",
        "source_path": source_path,
        "module_name": module_name,
        "available": available,
        "fail_closed": not (semantic_reference_closure and available),
        "support_class": "compiler-source-graph-artifact"
        if semantic_reference_closure
        else "compiler-declaration-graph-with-fail-closed-reference-candidates",
        "graph_truth_model": (
            "compiler-owned declarations plus checked package provenance; lexical candidates are non-authoritative "
            "until native semantic reference closure is emitted"
        ),
        "evidence": {
            "source_truth_inputs": graph_inputs,
            "manifest_path": manifest_path or "",
            "compiler_graph_fields": compiler_field_summaries,
            "source_index_digest": str(source_index.get("source_index_digest", "") or ""),
            "workspace_index_digest": str(workspace_index.get("workspace_index_digest", "") or ""),
            "compiler_declaration_coordinates": bool(declaration_nodes),
            "native_compiler_source_graph_present": bool(compiler_field_summaries),
            "semantic_reference_closure": semantic_reference_closure,
            "lexical_candidates_authoritative": False,
            "reference_authority": "compiler-owned semantic reference closure required",
            "package_authority": "workspace-index package guardrails",
            "unsupported_authoritative_claims": [
                "regex-only reference truth",
                "rename without semantic reference closure",
                "semantic tokens from lexical tokenization alone",
            ],
        },
        "node_count": len(nodes),
        "edge_count": len(edges),
        "declaration_node_count": len(declaration_nodes),
        "reference_candidate_count": len(reference_candidates),
        "semantic_reference_count": sum(
            1 for edge in edges if edge["edge_kind"] == "declaration-to-reference" and edge["semantic_proof"] is True
        ),
        "package_node_count": len(package_nodes),
        "package_dependency_edge_count": sum(1 for edge in edges if edge["edge_kind"] == "package-dependency"),
        "semantic_token_count": len(semantic_tokens),
        "nodes": nodes,
        "edges": edges,
        "declarations": [
            {
                "node_id": node["node_id"],
                "symbol": node["display_name"],
                "kind": node["symbol_kind"],
                "definition_target": node["definition_target"],
            }
            for node in declaration_nodes
        ],
        "reference_candidates": reference_candidates,
        "navigation_consumers": {
            "definition_targets": [
                {
                    "node_id": node["node_id"],
                    "name": node["display_name"],
                    "kind": node["symbol_kind"],
                    "target_uri": node["definition_target"]["target_uri"],
                    "target_range": node["definition_target"]["target_range"],
                    "target_compiler_range": node["definition_target"]["target_compiler_range"],
                    "semantic_proof": True,
                }
                for node in declaration_nodes
            ],
            "implementation_targets": [
                {
                    "source_node_id": edge["source_node_id"],
                    "target_node_id": edge["target_node_id"],
                    "edge_kind": edge["edge_kind"],
                }
                for edge in edges
                if edge["edge_kind"] in {"compiler-source-graph-edge", "implementation-to-interface"}
            ],
            "generated_accessor_targets": [],
            "semantic_token_source": "source_graph.semantic_tokens.tokens",
        },
        "references": {
            **references_status,
            "query_model": "node-id-to-semantic-reference-edges",
            "candidate_count": len(reference_candidates),
            "queries": [],
            "diagnostics": [
                diagnostic
                for diagnostic in diagnostics
                if diagnostic["code"].startswith("O3SG_REFERENCE")
            ],
        },
        "rename": {
            **rename_status,
            "edit_model": "workspace-edit-from-semantic-reference-closure",
            "diagnostics": [
                diagnostic
                for diagnostic in diagnostics
                if diagnostic["code"].startswith("O3SG_RENAME")
            ],
            "negative_case_policy": [
                "zero semantic nodes",
                "multiple semantic nodes",
                "generated/private/internal targets",
                "readonly package boundary",
                "scope collision",
                "selector ambiguity",
                "public ABI rename without governance approval",
                "insufficient macro expansion provenance",
            ],
        },
        "semantic_tokens": {
            **semantic_tokens_status,
            "legend": {
                "token_types": [
                    "namespace",
                    "type",
                    "class",
                    "interface",
                    "function",
                    "method",
                    "property",
                    "variable",
                    "parameter",
                    "enumMember",
                ],
                "token_modifiers": [
                    "declaration",
                    "definition",
                    "readonly",
                    "generated",
                    "public",
                    "internal",
                    "private",
                    "unverified-reference",
                ],
            },
            "tokens": semantic_tokens,
        },
        "package_provenance": {
            "local_package_id": local_package_id,
            "package_count": int(workspace_index.get("package_count", 0) or 0),
            "workspace_index_digest": str(workspace_index.get("workspace_index_digest", "") or ""),
            "package_dependencies": workspace_index.get("cross_package_edges", []),
        },
        "consumer_capabilities": {
            "references": references_status,
            "rename": rename_status,
            "semanticTokens": semantic_tokens_status,
        },
        "diagnostics": diagnostics,
        "source_graph_digest": _stable_digest(digest_input),
        "deterministic_ordering": "node-kind-canonical-name-range-then-edge-kind-source-target-range",
        "retired_route_reason": ""
        if available
        else "compile produced no manifest-backed declaration surface for source graph publication",
        "remaining_native_compiler_work": [
            "emit structured declaration-to-reference semantic edges with source spans from sema",
            "emit scope collision tables and selector disambiguation records for rename",
            "emit generated accessor and macro expansion provenance as structured graph nodes",
            "publish package write-boundary proofs for cross-package workspace edits",
        ],
    }

