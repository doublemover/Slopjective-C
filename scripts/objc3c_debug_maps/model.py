"""Fail-closed validation for Objective-C 3 debug/source-map artifacts."""

from __future__ import annotations

import hashlib
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import ROOT, display_path, repo_rel, resolve_repo_path, resolve_repo_path_inside


CONTRACT_ID = "objc3c.debug-source-maps.v1"
VALIDATION_CONTRACT_ID = "objc3c.debug-source-maps.validation.v1"
INSPECTION_CONTRACT_ID = "objc3c.debug-map.inspection.v1"
DEFAULT_FIXTURE_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "debug_source_maps"
    / "positive.json"
)

REQUIRED_SOURCE_MAP_RECORD_KINDS = frozenset(
    {
        "module",
        "declaration",
        "statement",
        "expression",
        "function",
        "method",
        "message-send",
        "property-access",
        "generated-accessor",
        "macro-expansion",
        "runtime-helper-call",
        "stdlib-lowering-call",
        "async-suspension-point",
        "error-bridge-edge",
        "ownership-cleanup-edge",
        "optimization-transform-edge",
    }
)
REQUIRED_CAPABILITY_ROWS = frozenset(
    {
        "compiler.artifacts.source-map",
        "compiler.artifacts.native-line-table",
        "tooling.debug.debug-map",
    }
)
REQUIRED_OPTIMIZATION_TRANSFORMS = frozenset(
    {
        "nil-receiver-folding",
        "direct-dispatch",
        "retained-result-cleanup",
        "devirtualization",
        "method-inlining",
        "cache-aware-dispatch",
    }
)
VALID_CLASSIFICATIONS = frozenset({"original", "generated"})
HEX_DIGEST_LENGTH = 64


@dataclass(frozen=True)
class Diagnostic:
    code: str
    message: str
    path: str

    def to_payload(self) -> dict[str, str]:
        return {"code": self.code, "message": self.message, "path": self.path}


@dataclass(frozen=True)
class SourceRange:
    line: int
    column: int
    end_line: int
    end_column: int

    @classmethod
    def from_payload(cls, payload: object) -> "SourceRange":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            line=_safe_int(item.get("line")),
            column=_safe_int(item.get("column")),
            end_line=_safe_int(item.get("end_line")),
            end_column=_safe_int(item.get("end_column")),
        )

    def is_valid(self) -> bool:
        if self.line < 1 or self.column < 1 or self.end_line < self.line:
            return False
        if self.end_line == self.line:
            return self.end_column > self.column
        return self.end_column >= 1

    def to_payload(self) -> dict[str, int]:
        return {
            "line": self.line,
            "column": self.column,
            "end_line": self.end_line,
            "end_column": self.end_column,
        }


@dataclass(frozen=True)
class SourceGraphNode:
    node_id: str
    symbol: str
    node_kind: str
    source_digest: str
    package_boundary_id: str
    source_range: SourceRange

    @classmethod
    def from_payload(cls, payload: object) -> "SourceGraphNode":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            node_id=_safe_str(item.get("id")),
            symbol=_safe_str(item.get("symbol")),
            node_kind=_safe_str(item.get("kind")),
            source_digest=_safe_str(item.get("source_digest")),
            package_boundary_id=_safe_str(item.get("package_boundary_id")),
            source_range=SourceRange.from_payload(item.get("range")),
        )


@dataclass(frozen=True)
class SourceMapEntry:
    entry_id: str
    record_kind: str
    source_file: str
    source_range: SourceRange
    ast_node_id: str
    source_graph_node_id: str
    source_digest: str
    lowering_stage_id: str
    ir_anchor: str
    ir_symbol: str
    object_debug_line_anchor: str
    native_symbol: str
    generated_artifact_id: str
    generated: bool
    classification: str
    provenance_link_id: str
    capability_rows: tuple[str, ...]
    language_service_anchor_ids: tuple[str, ...]
    optimization_claim_ids: tuple[str, ...]
    runtime_anchor_ids: tuple[str, ...]
    package_boundary_id: str
    native_line_table_row_ids: tuple[str, ...]

    @classmethod
    def from_payload(cls, payload: object) -> "SourceMapEntry":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            entry_id=_safe_str(item.get("entry_id")),
            record_kind=_safe_str(item.get("record_kind")),
            source_file=_safe_str(item.get("source_file")),
            source_range=SourceRange.from_payload(item.get("source_range")),
            ast_node_id=_safe_str(item.get("ast_node_id")),
            source_graph_node_id=_safe_str(item.get("source_graph_node_id")),
            source_digest=_safe_str(item.get("source_digest")),
            lowering_stage_id=_safe_str(item.get("lowering_stage_id")),
            ir_anchor=_safe_str(item.get("ir_anchor")),
            ir_symbol=_safe_str(item.get("ir_symbol")),
            object_debug_line_anchor=_safe_str(item.get("object_debug_line_anchor")),
            native_symbol=_safe_str(item.get("native_symbol")),
            generated_artifact_id=_safe_str(item.get("generated_artifact_id")),
            generated=item.get("generated") is True,
            classification=_safe_str(item.get("classification")),
            provenance_link_id=_safe_str(item.get("provenance_link_id")),
            capability_rows=_tuple_str(item.get("capability_rows")),
            language_service_anchor_ids=_tuple_str(item.get("language_service_anchor_ids")),
            optimization_claim_ids=_tuple_str(item.get("optimization_claim_ids")),
            runtime_anchor_ids=_tuple_str(item.get("runtime_anchor_ids")),
            package_boundary_id=_safe_str(item.get("package_boundary_id")),
            native_line_table_row_ids=_tuple_str(item.get("native_line_table_row_ids")),
        )


@dataclass(frozen=True)
class DebugMapEntry:
    entry_id: str
    source_map_entry_id: str
    source_graph_node_id: str
    source_digest: str
    ir_symbol: str
    native_symbol: str
    object_debug_line_anchor: str
    runtime_anchor_ids: tuple[str, ...]
    hover_anchor_id: str
    definition_anchor_id: str

    @classmethod
    def from_payload(cls, payload: object) -> "DebugMapEntry":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            entry_id=_safe_str(item.get("entry_id")),
            source_map_entry_id=_safe_str(item.get("source_map_entry_id")),
            source_graph_node_id=_safe_str(item.get("source_graph_node_id")),
            source_digest=_safe_str(item.get("source_digest")),
            ir_symbol=_safe_str(item.get("ir_symbol")),
            native_symbol=_safe_str(item.get("native_symbol")),
            object_debug_line_anchor=_safe_str(item.get("object_debug_line_anchor")),
            runtime_anchor_ids=_tuple_str(item.get("runtime_anchor_ids")),
            hover_anchor_id=_safe_str(item.get("hover_anchor_id")),
            definition_anchor_id=_safe_str(item.get("definition_anchor_id")),
        )


@dataclass(frozen=True)
class NativeLineTableRow:
    row_id: str
    source_map_entry_id: str
    source_graph_node_id: str
    source_file: str
    source_digest: str
    native_symbol: str
    object_debug_line_anchor: str
    package_boundary_id: str
    source_range: SourceRange
    native_line: int

    @classmethod
    def from_payload(cls, payload: object) -> "NativeLineTableRow":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            row_id=_safe_str(item.get("row_id")),
            source_map_entry_id=_safe_str(item.get("source_map_entry_id")),
            source_graph_node_id=_safe_str(item.get("source_graph_node_id")),
            source_file=_safe_str(item.get("source_file")),
            source_digest=_safe_str(item.get("source_digest")),
            native_symbol=_safe_str(item.get("native_symbol")),
            object_debug_line_anchor=_safe_str(item.get("object_debug_line_anchor")),
            package_boundary_id=_safe_str(item.get("package_boundary_id")),
            source_range=SourceRange.from_payload(item.get("source_range")),
            native_line=_safe_int(item.get("native_line")),
        )


@dataclass(frozen=True)
class NativeRange:
    start_offset: int
    end_offset: int

    @classmethod
    def from_payload(cls, payload: object) -> "NativeRange":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            start_offset=_safe_int(item.get("start_offset")),
            end_offset=_safe_int(item.get("end_offset")),
        )

    def is_valid(self) -> bool:
        return self.start_offset >= 0 and self.end_offset > self.start_offset


@dataclass(frozen=True)
class InlineFrame:
    frame_id: str
    caller_source_map_entry_id: str
    callee_source_map_entry_id: str
    callsite_span: SourceRange
    callee_body_span: SourceRange
    caller_runtime_anchor_id: str
    callee_runtime_anchor_id: str
    optimization_proof_id: str
    emitted_ir_anchor: str
    emitted_native_symbol: str

    @classmethod
    def from_payload(cls, payload: object) -> "InlineFrame":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            frame_id=_safe_str(item.get("frame_id")),
            caller_source_map_entry_id=_safe_str(item.get("caller_source_map_entry_id")),
            callee_source_map_entry_id=_safe_str(item.get("callee_source_map_entry_id")),
            callsite_span=SourceRange.from_payload(item.get("callsite_span")),
            callee_body_span=SourceRange.from_payload(item.get("callee_body_span")),
            caller_runtime_anchor_id=_safe_str(item.get("caller_runtime_anchor_id")),
            callee_runtime_anchor_id=_safe_str(item.get("callee_runtime_anchor_id")),
            optimization_proof_id=_safe_str(item.get("optimization_proof_id")),
            emitted_ir_anchor=_safe_str(item.get("emitted_ir_anchor")),
            emitted_native_symbol=_safe_str(item.get("emitted_native_symbol")),
        )


@dataclass(frozen=True)
class NativeInlineRange:
    range_id: str
    frame_id: str
    artifact_symbol_id: str
    native_range: NativeRange
    nesting_depth: int

    @classmethod
    def from_payload(cls, payload: object) -> "NativeInlineRange":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            range_id=_safe_str(item.get("range_id")),
            frame_id=_safe_str(item.get("frame_id")),
            artifact_symbol_id=_safe_str(item.get("artifact_symbol_id")),
            native_range=NativeRange.from_payload(item.get("native_range")),
            nesting_depth=_safe_int(item.get("nesting_depth")),
        )


@dataclass(frozen=True)
class InlineDebugChain:
    chain_id: str
    outer_frame_id: str
    inner_frame_id: str
    optimization_proof_id: str
    ordering_index: int

    @classmethod
    def from_payload(cls, payload: object) -> "InlineDebugChain":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            chain_id=_safe_str(item.get("chain_id")),
            outer_frame_id=_safe_str(item.get("outer_frame_id")),
            inner_frame_id=_safe_str(item.get("inner_frame_id")),
            optimization_proof_id=_safe_str(item.get("optimization_proof_id")),
            ordering_index=_safe_int(item.get("ordering_index")),
        )


@dataclass(frozen=True)
class InlineFrameFailure:
    failure_id: str
    native_range: NativeRange
    failure_code: str
    missing_identity: str

    @classmethod
    def from_payload(cls, payload: object) -> "InlineFrameFailure":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            failure_id=_safe_str(item.get("failure_id")),
            native_range=NativeRange.from_payload(item.get("native_range")),
            failure_code=_safe_str(item.get("failure_code")),
            missing_identity=_safe_str(item.get("missing_identity")),
        )


@dataclass(frozen=True)
class ProvenanceLink:
    link_id: str
    source_graph_node_id: str
    source_map_entry_id: str
    generated_artifact_id: str
    source_digest: str
    kind: str

    @classmethod
    def from_payload(cls, payload: object) -> "ProvenanceLink":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            link_id=_safe_str(item.get("link_id")),
            source_graph_node_id=_safe_str(item.get("source_graph_node_id")),
            source_map_entry_id=_safe_str(item.get("source_map_entry_id")),
            generated_artifact_id=_safe_str(item.get("generated_artifact_id")),
            source_digest=_safe_str(item.get("source_digest")),
            kind=_safe_str(item.get("kind")),
        )


@dataclass(frozen=True)
class LanguageServiceAnchor:
    anchor_id: str
    anchor_kind: str
    source_graph_node_id: str
    source_digest: str
    symbol: str

    @classmethod
    def from_payload(cls, payload: object) -> "LanguageServiceAnchor":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            anchor_id=_safe_str(item.get("anchor_id")),
            anchor_kind=_safe_str(item.get("kind")),
            source_graph_node_id=_safe_str(item.get("source_graph_node_id")),
            source_digest=_safe_str(item.get("source_digest")),
            symbol=_safe_str(item.get("symbol")),
        )


@dataclass(frozen=True)
class OptimizationPreservationClaim:
    claim_id: str
    transform_kind: str
    source_map_entry_ids: tuple[str, ...]
    native_line_table_row_ids: tuple[str, ...]
    preserves_source_map: bool
    invalidates_source_map: bool
    debugger_safe: bool
    diagnostic: str

    @classmethod
    def from_payload(cls, payload: object) -> "OptimizationPreservationClaim":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            claim_id=_safe_str(item.get("claim_id")),
            transform_kind=_safe_str(item.get("transform_kind")),
            source_map_entry_ids=_tuple_str(item.get("source_map_entry_ids")),
            native_line_table_row_ids=_tuple_str(item.get("native_line_table_row_ids")),
            preserves_source_map=item.get("preserves_source_map") is True,
            invalidates_source_map=item.get("invalidates_source_map") is True,
            debugger_safe=item.get("debugger_safe") is True,
            diagnostic=_safe_str(item.get("diagnostic")),
        )


@dataclass(frozen=True)
class PackageBoundary:
    boundary_id: str
    package_id: str
    module_id: str
    source_graph_node_ids: tuple[str, ...]
    source_digest: str

    @classmethod
    def from_payload(cls, payload: object) -> "PackageBoundary":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            boundary_id=_safe_str(item.get("boundary_id")),
            package_id=_safe_str(item.get("package_id")),
            module_id=_safe_str(item.get("module_id")),
            source_graph_node_ids=_tuple_str(item.get("source_graph_node_ids")),
            source_digest=_safe_str(item.get("source_digest")),
        )


@dataclass(frozen=True)
class CapabilityRow:
    row_id: str
    status: str
    evidence: tuple[str, ...]

    @classmethod
    def from_payload(cls, payload: object) -> "CapabilityRow":
        item = payload if isinstance(payload, dict) else {}
        return cls(
            row_id=_safe_str(item.get("row_id")),
            status=_safe_str(item.get("status")),
            evidence=_tuple_str(item.get("evidence")),
        )


@dataclass(frozen=True)
class DebugSourceMapBundle:
    path: Path
    payload: dict[str, Any]
    load_diagnostics: tuple[Diagnostic, ...]
    source_path: Path
    source_display_path: str
    expected_source_digest: str
    statement_stepping_requested: bool
    statement_stepping_supported: bool
    source_graph_nodes: tuple[SourceGraphNode, ...]
    source_maps: tuple[SourceMapEntry, ...]
    debug_maps: tuple[DebugMapEntry, ...]
    native_line_tables: tuple[NativeLineTableRow, ...]
    inline_frames: tuple[InlineFrame, ...]
    native_inline_ranges: tuple[NativeInlineRange, ...]
    inline_debug_chains: tuple[InlineDebugChain, ...]
    inline_frame_failures: tuple[InlineFrameFailure, ...]
    provenance_links: tuple[ProvenanceLink, ...]
    language_service_anchors: tuple[LanguageServiceAnchor, ...]
    optimization_claims: tuple[OptimizationPreservationClaim, ...]
    package_boundaries: tuple[PackageBoundary, ...]
    capability_rows: tuple[CapabilityRow, ...]
    required_record_kinds: tuple[str, ...]


@dataclass(frozen=True)
class ValidationResult:
    ok: bool
    diagnostics: tuple[Diagnostic, ...]
    bundle_path: str
    source_path: str

    def to_payload(self) -> dict[str, object]:
        return {
            "ok": self.ok,
            "bundle_path": self.bundle_path,
            "source_path": self.source_path,
            "diagnostics": [diagnostic.to_payload() for diagnostic in self.diagnostics],
        }


def _safe_str(value: object) -> str:
    return value if isinstance(value, str) else ""


def _safe_int(value: object) -> int:
    return value if isinstance(value, int) and not isinstance(value, bool) else 0


def _tuple_str(value: object) -> tuple[str, ...]:
    if not isinstance(value, list):
        return ()
    return tuple(item for item in value if isinstance(item, str))


def _list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _diag(code: str, message: str, path: str) -> Diagnostic:
    return Diagnostic(code=code, message=message, path=path)


def _file_digest(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _is_sha256_digest(value: str) -> bool:
    return len(value) == HEX_DIGEST_LENGTH and all(ch in "0123456789abcdef" for ch in value)


def _stable_source_path(raw_path: object, diagnostics: list[Diagnostic]) -> tuple[Path, str]:
    if not isinstance(raw_path, str) or not raw_path.strip():
        diagnostics.append(
            _diag("schema-missing-field", "source.path must be a non-empty repository-relative string", "source.path")
        )
        return ROOT, ""
    if raw_path != raw_path.strip() or "\\" in raw_path:
        diagnostics.append(
            _diag("source-path-unstable", "source.path must be trimmed and use forward slashes", "source.path")
        )
    path = Path(raw_path)
    if path.is_absolute() or ".." in path.parts:
        diagnostics.append(
            _diag("source-path-unstable", "source.path must stay inside the repository", "source.path")
        )
        return ROOT / raw_path, raw_path
    try:
        resolved = resolve_repo_path_inside(raw_path)
    except ValueError:
        diagnostics.append(
            _diag("source-path-unstable", "source.path must resolve inside the repository", "source.path")
        )
        return ROOT / raw_path, raw_path
    return resolved, repo_rel(resolved)


def _load_json_payload(bundle_path: Path) -> tuple[dict[str, Any], tuple[Diagnostic, ...]]:
    try:
        raw = bundle_path.read_text(encoding="utf-8")
    except OSError as exc:
        return {}, (
            _diag("bundle-read-failed", f"unable to read debug/source-map bundle: {exc}", display_path(bundle_path)),
        )
    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return {}, (
            _diag("bundle-json-invalid", f"invalid JSON at {exc.lineno}:{exc.colno}: {exc.msg}", display_path(bundle_path)),
        )
    if not isinstance(payload, dict):
        return {}, (
            _diag("schema-type", "debug/source-map bundle must be a JSON object", "$"),
        )
    return payload, ()


def _schema_diagnostics(payload: dict[str, Any]) -> list[Diagnostic]:
    diagnostics: list[Diagnostic] = []
    required_top_level = (
        "contract_id",
        "source",
        "source_graph",
        "source_maps",
        "debug_maps",
        "native_line_tables",
        "inline_frames",
        "provenance_links",
        "language_service",
        "optimization_preservation",
        "package_boundaries",
        "capabilities",
        "debug_policy",
    )
    for field in required_top_level:
        if field not in payload:
            diagnostics.append(_diag("schema-missing-field", f"missing top-level field: {field}", field))

    list_fields = ("source_maps", "debug_maps", "native_line_tables", "provenance_links")
    for field in list_fields:
        if field in payload and not isinstance(payload[field], list):
            diagnostics.append(_diag("schema-type", f"{field} must be a list", field))

    object_fields = (
        "source",
        "source_graph",
        "inline_frames",
        "language_service",
        "optimization_preservation",
        "package_boundaries",
        "capabilities",
        "debug_policy",
    )
    for field in object_fields:
        if field in payload and not isinstance(payload[field], dict):
            diagnostics.append(_diag("schema-type", f"{field} must be an object", field))

    _validate_list_items(_object(payload.get("source_graph")).get("nodes"), "source_graph.nodes", diagnostics)
    _validate_list_items(_object(payload.get("inline_frames")).get("frames"), "inline_frames.frames", diagnostics)
    _validate_list_items(_object(payload.get("inline_frames")).get("native_ranges"), "inline_frames.native_ranges", diagnostics)
    _validate_list_items(_object(payload.get("inline_frames")).get("chains"), "inline_frames.chains", diagnostics)
    _validate_list_items(_object(payload.get("inline_frames")).get("failures"), "inline_frames.failures", diagnostics)
    _validate_list_items(_object(payload.get("language_service")).get("anchors"), "language_service.anchors", diagnostics)
    _validate_list_items(_object(payload.get("optimization_preservation")).get("claims"), "optimization_preservation.claims", diagnostics)
    _validate_list_items(_object(payload.get("package_boundaries")).get("boundaries"), "package_boundaries.boundaries", diagnostics)
    _validate_list_items(_object(payload.get("capabilities")).get("rows"), "capabilities.rows", diagnostics)
    _validate_list_items(payload.get("required_record_kinds", []), "required_record_kinds", diagnostics, item_type=str)
    return diagnostics


def _validate_list_items(
    value: object,
    path: str,
    diagnostics: list[Diagnostic],
    *,
    item_type: type = dict,
) -> None:
    if not isinstance(value, list):
        diagnostics.append(_diag("schema-type", f"{path} must be a list", path))
        return
    for index, item in enumerate(value):
        if not isinstance(item, item_type):
            diagnostics.append(
                _diag("schema-type", f"{path}[{index}] must be a {item_type.__name__}", f"{path}[{index}]")
            )


def load_bundle(path: Path | str) -> DebugSourceMapBundle:
    bundle_path = resolve_repo_path(path)
    payload, load_diagnostics = _load_json_payload(bundle_path)
    source = _object(payload.get("source"))
    path_diagnostics: list[Diagnostic] = []
    source_path, source_display_path = _stable_source_path(source.get("path"), path_diagnostics)
    source_graph = _object(payload.get("source_graph"))
    inline_frames = _object(payload.get("inline_frames"))
    language_service = _object(payload.get("language_service"))
    optimization = _object(payload.get("optimization_preservation"))
    packages = _object(payload.get("package_boundaries"))
    capabilities = _object(payload.get("capabilities"))
    debug_policy = _object(payload.get("debug_policy"))

    return DebugSourceMapBundle(
        path=bundle_path,
        payload=payload,
        load_diagnostics=(*load_diagnostics, *path_diagnostics),
        source_path=source_path,
        source_display_path=source_display_path,
        expected_source_digest=_safe_str(source.get("sha256")),
        statement_stepping_requested=debug_policy.get("statement_stepping_requested") is True,
        statement_stepping_supported=debug_policy.get("statement_stepping_supported") is True,
        source_graph_nodes=tuple(SourceGraphNode.from_payload(item) for item in _list(source_graph.get("nodes"))),
        source_maps=tuple(SourceMapEntry.from_payload(item) for item in _list(payload.get("source_maps"))),
        debug_maps=tuple(DebugMapEntry.from_payload(item) for item in _list(payload.get("debug_maps"))),
        native_line_tables=tuple(NativeLineTableRow.from_payload(item) for item in _list(payload.get("native_line_tables"))),
        inline_frames=tuple(InlineFrame.from_payload(item) for item in _list(inline_frames.get("frames"))),
        native_inline_ranges=tuple(NativeInlineRange.from_payload(item) for item in _list(inline_frames.get("native_ranges"))),
        inline_debug_chains=tuple(InlineDebugChain.from_payload(item) for item in _list(inline_frames.get("chains"))),
        inline_frame_failures=tuple(InlineFrameFailure.from_payload(item) for item in _list(inline_frames.get("failures"))),
        provenance_links=tuple(ProvenanceLink.from_payload(item) for item in _list(payload.get("provenance_links"))),
        language_service_anchors=tuple(LanguageServiceAnchor.from_payload(item) for item in _list(language_service.get("anchors"))),
        optimization_claims=tuple(OptimizationPreservationClaim.from_payload(item) for item in _list(optimization.get("claims"))),
        package_boundaries=tuple(PackageBoundary.from_payload(item) for item in _list(packages.get("boundaries"))),
        capability_rows=tuple(CapabilityRow.from_payload(item) for item in _list(capabilities.get("rows"))),
        required_record_kinds=tuple(
            _tuple_str(payload.get("required_record_kinds")) or sorted(REQUIRED_SOURCE_MAP_RECORD_KINDS)
        ),
    )


def _index_by_id(items: tuple[Any, ...], attr: str, label: str, diagnostics: list[Diagnostic]) -> dict[str, Any]:
    indexed: dict[str, Any] = {}
    for offset, item in enumerate(items):
        item_id = str(getattr(item, attr))
        if not item_id:
            diagnostics.append(_diag("missing-id", f"{label} is missing a stable id", f"{label}[{offset}]"))
            continue
        if item_id in indexed:
            diagnostics.append(_diag("duplicate-id", f"{label} id is duplicated: {item_id}", item_id))
            continue
        indexed[item_id] = item
    return indexed


def validate_bundle(bundle: DebugSourceMapBundle) -> ValidationResult:
    diagnostics: list[Diagnostic] = [*bundle.load_diagnostics, *_schema_diagnostics(bundle.payload)]
    payload = bundle.payload
    if payload.get("contract_id") != CONTRACT_ID:
        diagnostics.append(_diag("contract-id", "debug/source-map contract id drifted", "contract_id"))

    if not bundle.source_path.is_file():
        diagnostics.append(_diag("source-missing", "source file for debug/source maps is missing", "source.path"))
        actual_source_digest = ""
    else:
        actual_source_digest = _file_digest(bundle.source_path)
    if not _is_sha256_digest(bundle.expected_source_digest):
        diagnostics.append(_diag("source-digest-invalid", "source.sha256 must be a lowercase SHA-256 digest", "source.sha256"))
    elif bundle.expected_source_digest != actual_source_digest:
        diagnostics.append(_diag("source-digest-stale", "source digest does not match source file", "source.sha256"))

    nodes = _index_by_id(bundle.source_graph_nodes, "node_id", "source_graph.nodes", diagnostics)
    source_maps = _index_by_id(bundle.source_maps, "entry_id", "source_maps", diagnostics)
    debug_maps = _index_by_id(bundle.debug_maps, "entry_id", "debug_maps", diagnostics)
    line_rows = _index_by_id(bundle.native_line_tables, "row_id", "native_line_tables", diagnostics)
    inline_frames = _index_by_id(bundle.inline_frames, "frame_id", "inline_frames.frames", diagnostics)
    _index_by_id(bundle.native_inline_ranges, "range_id", "inline_frames.native_ranges", diagnostics)
    _index_by_id(bundle.inline_debug_chains, "chain_id", "inline_frames.chains", diagnostics)
    links = _index_by_id(bundle.provenance_links, "link_id", "provenance_links", diagnostics)
    anchors = _index_by_id(bundle.language_service_anchors, "anchor_id", "language_service.anchors", diagnostics)
    claims = _index_by_id(bundle.optimization_claims, "claim_id", "optimization_preservation.claims", diagnostics)
    boundaries = _index_by_id(bundle.package_boundaries, "boundary_id", "package_boundaries.boundaries", diagnostics)
    capability_rows = _index_by_id(bundle.capability_rows, "row_id", "capabilities.rows", diagnostics)

    _validate_capabilities(bundle.capability_rows, capability_rows, diagnostics)
    _validate_debug_policy(bundle, diagnostics)
    _validate_required_record_kinds(bundle, diagnostics)
    _validate_required_optimization_hooks(bundle, diagnostics)

    for node in bundle.source_graph_nodes:
        _validate_source_graph_node(node, boundaries, actual_source_digest, diagnostics)

    for entry in bundle.source_maps:
        _validate_source_map_entry(
            entry,
            nodes,
            links,
            anchors,
            claims,
            line_rows,
            capability_rows,
            actual_source_digest,
            bundle.source_display_path,
            diagnostics,
        )

    for debug_map in bundle.debug_maps:
        _validate_debug_map(debug_map, source_maps, nodes, anchors, actual_source_digest, diagnostics)

    for row in bundle.native_line_tables:
        _validate_line_table_row(row, source_maps, nodes, actual_source_digest, bundle.source_display_path, diagnostics)

    for claim in bundle.optimization_claims:
        _validate_optimization_claim(claim, source_maps, line_rows, diagnostics)

    _validate_inline_frame_model(
        bundle,
        inline_frames,
        source_maps,
        claims,
        diagnostics,
    )

    for boundary in bundle.package_boundaries:
        if boundary.source_digest != actual_source_digest:
            diagnostics.append(
                _diag("source-digest-stale", f"package boundary source digest is stale: {boundary.boundary_id}", boundary.boundary_id)
            )
        if not boundary.package_id or not boundary.module_id:
            diagnostics.append(
                _diag("package-boundary-missing", f"package boundary identity is incomplete: {boundary.boundary_id}", boundary.boundary_id)
            )

    return ValidationResult(
        ok=not diagnostics,
        diagnostics=tuple(diagnostics),
        bundle_path=display_path(bundle.path),
        source_path=display_path(bundle.source_path),
    )


def _validate_capabilities(
    rows: tuple[CapabilityRow, ...],
    rows_by_id: dict[str, CapabilityRow],
    diagnostics: list[Diagnostic],
) -> None:
    missing = sorted(REQUIRED_CAPABILITY_ROWS - set(rows_by_id))
    for row_id in missing:
        diagnostics.append(_diag("capability-row-missing", f"required capability row is missing: {row_id}", row_id))
    for row in rows:
        if row.status not in {"supported", "reserved"}:
            diagnostics.append(_diag("capability-row-invalid", f"capability row has invalid status: {row.row_id}", row.row_id))
        if row.row_id in REQUIRED_CAPABILITY_ROWS and row.status != "supported":
            diagnostics.append(_diag("capability-row-invalid", f"required capability row is not supported: {row.row_id}", row.row_id))
        if not row.evidence:
            diagnostics.append(_diag("capability-row-missing", f"capability row lacks evidence: {row.row_id}", row.row_id))


def _validate_debug_policy(bundle: DebugSourceMapBundle, diagnostics: list[Diagnostic]) -> None:
    if bundle.statement_stepping_supported:
        diagnostics.append(
            _diag(
                "statement-stepping-overclaimed",
                "statement stepping must remain reserved until debugger consumer work lands",
                "debug_policy.statement_stepping_supported",
            )
        )
    if bundle.statement_stepping_requested:
        statement_entries = [entry for entry in bundle.source_maps if entry.record_kind == "statement"]
        if not statement_entries:
            diagnostics.append(
                _diag("line-table-evidence-missing", "statement stepping was requested without statement source-map records", "source_maps")
            )
        for entry in statement_entries:
            if not entry.native_line_table_row_ids:
                diagnostics.append(
                    _diag(
                        "line-table-evidence-missing",
                        f"statement stepping was requested without line-table evidence: {entry.entry_id}",
                        entry.entry_id,
                    )
                )


def _validate_required_record_kinds(bundle: DebugSourceMapBundle, diagnostics: list[Diagnostic]) -> None:
    required = set(bundle.required_record_kinds)
    unknown_required = sorted(required - REQUIRED_SOURCE_MAP_RECORD_KINDS)
    for record_kind in unknown_required:
        diagnostics.append(_diag("record-kind-unknown", f"unknown required source-map record kind: {record_kind}", record_kind))
    present = {entry.record_kind for entry in bundle.source_maps}
    for record_kind in sorted(required & REQUIRED_SOURCE_MAP_RECORD_KINDS - present):
        diagnostics.append(
            _diag("required-record-kind-missing", f"required source-map record kind is missing: {record_kind}", record_kind)
        )


def _validate_required_optimization_hooks(bundle: DebugSourceMapBundle, diagnostics: list[Diagnostic]) -> None:
    present = {claim.transform_kind for claim in bundle.optimization_claims}
    for transform in sorted(REQUIRED_OPTIMIZATION_TRANSFORMS - present):
        diagnostics.append(
            _diag("optimization-hook-missing", f"required optimization source-map hook is missing: {transform}", transform)
        )


def _validate_source_graph_node(
    node: SourceGraphNode,
    boundaries: dict[str, PackageBoundary],
    actual_source_digest: str,
    diagnostics: list[Diagnostic],
) -> None:
    if not node.node_kind:
        diagnostics.append(_diag("schema-missing-field", f"source graph node kind is missing: {node.node_id}", node.node_id))
    if node.source_digest != actual_source_digest:
        diagnostics.append(_diag("source-graph-digest-stale", f"source graph node digest is stale: {node.node_id}", node.node_id))
    if not node.source_range.is_valid():
        diagnostics.append(_diag("source-graph-range", f"source graph range is invalid: {node.node_id}", node.node_id))
    boundary = boundaries.get(node.package_boundary_id)
    if boundary is None:
        diagnostics.append(
            _diag("package-boundary-missing", f"source graph node package boundary is missing: {node.node_id}", node.node_id)
        )
    elif node.node_id not in boundary.source_graph_node_ids:
        diagnostics.append(
            _diag("package-boundary-mismatch", f"source graph node is outside its package boundary: {node.node_id}", node.node_id)
        )


def _validate_source_map_entry(
    entry: SourceMapEntry,
    nodes: dict[str, SourceGraphNode],
    links: dict[str, ProvenanceLink],
    anchors: dict[str, LanguageServiceAnchor],
    claims: dict[str, OptimizationPreservationClaim],
    line_rows: dict[str, NativeLineTableRow],
    capability_rows: dict[str, CapabilityRow],
    actual_source_digest: str,
    source_display_path: str,
    diagnostics: list[Diagnostic],
) -> None:
    node = nodes.get(entry.source_graph_node_id)
    if entry.record_kind not in REQUIRED_SOURCE_MAP_RECORD_KINDS:
        diagnostics.append(_diag("record-kind-unknown", f"source map record kind is unknown: {entry.entry_id}", entry.entry_id))
    if entry.source_file != source_display_path:
        diagnostics.append(_diag("source-path-drift", f"source map source file drifted: {entry.entry_id}", entry.entry_id))
    if not entry.source_range.is_valid():
        diagnostics.append(_diag("source-map-range", f"source map range is invalid: {entry.entry_id}", entry.entry_id))
    if not entry.ast_node_id:
        diagnostics.append(_diag("schema-missing-field", f"source map lacks AST node id: {entry.entry_id}", entry.entry_id))
    if not entry.lowering_stage_id or not entry.ir_anchor:
        diagnostics.append(_diag("schema-missing-field", f"source map lacks lowering or IR anchor: {entry.entry_id}", entry.entry_id))
    if not entry.object_debug_line_anchor:
        diagnostics.append(_diag("line-table-evidence-missing", f"source map lacks object/debug line anchor: {entry.entry_id}", entry.entry_id))
    if entry.classification not in VALID_CLASSIFICATIONS:
        diagnostics.append(_diag("classification-invalid", f"source map classification is invalid: {entry.entry_id}", entry.entry_id))
    if entry.generated != (entry.classification == "generated"):
        diagnostics.append(_diag("classification-invalid", f"source map generated flag disagrees with classification: {entry.entry_id}", entry.entry_id))
    if not entry.capability_rows:
        diagnostics.append(_diag("capability-row-missing", f"source map lacks capability rows: {entry.entry_id}", entry.entry_id))
    for row_id in entry.capability_rows:
        if row_id not in capability_rows:
            diagnostics.append(_diag("capability-row-missing", f"source map references missing capability row: {entry.entry_id}", entry.entry_id))
    if node is None:
        diagnostics.append(_diag("source-graph-node-missing", f"source map references missing source graph node: {entry.entry_id}", entry.entry_id))
        return
    if entry.source_digest != actual_source_digest or entry.source_digest != node.source_digest:
        diagnostics.append(_diag("source-digest-stale", f"source map digest is stale: {entry.entry_id}", entry.entry_id))
    if entry.package_boundary_id != node.package_boundary_id:
        diagnostics.append(
            _diag("package-boundary-mismatch", f"source map package boundary differs from source graph node: {entry.entry_id}", entry.entry_id)
        )
    if entry.generated and not entry.provenance_link_id:
        diagnostics.append(_diag("generated-provenance-missing", f"generated source map lacks provenance: {entry.entry_id}", entry.entry_id))
    link = links.get(entry.provenance_link_id)
    if entry.generated and link is None:
        diagnostics.append(
            _diag("generated-provenance-missing", f"generated source map provenance link is missing: {entry.entry_id}", entry.entry_id)
        )
    elif link is not None and (
        link.source_graph_node_id != entry.source_graph_node_id
        or link.source_map_entry_id != entry.entry_id
        or link.generated_artifact_id != entry.generated_artifact_id
        or link.source_digest != actual_source_digest
    ):
        diagnostics.append(_diag("provenance-link-drift", f"source map provenance link drifted: {entry.entry_id}", entry.entry_id))
    for anchor_id in entry.language_service_anchor_ids:
        anchor = anchors.get(anchor_id)
        if anchor is None:
            diagnostics.append(_diag("language-anchor-missing", f"source map references missing language anchor: {entry.entry_id}", entry.entry_id))
            continue
        if anchor.source_graph_node_id != entry.source_graph_node_id or anchor.source_digest != actual_source_digest:
            diagnostics.append(_diag("language-anchor-drift", f"language anchor drifted from source map: {entry.entry_id}", entry.entry_id))
    for row_id in entry.native_line_table_row_ids:
        row = line_rows.get(row_id)
        if row is None:
            diagnostics.append(_diag("line-table-row-missing", f"source map references missing line-table row: {entry.entry_id}", entry.entry_id))
            continue
        if (
            row.source_map_entry_id != entry.entry_id
            or row.source_graph_node_id != entry.source_graph_node_id
            or row.object_debug_line_anchor != entry.object_debug_line_anchor
        ):
            diagnostics.append(_diag("line-table-drift", f"native line-table row drifted from source map: {entry.entry_id}", entry.entry_id))
    for claim_id in entry.optimization_claim_ids:
        claim = claims.get(claim_id)
        if claim is None:
            diagnostics.append(_diag("optimization-claim-missing", f"source map references missing optimization claim: {entry.entry_id}", entry.entry_id))
            continue
        if entry.entry_id not in claim.source_map_entry_ids:
            diagnostics.append(
                _diag("optimization-preservation-missing", f"optimization claim omits source map entry: {entry.entry_id}", entry.entry_id)
            )


def _validate_debug_map(
    debug_map: DebugMapEntry,
    source_maps: dict[str, SourceMapEntry],
    nodes: dict[str, SourceGraphNode],
    anchors: dict[str, LanguageServiceAnchor],
    actual_source_digest: str,
    diagnostics: list[Diagnostic],
) -> None:
    entry = source_maps.get(debug_map.source_map_entry_id)
    node = nodes.get(debug_map.source_graph_node_id)
    if entry is None:
        diagnostics.append(_diag("source-map-entry-missing", f"debug map references missing source map: {debug_map.entry_id}", debug_map.entry_id))
        return
    if node is None:
        diagnostics.append(_diag("source-graph-node-missing", f"debug map references missing source graph node: {debug_map.entry_id}", debug_map.entry_id))
        return
    if (
        debug_map.source_graph_node_id != entry.source_graph_node_id
        or debug_map.source_digest != actual_source_digest
        or debug_map.ir_symbol != entry.ir_symbol
        or debug_map.native_symbol != entry.native_symbol
        or debug_map.object_debug_line_anchor != entry.object_debug_line_anchor
    ):
        diagnostics.append(_diag("debug-map-drift", f"debug map drifted from source map: {debug_map.entry_id}", debug_map.entry_id))
    if tuple(debug_map.runtime_anchor_ids) != tuple(entry.runtime_anchor_ids):
        diagnostics.append(_diag("debug-map-drift", f"debug map runtime anchors drifted from source map: {debug_map.entry_id}", debug_map.entry_id))
    for anchor_id in (debug_map.hover_anchor_id, debug_map.definition_anchor_id):
        if anchor_id not in anchors:
            diagnostics.append(_diag("language-anchor-missing", f"debug map references missing language anchor: {debug_map.entry_id}", debug_map.entry_id))


def _validate_line_table_row(
    row: NativeLineTableRow,
    source_maps: dict[str, SourceMapEntry],
    nodes: dict[str, SourceGraphNode],
    actual_source_digest: str,
    source_display_path: str,
    diagnostics: list[Diagnostic],
) -> None:
    node = nodes.get(row.source_graph_node_id)
    entry = source_maps.get(row.source_map_entry_id)
    if node is None:
        diagnostics.append(_diag("source-graph-node-missing", f"line-table row references missing source graph node: {row.row_id}", row.row_id))
        return
    if entry is None:
        diagnostics.append(_diag("source-map-entry-missing", f"line-table row references missing source map: {row.row_id}", row.row_id))
        return
    if row.source_file != source_display_path:
        diagnostics.append(_diag("source-path-drift", f"line-table source file drifted: {row.row_id}", row.row_id))
    if row.source_digest != actual_source_digest:
        diagnostics.append(_diag("source-digest-stale", f"native line-table digest is stale: {row.row_id}", row.row_id))
    if row.package_boundary_id != node.package_boundary_id:
        diagnostics.append(_diag("package-boundary-mismatch", f"line-table package boundary differs from source graph node: {row.row_id}", row.row_id))
    if (
        row.native_symbol != entry.native_symbol
        or row.object_debug_line_anchor != entry.object_debug_line_anchor
        or row.source_range != entry.source_range
        or row.native_line < 1
    ):
        diagnostics.append(_diag("line-table-drift", f"native line-table row drifted from source map: {row.row_id}", row.row_id))


def _validate_optimization_claim(
    claim: OptimizationPreservationClaim,
    source_maps: dict[str, SourceMapEntry],
    line_rows: dict[str, NativeLineTableRow],
    diagnostics: list[Diagnostic],
) -> None:
    if claim.transform_kind not in REQUIRED_OPTIMIZATION_TRANSFORMS:
        diagnostics.append(_diag("optimization-hook-unknown", f"optimization transform is unknown: {claim.claim_id}", claim.claim_id))
    if claim.preserves_source_map and claim.invalidates_source_map:
        diagnostics.append(_diag("optimization-preservation-invalid", f"optimization claim both preserves and invalidates source maps: {claim.claim_id}", claim.claim_id))
    if claim.debugger_safe and not claim.preserves_source_map:
        diagnostics.append(
            _diag("optimization-preservation-missing", f"debugger-safe optimization lacks source-map preservation: {claim.claim_id}", claim.claim_id)
        )
    if claim.debugger_safe and not claim.native_line_table_row_ids:
        diagnostics.append(
            _diag("optimization-preservation-missing", f"debugger-safe optimization lacks line-table preservation: {claim.claim_id}", claim.claim_id)
        )
    if not claim.preserves_source_map and not claim.invalidates_source_map and not claim.diagnostic:
        diagnostics.append(
            _diag("optimization-preservation-missing", f"optimization claim lacks preservation or invalidation proof: {claim.claim_id}", claim.claim_id)
        )
    for entry_id in claim.source_map_entry_ids:
        if entry_id not in source_maps:
            diagnostics.append(_diag("source-map-entry-missing", f"optimization claim references missing source map: {claim.claim_id}", claim.claim_id))
    for row_id in claim.native_line_table_row_ids:
        if row_id not in line_rows:
            diagnostics.append(_diag("line-table-row-missing", f"optimization claim references missing line-table row: {claim.claim_id}", claim.claim_id))


def _validate_inline_frame_model(
    bundle: DebugSourceMapBundle,
    frames_by_id: dict[str, InlineFrame],
    source_maps: dict[str, SourceMapEntry],
    claims: dict[str, OptimizationPreservationClaim],
    diagnostics: list[Diagnostic],
) -> None:
    ranges_by_frame: dict[str, list[NativeInlineRange]] = {}
    for native_range in bundle.native_inline_ranges:
        ranges_by_frame.setdefault(native_range.frame_id, []).append(native_range)

    for frame in bundle.inline_frames:
        _validate_inline_frame(frame, ranges_by_frame.get(frame.frame_id, []), source_maps, claims, diagnostics)

    for native_range in bundle.native_inline_ranges:
        frame = frames_by_id.get(native_range.frame_id)
        if frame is None:
            diagnostics.append(
                _diag(
                    "inline-frame-missing",
                    f"native inline range references missing inline frame: {native_range.range_id}",
                    native_range.range_id,
                )
            )
            continue
        if not native_range.native_range.is_valid() or native_range.nesting_depth < 0:
            diagnostics.append(
                _diag("inline-native-range-drift", f"native inline range is invalid: {native_range.range_id}", native_range.range_id)
            )
        if native_range.artifact_symbol_id != frame.emitted_native_symbol:
            diagnostics.append(
                _diag(
                    "inline-native-range-drift",
                    f"native inline range artifact symbol drifted from inline frame: {native_range.range_id}",
                    native_range.range_id,
                )
            )

    for chain in bundle.inline_debug_chains:
        outer_frame = frames_by_id.get(chain.outer_frame_id)
        inner_frame = frames_by_id.get(chain.inner_frame_id)
        if outer_frame is None or inner_frame is None:
            diagnostics.append(_diag("inline-frame-missing", f"inline debug chain references missing frame: {chain.chain_id}", chain.chain_id))
            continue
        if chain.ordering_index < 0:
            diagnostics.append(_diag("inline-debug-chain-drift", f"inline debug chain ordering is invalid: {chain.chain_id}", chain.chain_id))
        if chain.optimization_proof_id != inner_frame.optimization_proof_id:
            diagnostics.append(
                _diag(
                    "inline-debug-chain-drift",
                    f"inline debug chain optimization proof drifted from inner frame: {chain.chain_id}",
                    chain.chain_id,
                )
            )

    for failure in bundle.inline_frame_failures:
        if not failure.failure_code or not failure.missing_identity or not failure.native_range.is_valid():
            diagnostics.append(
                _diag("inline-frame-failure", f"inline-frame failure record is incomplete: {failure.failure_id}", failure.failure_id)
            )
        else:
            diagnostics.append(
                _diag("inline-frame-failure", f"inline-frame failure record is present: {failure.failure_id}", failure.failure_id)
            )

    if bundle.inline_frames and not bundle.inline_debug_chains:
        diagnostics.append(_diag("inline-debug-chain-missing", "inline frames require explicit debug-chain ordering", "inline_frames.chains"))
    if bundle.inline_debug_chains and not bundle.inline_frames:
        diagnostics.append(_diag("inline-frame-missing", "inline debug chains require inline frame records", "inline_frames.frames"))
    if bundle.native_inline_ranges and not bundle.inline_frames:
        diagnostics.append(_diag("inline-frame-missing", "native inline ranges require inline frame records", "inline_frames.frames"))


def _validate_inline_frame(
    frame: InlineFrame,
    native_ranges: list[NativeInlineRange],
    source_maps: dict[str, SourceMapEntry],
    claims: dict[str, OptimizationPreservationClaim],
    diagnostics: list[Diagnostic],
) -> None:
    caller = source_maps.get(frame.caller_source_map_entry_id)
    callee = source_maps.get(frame.callee_source_map_entry_id)
    if caller is None:
        diagnostics.append(_diag("source-map-entry-missing", f"inline frame references missing caller source map: {frame.frame_id}", frame.frame_id))
    if callee is None:
        diagnostics.append(_diag("source-map-entry-missing", f"inline frame references missing callee source map: {frame.frame_id}", frame.frame_id))
    if caller is None or callee is None:
        return

    if not frame.callsite_span.is_valid() or not frame.callee_body_span.is_valid():
        diagnostics.append(_diag("inline-frame-range", f"inline frame source spans are invalid: {frame.frame_id}", frame.frame_id))
    if frame.callsite_span != caller.source_range:
        diagnostics.append(_diag("inline-frame-range-drift", f"inline frame callsite span drifted from caller source map: {frame.frame_id}", frame.frame_id))
    if frame.callee_body_span.line < callee.source_range.line:
        diagnostics.append(_diag("inline-frame-range-drift", f"inline frame callee body precedes callee declaration: {frame.frame_id}", frame.frame_id))
    if frame.caller_runtime_anchor_id not in caller.runtime_anchor_ids:
        diagnostics.append(_diag("inline-frame-runtime-anchor-drift", f"inline frame caller runtime anchor drifted: {frame.frame_id}", frame.frame_id))
    if frame.callee_runtime_anchor_id not in callee.runtime_anchor_ids:
        diagnostics.append(_diag("inline-frame-runtime-anchor-drift", f"inline frame callee runtime anchor drifted: {frame.frame_id}", frame.frame_id))
    if frame.emitted_ir_anchor != caller.ir_anchor or frame.emitted_native_symbol != caller.native_symbol:
        diagnostics.append(_diag("inline-frame-artifact-drift", f"inline frame artifact anchor drifted from caller source map: {frame.frame_id}", frame.frame_id))

    claim = claims.get(frame.optimization_proof_id)
    if claim is None:
        diagnostics.append(_diag("inline-frame-optimization-missing", f"inline frame references missing optimization proof: {frame.frame_id}", frame.frame_id))
    elif claim.transform_kind != "method-inlining" or not claim.preserves_source_map or claim.invalidates_source_map:
        diagnostics.append(_diag("inline-frame-optimization-drift", f"inline frame optimization proof is not a preserving method-inlining proof: {frame.frame_id}", frame.frame_id))
    elif frame.callee_source_map_entry_id not in claim.source_map_entry_ids:
        diagnostics.append(_diag("inline-frame-optimization-drift", f"inline frame callee is absent from method-inlining proof: {frame.frame_id}", frame.frame_id))

    if not native_ranges:
        diagnostics.append(_diag("inline-native-range-missing", f"inline frame lacks native inline range: {frame.frame_id}", frame.frame_id))


def validate_bundle_path(path: Path | str = DEFAULT_FIXTURE_PATH) -> ValidationResult:
    try:
        bundle = load_bundle(path)
    except Exception as exc:  # pragma: no cover - last-resort deterministic CLI guard.
        bundle_path = resolve_repo_path(path)
        return ValidationResult(
            ok=False,
            diagnostics=(
                _diag("bundle-load-failed", f"unable to load debug/source-map bundle: {type(exc).__name__}: {exc}", display_path(bundle_path)),
            ),
            bundle_path=display_path(bundle_path),
            source_path="",
        )
    return validate_bundle(bundle)


def inspect_bundle_path(path: Path | str = DEFAULT_FIXTURE_PATH) -> dict[str, object]:
    try:
        bundle = load_bundle(path)
    except Exception as exc:  # pragma: no cover - last-resort deterministic CLI guard.
        bundle_path = resolve_repo_path(path)
        diagnostic = _diag(
            "bundle-load-failed",
            f"unable to inspect debug/source-map bundle: {type(exc).__name__}: {exc}",
            display_path(bundle_path),
        )
        return {
            "ok": False,
            "contract_id": INSPECTION_CONTRACT_ID,
            "bundle_path": display_path(bundle_path),
            "source_path": "",
            "source_map_count": 0,
            "debug_map_count": 0,
            "native_line_table_count": 0,
            "inline_frame_count": 0,
            "native_inline_range_count": 0,
            "inline_debug_chain_count": 0,
            "inline_frame_failure_count": 0,
            "record_kinds": [],
            "capability_rows": [],
            "optimization_transforms": [],
            "diagnostics": [diagnostic.to_payload()],
        }
    result = validate_bundle(bundle)
    return {
        "ok": result.ok,
        "contract_id": INSPECTION_CONTRACT_ID,
        "bundle_path": result.bundle_path,
        "source_path": result.source_path,
        "source_map_count": len(bundle.source_maps),
        "debug_map_count": len(bundle.debug_maps),
        "native_line_table_count": len(bundle.native_line_tables),
        "inline_frame_count": len(bundle.inline_frames),
        "native_inline_range_count": len(bundle.native_inline_ranges),
        "inline_debug_chain_count": len(bundle.inline_debug_chains),
        "inline_frame_failure_count": len(bundle.inline_frame_failures),
        "record_kinds": sorted({entry.record_kind for entry in bundle.source_maps}),
        "capability_rows": sorted(row.row_id for row in bundle.capability_rows),
        "optimization_transforms": sorted(claim.transform_kind for claim in bundle.optimization_claims),
        "diagnostics": [diagnostic.to_payload() for diagnostic in result.diagnostics],
    }
