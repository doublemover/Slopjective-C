"""Fail-closed object-model debugger proof validation."""

from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from scripts.objc3c_tooling.paths import ROOT, display_path, resolve_repo_path

SCRIPT_ROOT = ROOT / "scripts"
SCRIPT_ROOT_TEXT = str(SCRIPT_ROOT)
if SCRIPT_ROOT_TEXT not in sys.path:
    sys.path.insert(0, SCRIPT_ROOT_TEXT)

from scripts.objc3c_debug_maps import load_bundle, validate_bundle
from scripts.objc3c_debugger_integration import validate_replay_path

CONTRACT_ID = "objc3c.object_model.debugger_value_inspection_replay.v1"
VALIDATION_CONTRACT_ID = "objc3c.object_model.debugger_value_inspection.validation.v1"
PRODUCTION_PROBE_CONTRACT_ID = "objc3c.object_model.production_artifact_probe.v1"
PRODUCTION_SOURCE_IDENTITY_CONTRACT_ID = "objc3c.object_model.production.source_identity.v1"
PRODUCTION_SOURCE_MAP_PUBLICATION_CONTRACT_ID = (
    "objc3c.object_model.production.source_map_native_line_table.v1"
)
PRODUCTION_NATIVE_DEBUG_INFO_EVIDENCE_CONTRACT_ID = (
    "objc3c.object_model.production.native_debug_info_evidence.v1"
)
STATEMENT_STEP_RESERVATION_CONTRACT_ID = (
    "objc3c.object_model.statement_step_reservation.fail_closed.v1"
)
REQUIRED_STEPPING_BLOCKER = "runtime-debug-trace-statement-stepping-integration"
REQUIRED_SOURCE_TRUTH_KIND = "canonical-frontend-runtime-metadata-manifest"
REQUIRED_NATIVE_LINE_TABLE_MODEL = (
    "canonical-frontend-source-map-native-line-table-publication"
)
REQUIRED_STEPPING_STATUS = "native-line-table-ready-stepping-blocked"
DEFAULT_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "object_model_closure"
    / "debugger_value_inspection_replay_contract.json"
)
DEBUG_ANCHOR_CONTRACT_ID = "objc3c.object_model.debug_anchor_identity_replay.v1"
REQUIRED_IDENTITY_KINDS = frozenset(
    {"class", "category", "protocol", "property", "ivar", "method"}
)
DEBUG_ANCHOR_KIND_BY_IDENTITY = {
    "class": "OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CLASS",
    "category": "OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_CATEGORY",
    "protocol": "OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROTOCOL",
    "property": "OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_PROPERTY",
    "ivar": "OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_IVAR",
    "method": "OBJC3_RUNTIME_REFLECTION_DEBUG_ANCHOR_METHOD",
}
REQUIRED_PRODUCTION_ARTIFACT_KINDS = frozenset(
    {
        "manifest",
        "ir",
        "object",
        "runtime-metadata-binary",
        "source-graph",
        "artifact-inspector",
        "debug-map",
    }
)
ABI_MACRO_PATTERN = re.compile(
    r"^\s*#define\s+(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s+"
    r"(?P<value>[0-9]+)u?\s*$"
)
REQUIRED_DEBUG_ANCHOR_NEGATIVE_CASES = frozenset(
    {"missing-anchor", "stale-generation", "malformed-metadata"}
)
REQUIRED_RUNTIME_PROOF_AXES = frozenset(
    {
        "class-registration",
        "metaclass-registration",
        "superclass-root-checks",
        "interface-method-tables",
        "instance-method-lookup",
        "class-method-lookup",
        "categories",
        "protocols",
        "protocol-conformance",
        "properties",
        "ivars",
        "selector-table",
        "registration-replay",
        "reset-reload-boundaries",
        "imported-runtime-packages",
        "reflection-result-lifetime",
    }
)
REQUIRED_UNSUPPORTED_RUNTIME_BOUNDARIES = frozenset(
    {
        "objective-c2-runtime-compatibility",
        "foreign-runtime-mirroring",
        "swift-cxx-abi-import",
        "dynamic-forwarding",
        "private-testing-snapshots-public-api",
        "malformed-metadata-acceptance",
    }
)
REQUIRED_DEBUG_ANCHOR_SOURCE_FIELDS = frozenset(
    {
        "abi_governance_policy",
        "source_anchor_kind",
        "source_map_record_kind",
        "source_map_anchor_policy",
        "artifact_inspector_compatibility",
    }
)
DEBUG_ANCHOR_SIZE_NEGOTIATION_POLICY = "snapshot_size-zero-means-v1-prefix"
REQUIRED_ROADMAP_ISSUE_LINKS = frozenset({8202, 8208, 8209, 8210, 8211, 8212})
DEBUGGER_UMBRELLA_READINESS_CONTRACT_ID = (
    "objc3c.object_model.debugger_grade_umbrella_readiness.v1"
)
REQUIRED_DEBUGGER_UMBRELLA_ISSUES = frozenset({8202, 8209, 8210, 8211, 8212, 8225})
REQUIRED_DEBUGGER_UMBRELLA_CONTRACTS = {
    "debug_source_map_bundle": "objc3c.debug-source-maps.v1",
    "debug_source_map_negative_cases": "objc3c.debug-source-maps.negative-cases.v1",
    "inline_frame_source_map_contract": "objc3c.debug-source-maps.inline-frame.method-inlining.v1",
    "debugger_replay": "objc3c.debugger-integration.replay.v1",
    "lldb_protocol_contract": "objc3c.lldb-plugin.protocol.v1",
    "typed_keypath_debugger_metadata_contract": "objc3c.typed-keypath.debugger-lowering-metadata.v1",
    "debug_anchor_identity_contract": DEBUG_ANCHOR_CONTRACT_ID,
}
REQUIRED_DEBUGGER_UMBRELLA_ROWS = {
    "runtime.debug-trace.inline-frame-source-map": "bounded-supported",
    "runtime.debug-trace.statement-stepping": "bounded-supported",
    "runtime.debug-trace.lldb-plugin": "bounded-supported",
    "runtime.typed-keypath.debugger-lowering": "bounded-supported",
    "runtime.object-model.full-realization": "reserved",
}
REQUIRED_DEBUGGER_UMBRELLA_NEGATIVE_CASES = frozenset(
    {
        "generated-only-source-maps",
        "stale-native-debug-line-table",
        "lldb-protocol-drift",
        "typed-keypath-fallback-interpretation",
        "object-debug-identity-mismatch",
    }
)


@dataclass(frozen=True)
class Diagnostic:
    code: str
    message: str
    path: str

    def to_payload(self) -> dict[str, str]:
        return {"code": self.code, "message": self.message, "path": self.path}


@dataclass(frozen=True)
class ValidationResult:
    ok: bool
    diagnostics: tuple[Diagnostic, ...]
    contract_path: str
    source_map_bundle_path: str
    debugger_replay_path: str

    def to_payload(self) -> dict[str, object]:
        return {
            "ok": self.ok,
            "contract_id": VALIDATION_CONTRACT_ID,
            "contract_path": self.contract_path,
            "source_map_bundle_path": self.source_map_bundle_path,
            "debugger_replay_path": self.debugger_replay_path,
            "diagnostics": [diagnostic.to_payload() for diagnostic in self.diagnostics],
        }


@dataclass(frozen=True)
class ProductionProbeArtifacts:
    summary: dict[str, Any]
    manifest: dict[str, Any]
    source_graph: dict[str, Any]
    artifact_inspector: dict[str, Any]
    debug_map: dict[str, Any]


def _diag(code: str, message: str, path: str) -> Diagnostic:
    return Diagnostic(code=code, message=message, path=path)


def _object(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _list(value: object) -> list[Any]:
    return value if isinstance(value, list) else []


def _safe_str(value: object) -> str:
    return value if isinstance(value, str) else ""


def _safe_int(value: object) -> int:
    return value if isinstance(value, int) and not isinstance(value, bool) else 0


def _load_json(path: Path | str) -> tuple[Path, dict[str, Any], tuple[Diagnostic, ...]]:
    resolved = resolve_repo_path(path)
    try:
        raw = resolved.read_text(encoding="utf-8")
    except OSError as exc:
        return resolved, {}, (
            _diag("json-read-failed", f"unable to read JSON fixture: {exc}", display_path(resolved)),
        )
    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return resolved, {}, (
            _diag("json-invalid", f"invalid JSON at {exc.lineno}:{exc.colno}: {exc.msg}", display_path(resolved)),
        )
    if not isinstance(payload, dict):
        return resolved, {}, (_diag("schema-type", "fixture must be a JSON object", "$"),)
    return resolved, payload, ()


def _records_by_case_id(records: object) -> dict[str, dict[str, Any]]:
    return {
        _safe_str(record.get("case_id")): record
        for record in (_object(item) for item in _list(records))
        if _safe_str(record.get("case_id"))
    }


def _contract_path(payload: dict[str, Any], key: str, default: Path) -> Path:
    raw = _safe_str(payload.get(key))
    return resolve_repo_path(raw) if raw else default


def _replay_commands(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        _safe_str(command.get("command_id")): command
        for command in (
            _object(item)
            for item in _list(_object(payload.get("lldb_plugin")).get("commands"))
        )
        if _safe_str(command.get("command_id"))
    }


def _replay_value_records(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        _safe_str(record.get("value_kind")): record
        for record in (
            _object(item)
            for item in _list(_object(payload.get("value_inspection")).get("records"))
        )
        if _safe_str(record.get("value_kind"))
    }


def _supported_step_kinds(payload: dict[str, Any]) -> set[str]:
    return {
        _safe_str(record.get("step_kind"))
        for record in (
            _object(item)
            for item in _list(_object(payload.get("stepping_plan")).get("records"))
        )
        if record.get("status") == "supported"
    }


def _debug_anchor_queries(payload: dict[str, Any]) -> dict[tuple[str, str], dict[str, Any]]:
    queries: dict[tuple[str, str], dict[str, Any]] = {}
    for item in _list(payload.get("positive_replay_queries")):
        query = _object(item)
        anchor_kind = _safe_str(query.get("anchor_kind"))
        reflection_entrypoint = _safe_str(query.get("ties_to_reflection_row"))
        if anchor_kind and reflection_entrypoint:
            queries[(anchor_kind, reflection_entrypoint)] = query
    return queries


def _contract_value_records(payload: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        _safe_str(record.get("record_id")): record
        for record in (
            _object(item)
            for item in _list(payload.get("object_model_value_inspection_records"))
        )
        if _safe_str(record.get("record_id"))
    }


def _path_exists_in_repo(raw_path: object) -> bool:
    path_text = _safe_str(raw_path)
    return bool(path_text and resolve_repo_path(path_text).is_file())


def _path_exists(raw_path: object) -> bool:
    path_text = _safe_str(raw_path)
    if not path_text:
        return False
    return resolve_repo_path(path_text).exists()


def _positive_int(value: object) -> bool:
    return isinstance(value, int) and not isinstance(value, bool) and value > 0


def _source_map_record_kind_for_identity(identity_kind: str) -> str:
    if identity_kind in {"class", "category", "protocol"}:
        return "declaration"
    if identity_kind == "property":
        return "property-access"
    if identity_kind == "ivar":
        return "generated-accessor"
    if identity_kind == "method":
        return "method"
    return ""


def _source_map_record_kinds_for_identity(identity_kind: str) -> frozenset[str]:
    if identity_kind == "method":
        return frozenset({"method", "message-send"})
    expected = _source_map_record_kind_for_identity(identity_kind)
    return frozenset({expected}) if expected else frozenset()


def _repo_path_text(path: Path | str) -> str:
    return display_path(resolve_repo_path(path)).replace("\\", "/")


def _read_header_macros(raw_path: object) -> tuple[Path, dict[str, int], str, tuple[Diagnostic, ...]]:
    path_text = _safe_str(raw_path)
    if not path_text:
        return ROOT, {}, "", (
            _diag(
                "reflection-abi-governance-drift",
                "reflection ABI governance must name the public header",
                "reflection_abi_governance.public_header",
            ),
        )
    resolved = resolve_repo_path(path_text)
    try:
        text = resolved.read_text(encoding="utf-8")
    except OSError as exc:
        return resolved, {}, "", (
            _diag(
                "reflection-abi-governance-drift",
                f"unable to read reflection ABI header: {exc}",
                display_path(resolved),
            ),
        )
    macros: dict[str, int] = {}
    for line in text.splitlines():
        match = ABI_MACRO_PATTERN.match(line)
        if match is not None:
            macros[match.group("name")] = int(match.group("value"))
    return resolved, macros, text, ()


def _validate_required_sets(
    payload: dict[str, Any],
    replay_payload: dict[str, Any],
    source_record_kinds: set[str],
    diagnostics: list[Diagnostic],
) -> None:
    missing_identity_kinds = REQUIRED_IDENTITY_KINDS - set(
        _safe_str(item) for item in _list(payload.get("required_runtime_identity_kinds"))
    )
    for identity_kind in sorted(missing_identity_kinds):
        diagnostics.append(
            _diag(
                "identity-kind-missing",
                f"required object-model identity kind is missing: {identity_kind}",
                "required_runtime_identity_kinds",
            )
        )

    for record_kind in _list(payload.get("required_source_map_record_kinds")):
        required = _safe_str(record_kind)
        if required and required not in source_record_kinds:
            diagnostics.append(
                _diag(
                    "source-map-record-kind-missing",
                    f"required source-map record kind is missing: {required}",
                    "required_source_map_record_kinds",
                )
            )

    replay_values = _replay_value_records(replay_payload)
    for value_kind in _list(payload.get("required_debugger_replay_value_kinds")):
        required = _safe_str(value_kind)
        if required and required not in replay_values:
            diagnostics.append(
                _diag(
                    "debugger-value-kind-missing",
                    f"debugger replay lacks required value inspection kind: {required}",
                    "required_debugger_replay_value_kinds",
                )
            )

    supported_steps = _supported_step_kinds(replay_payload)
    for step_kind in _list(payload.get("required_supported_step_kinds")):
        required = _safe_str(step_kind)
        if required and required not in supported_steps:
            diagnostics.append(
                _diag(
                    "stepping-kind-missing",
                    f"debugger replay lacks required supported step kind: {required}",
                    "required_supported_step_kinds",
                )
            )


def _validate_value_record(
    value_record: dict[str, Any],
    *,
    commands: dict[str, dict[str, Any]],
    diagnostics: list[Diagnostic],
    path: str,
) -> None:
    value_kind = _safe_str(value_record.get("value_kind"))
    if not value_kind:
        diagnostics.append(_diag("value-inspection-kind-missing", "value record lacks value_kind", path))
    if value_record.get("replayable") is not True:
        diagnostics.append(
            _diag(
                "value-inspection-not-replayable",
                f"object-model value inspection must be replayable: {value_kind}",
                path,
            )
        )
    command_id = _safe_str(value_record.get("lldb_command_id"))
    command = commands.get(command_id)
    if command is None:
        diagnostics.append(
            _diag(
                "lldb-command-missing",
                f"object-model value inspection references missing command: {command_id}",
                path,
            )
        )
    elif command.get("replayable") is not True:
        diagnostics.append(
            _diag(
                "lldb-command-not-replayable",
                f"object-model value inspection command is not replayable: {command_id}",
                path,
            )
        )
    if not _safe_str(value_record.get("runtime_anchor_id")):
        diagnostics.append(
            _diag(
                "runtime-anchor-missing",
                f"object-model value inspection lacks runtime anchor: {value_kind}",
                path,
            )
        )


def _validate_artifact_links(
    payload: dict[str, Any],
    *,
    source_maps: dict[str, Any],
    debug_maps: dict[str, Any],
    line_rows: dict[str, Any],
    debug_anchor_queries: dict[tuple[str, str], dict[str, Any]],
    contract_value_records: dict[str, dict[str, Any]],
    commands: dict[str, dict[str, Any]],
    diagnostics: list[Diagnostic],
) -> None:
    seen: set[str] = set()
    linked_identity_kinds: set[str] = set()
    links = _list(payload.get("artifact_runtime_reflection_links"))
    if not links:
        diagnostics.append(
            _diag(
                "artifact-link-missing",
                "contract must publish artifact/runtime reflection links",
                "artifact_runtime_reflection_links",
            )
        )
    for index, item in enumerate(links):
        link = _object(item)
        path = f"artifact_runtime_reflection_links[{index}]"
        link_id = _safe_str(link.get("link_id"))
        if not link_id:
            diagnostics.append(_diag("artifact-link-id-missing", "artifact link lacks link_id", path))
        elif link_id in seen:
            diagnostics.append(_diag("artifact-link-duplicate", f"duplicate artifact link: {link_id}", path))
        seen.add(link_id)

        source_map_entry_id = _safe_str(link.get("source_map_entry_id"))
        source_entry = source_maps.get(source_map_entry_id)
        if source_entry is None:
            diagnostics.append(
                _diag(
                    "source-map-entry-missing",
                    f"artifact link references missing source-map entry: {source_map_entry_id}",
                    path,
                )
            )
            continue

        debug_map_entry_id = _safe_str(link.get("debug_map_entry_id"))
        debug_map = debug_maps.get(debug_map_entry_id)
        if debug_map is None:
            diagnostics.append(
                _diag(
                    "debug-map-entry-missing",
                    f"artifact link references missing debug-map entry: {debug_map_entry_id}",
                    path,
                )
            )
        elif debug_map.source_map_entry_id != source_entry.entry_id:
            diagnostics.append(
                _diag(
                    "debug-map-drift",
                    f"debug-map entry drifted from artifact link source map: {debug_map_entry_id}",
                    path,
                )
            )

        line_row_id = _safe_str(link.get("native_line_table_row_id"))
        line_row = line_rows.get(line_row_id)
        if line_row is None or line_row_id not in source_entry.native_line_table_row_ids:
            diagnostics.append(
                _diag(
                    "line-table-row-missing",
                    f"artifact link lacks matching native line-table row: {line_row_id}",
                    path,
                )
            )
        elif (
            line_row.source_map_entry_id != source_entry.entry_id
            or line_row.object_debug_line_anchor != source_entry.object_debug_line_anchor
        ):
            diagnostics.append(
                _diag(
                    "line-table-drift",
                    f"artifact link line-table row drifted from source map: {line_row_id}",
                    path,
                )
            )

        runtime_anchor_id = _safe_str(link.get("runtime_anchor_id"))
        if runtime_anchor_id not in source_entry.runtime_anchor_ids:
            diagnostics.append(
                _diag(
                    "runtime-anchor-missing",
                    f"source-map entry does not carry runtime anchor: {runtime_anchor_id}",
                    path,
                )
            )
        if debug_map is not None and runtime_anchor_id not in debug_map.runtime_anchor_ids:
            diagnostics.append(
                _diag(
                    "runtime-anchor-missing",
                    f"debug-map entry does not carry runtime anchor: {runtime_anchor_id}",
                    path,
                )
            )

        identity_kind = _safe_str(link.get("runtime_identity_kind"))
        if identity_kind not in REQUIRED_IDENTITY_KINDS:
            diagnostics.append(
                _diag(
                    "identity-kind-unknown",
                    f"artifact link uses unknown object-model identity kind: {identity_kind}",
                    path,
                )
            )
        else:
            linked_identity_kinds.add(identity_kind)

        expected_record_kinds = _source_map_record_kinds_for_identity(identity_kind)
        if expected_record_kinds and source_entry.record_kind not in expected_record_kinds:
            diagnostics.append(
                _diag(
                    "object-debug-identity-mismatch",
                    "artifact link runtime identity kind drifted from the source-map record kind",
                    path,
                )
            )
        if debug_map is not None and (
            debug_map.object_debug_line_anchor != source_entry.object_debug_line_anchor
            or debug_map.native_symbol != source_entry.native_symbol
        ):
            diagnostics.append(
                _diag(
                    "object-debug-identity-mismatch",
                    "debug-map native identity drifted from the source-map record",
                    path,
                )
            )

        expected_anchor_kind = DEBUG_ANCHOR_KIND_BY_IDENTITY.get(identity_kind, "")
        reflection_entrypoint = _safe_str(link.get("reflection_entrypoint"))
        if (expected_anchor_kind, reflection_entrypoint) not in debug_anchor_queries:
            diagnostics.append(
                _diag(
                    "debug-anchor-query-missing",
                    "artifact link is not backed by the debug-anchor replay contract",
                    path,
                )
            )

        value_record_id = _safe_str(link.get("value_record_id"))
        value_record = contract_value_records.get(value_record_id)
        if value_record is None:
            diagnostics.append(
                _diag(
                    "value-inspection-link-missing",
                    f"artifact link references missing value inspection record: {value_record_id}",
                    path,
                )
            )
            continue
        _validate_value_record(
            value_record,
            commands=commands,
            diagnostics=diagnostics,
            path=f"object_model_value_inspection_records.{value_record_id}",
        )
        if _safe_str(value_record.get("runtime_anchor_id")) != runtime_anchor_id:
            diagnostics.append(
                _diag(
                    "value-inspection-anchor-drift",
                    f"value inspection runtime anchor drifted from artifact link: {value_record_id}",
                    path,
                )
            )

    for identity_kind in sorted(REQUIRED_IDENTITY_KINDS - linked_identity_kinds):
        diagnostics.append(
            _diag(
                "artifact-link-missing",
                f"artifact/runtime reflection link is missing identity kind: {identity_kind}",
                "artifact_runtime_reflection_links",
            )
        )


def _validate_reflection_abi_governance(
    payload: dict[str, Any],
    debug_anchor_payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    governance = _object(payload.get("reflection_abi_governance"))
    if not governance:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-missing",
                "object-model debugger proof must declare reflection ABI governance",
                "reflection_abi_governance",
            )
        )
        return

    _, macros, header_text, header_diagnostics = _read_header_macros(
        governance.get("public_header")
    )
    diagnostics.extend(header_diagnostics)
    for macro_key, expected_key in (
        ("public_reflection_abi_version_macro", "expected_public_reflection_abi_version"),
        ("debug_anchor_abi_version_macro", "expected_debug_anchor_abi_version"),
        (
            "debug_anchor_min_reader_abi_version_macro",
            "expected_debug_anchor_min_reader_abi_version",
        ),
    ):
        macro_name = _safe_str(governance.get(macro_key))
        expected = _safe_int(governance.get(expected_key))
        actual = macros.get(macro_name)
        if not macro_name or actual != expected or expected <= 0:
            diagnostics.append(
                _diag(
                    "reflection-abi-governance-drift",
                    f"reflection ABI macro drifted: {macro_name or macro_key}",
                    f"reflection_abi_governance.{expected_key}",
                )
            )

    if _safe_int(governance.get("expected_debug_anchor_min_reader_abi_version")) > _safe_int(
        governance.get("expected_debug_anchor_abi_version")
    ):
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor minimum reader ABI cannot exceed the current ABI version",
                "reflection_abi_governance.expected_debug_anchor_min_reader_abi_version",
            )
        )

    snapshot_type = _safe_str(governance.get("snapshot_type"))
    if not snapshot_type or snapshot_type not in header_text:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                f"reflection ABI governance snapshot type is missing from the header: {snapshot_type}",
                "reflection_abi_governance.snapshot_type",
            )
        )

    snapshot_fields = set(_safe_str(item) for item in _list(governance.get("snapshot_fields")))
    for field in sorted(REQUIRED_DEBUG_ANCHOR_SOURCE_FIELDS - snapshot_fields):
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                f"reflection ABI governance must require snapshot field: {field}",
                "reflection_abi_governance.snapshot_fields",
            )
        )
    for field in sorted(snapshot_fields):
        if field and field not in header_text:
            diagnostics.append(
                _diag(
                    "reflection-abi-governance-drift",
                    f"reflection ABI governance field is missing from the header: {field}",
                    "reflection_abi_governance.snapshot_fields",
                )
            )

    runtime_api = _object(debug_anchor_payload.get("runtime_anchor_api"))
    if governance.get("caller_size_negotiation") != DEBUG_ANCHOR_SIZE_NEGOTIATION_POLICY:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor ABI governance must declare caller snapshot-size negotiation",
                "reflection_abi_governance.caller_size_negotiation",
            )
        )
    if runtime_api.get("caller_size_negotiation") != DEBUG_ANCHOR_SIZE_NEGOTIATION_POLICY:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor identity contract must declare caller snapshot-size negotiation",
                "debug_anchor_identity_contract.runtime_anchor_api.caller_size_negotiation",
            )
        )
    if "snapshot_size" not in header_text:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor snapshot must carry caller-provided snapshot_size",
                "reflection_abi_governance.snapshot_type",
            )
        )
    v1_prefix_last_field = _safe_str(governance.get("v1_prefix_last_field"))
    if v1_prefix_last_field != _safe_str(runtime_api.get("v1_prefix_last_field")):
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor v1 prefix boundary drifted between governance contracts",
                "reflection_abi_governance.v1_prefix_last_field",
            )
        )
    if not v1_prefix_last_field or v1_prefix_last_field not in header_text:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor ABI governance must name the v1 prefix boundary field",
                "reflection_abi_governance.v1_prefix_last_field",
            )
        )
    else:
        prefix_index = header_text.index(v1_prefix_last_field)
        for field in sorted(REQUIRED_DEBUG_ANCHOR_SOURCE_FIELDS):
            if field in header_text and header_text.index(field) < prefix_index:
                diagnostics.append(
                    _diag(
                        "reflection-abi-governance-drift",
                        f"debug-anchor v2 field must be appended after the v1 prefix: {field}",
                        "reflection_abi_governance.snapshot_fields",
                    )
                )
    appended_fields = set(_safe_str(item) for item in _list(runtime_api.get("v2_appended_fields")))
    if appended_fields != REQUIRED_DEBUG_ANCHOR_SOURCE_FIELDS:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor identity contract appended fields drifted",
                "debug_anchor_identity_contract.runtime_anchor_api.v2_appended_fields",
            )
        )
    if runtime_api.get("abi_version_macro") != governance.get("debug_anchor_abi_version_macro"):
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor identity contract ABI macro drifted from debugger proof governance",
                "debug_anchor_identity_contract.runtime_anchor_api.abi_version_macro",
            )
        )
    if runtime_api.get("min_reader_abi_version_macro") != governance.get(
        "debug_anchor_min_reader_abi_version_macro"
    ):
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor identity contract minimum reader ABI macro drifted",
                "debug_anchor_identity_contract.runtime_anchor_api.min_reader_abi_version_macro",
            )
        )
    entrypoints = set(_safe_str(item) for item in _list(runtime_api.get("entrypoints")))
    if "objc3_runtime_reflection_debug_anchor_min_reader_abi_version" not in entrypoints:
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "debug-anchor ABI governance must expose the minimum reader version entrypoint",
                "debug_anchor_identity_contract.runtime_anchor_api.entrypoints",
            )
        )
    if not _safe_str(governance.get("versioning_policy")):
        diagnostics.append(
            _diag(
                "reflection-abi-governance-drift",
                "reflection ABI governance must publish a versioning policy",
                "reflection_abi_governance.versioning_policy",
            )
        )


def _validate_debug_anchor_contract(
    payload: dict[str, Any],
    debug_anchor_payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    runtime_api = _object(debug_anchor_payload.get("runtime_anchor_api"))
    status_codes = set(_safe_str(item) for item in _list(runtime_api.get("status_codes")))
    for status in (
        "OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND",
        "OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA",
        "OBJC3_RUNTIME_REFLECTION_STATUS_STALE_ANCHOR",
        "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY",
        "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT",
    ):
        if status not in status_codes:
            diagnostics.append(
                _diag(
                    "debug-anchor-boundary-missing",
                    f"debug-anchor identity contract lacks status boundary: {status}",
                    "debug_anchor_identity_contract.runtime_anchor_api.status_codes",
                )
            )

    runtime_owned_fields = set(
        _safe_str(item) for item in _list(debug_anchor_payload.get("runtime_owned_fields"))
    )
    for field in sorted(REQUIRED_DEBUG_ANCHOR_SOURCE_FIELDS - runtime_owned_fields):
        diagnostics.append(
            _diag(
                "debug-anchor-source-field-missing",
                f"debug-anchor identity contract lacks runtime-owned source field: {field}",
                "debug_anchor_identity_contract.runtime_owned_fields",
            )
        )

    positive_queries = _debug_anchor_queries(debug_anchor_payload)
    for identity_kind, anchor_kind in sorted(DEBUG_ANCHOR_KIND_BY_IDENTITY.items()):
        if not any(key[0] == anchor_kind for key in positive_queries):
            diagnostics.append(
                _diag(
                    "debug-anchor-query-missing",
                    f"debug-anchor replay contract lacks positive query for {identity_kind}",
                    "debug_anchor_identity_contract.positive_replay_queries",
                )
            )

    negative_queries = {
        _safe_str(item.get("case_id")): _object(item)
        for item in (
            _object(item) for item in _list(debug_anchor_payload.get("negative_replay_queries"))
        )
        if _safe_str(item.get("case_id"))
    }
    required_negative_cases = {
        _safe_str(item.get("case_id")): _object(item)
        for item in (
            _object(item) for item in _list(payload.get("required_debug_anchor_negative_cases"))
        )
        if _safe_str(item.get("case_id"))
    }
    for case_id in sorted(REQUIRED_DEBUG_ANCHOR_NEGATIVE_CASES):
        required = required_negative_cases.get(case_id)
        observed = negative_queries.get(case_id)
        if required is None or observed is None:
            diagnostics.append(
                _diag(
                    "debug-anchor-boundary-missing",
                    f"debug-anchor negative boundary is missing: {case_id}",
                    "required_debug_anchor_negative_cases",
                )
            )
            continue
        if _safe_str(required.get("expected_status")) != _safe_str(observed.get("expected_status")):
            diagnostics.append(
                _diag(
                    "debug-anchor-boundary-missing",
                    f"debug-anchor negative boundary status drifted: {case_id}",
                    "required_debug_anchor_negative_cases",
                )
            )
        expected_field = _safe_str(required.get("expected_snapshot_field"))
        if expected_field and expected_field != _safe_str(observed.get("expected_snapshot_field")):
            diagnostics.append(
                _diag(
                    "debug-anchor-boundary-missing",
                    f"debug-anchor negative boundary field drifted: {case_id}",
                    "required_debug_anchor_negative_cases",
                )
            )

    boundaries = _object(debug_anchor_payload.get("boundaries"))
    for key in (
        "does_not_promote_umbrella",
        "does_not_expose_private_testing_snapshots",
        "does_not_claim_statement_stepping",
        "does_not_claim_lldb_plugin",
        "does_not_create_dynamic_runtime_state",
    ):
        if boundaries.get(key) is not True:
            diagnostics.append(
                _diag(
                    "debug-anchor-boundary-missing",
                    f"debug-anchor boundary must remain true: {key}",
                    f"debug_anchor_identity_contract.boundaries.{key}",
                )
            )


def _validate_source_backed_debug_anchors(
    payload: dict[str, Any],
    *,
    source_maps: dict[str, Any],
    debug_maps: dict[str, Any],
    line_rows: dict[str, Any],
    debug_anchor_queries: dict[tuple[str, str], dict[str, Any]],
    diagnostics: list[Diagnostic],
) -> None:
    artifact_links = {
        (
            _safe_str(link.get("runtime_identity_kind")),
            _safe_str(link.get("runtime_anchor_id")),
        ): link
        for link in (_object(item) for item in _list(payload.get("artifact_runtime_reflection_links")))
    }
    requirements = _list(payload.get("source_backed_debug_anchors"))
    if not requirements:
        diagnostics.append(
            _diag(
                "source-backed-anchor-missing",
                "object-model debugger proof must declare source-backed debug anchors",
                "source_backed_debug_anchors",
            )
        )
        return
    seen_runtime_anchors: set[str] = set()
    seen_identity_kinds: set[str] = set()
    for index, item in enumerate(requirements):
        requirement = _object(item)
        path = f"source_backed_debug_anchors[{index}]"
        identity_kind = _safe_str(requirement.get("runtime_identity_kind"))
        anchor_kind = _safe_str(requirement.get("anchor_kind"))
        runtime_anchor_id = _safe_str(requirement.get("runtime_anchor_id"))
        reflection_entrypoint = _safe_str(requirement.get("reflection_entrypoint"))
        expected_anchor_kind = DEBUG_ANCHOR_KIND_BY_IDENTITY.get(identity_kind, "")
        if identity_kind not in REQUIRED_IDENTITY_KINDS or anchor_kind != expected_anchor_kind:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"source-backed anchor kind drifted for identity kind: {identity_kind}",
                    path,
                )
            )
        if not runtime_anchor_id.startswith("runtime.anchor.object_model."):
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"source-backed anchor must use object-model runtime anchor id: {runtime_anchor_id}",
                    path,
                )
            )
        if runtime_anchor_id in seen_runtime_anchors:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"source-backed anchor runtime id is not unique: {runtime_anchor_id}",
                    path,
                )
            )
        seen_runtime_anchors.add(runtime_anchor_id)
        seen_identity_kinds.add(identity_kind)

        source_entry = source_maps.get(_safe_str(requirement.get("source_map_entry_id")))
        debug_map = debug_maps.get(_safe_str(requirement.get("debug_map_entry_id")))
        line_row = line_rows.get(_safe_str(requirement.get("native_line_table_row_id")))
        required_record_kind = _safe_str(requirement.get("required_source_map_record_kind"))
        if source_entry is None or source_entry.record_kind != required_record_kind:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"source-backed anchor lacks required source-map record kind: {identity_kind}",
                    path,
                )
            )
            continue
        if runtime_anchor_id not in source_entry.runtime_anchor_ids:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"source-map entry does not carry source-backed runtime anchor: {runtime_anchor_id}",
                    path,
                )
            )
        if debug_map is None or debug_map.source_map_entry_id != source_entry.entry_id:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"debug map does not carry source-backed entry: {identity_kind}",
                    path,
                )
            )
        elif runtime_anchor_id not in debug_map.runtime_anchor_ids:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"debug map does not carry source-backed runtime anchor: {runtime_anchor_id}",
                    path,
                )
            )
        if line_row is None or line_row.source_map_entry_id != source_entry.entry_id:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"native line table row does not carry source-backed entry: {identity_kind}",
                    path,
                )
            )
        if (anchor_kind, reflection_entrypoint) not in debug_anchor_queries:
            diagnostics.append(
                _diag(
                    "debug-anchor-query-missing",
                    f"source-backed anchor lacks matching debug-anchor replay query: {identity_kind}",
                    path,
                )
            )
        link = artifact_links.get((identity_kind, runtime_anchor_id))
        if link is None:
            diagnostics.append(
                _diag(
                    "source-backed-anchor-drift",
                    f"source-backed anchor lacks artifact/runtime reflection link: {identity_kind}",
                    path,
                )
            )
            continue
        for key in (
            "source_map_entry_id",
            "debug_map_entry_id",
            "native_line_table_row_id",
            "reflection_entrypoint",
        ):
            if _safe_str(link.get(key)) != _safe_str(requirement.get(key)):
                diagnostics.append(
                    _diag(
                        "source-backed-anchor-drift",
                        f"source-backed artifact link drifted for {identity_kind}: {key}",
                        path,
                    )
                )

    for identity_kind in sorted(REQUIRED_IDENTITY_KINDS - seen_identity_kinds):
        diagnostics.append(
            _diag(
                "source-backed-anchor-missing",
                f"source-backed debug anchor is missing identity kind: {identity_kind}",
                "source_backed_debug_anchors",
            )
        )


def _validate_runtime_proof_axes(payload: dict[str, Any], diagnostics: list[Diagnostic]) -> None:
    axes = {
        _safe_str(axis.get("axis_id")): axis
        for axis in (
            _object(item) for item in _list(payload.get("runtime_proof_axes"))
        )
        if _safe_str(axis.get("axis_id"))
    }
    if not axes:
        diagnostics.append(
            _diag(
                "runtime-proof-axis-missing",
                "object-model debugger proof must declare runtime proof axes",
                "runtime_proof_axes",
            )
        )
        return

    for axis_id in sorted(REQUIRED_RUNTIME_PROOF_AXES):
        axis = axes.get(axis_id)
        if axis is None:
            diagnostics.append(
                _diag(
                    "runtime-proof-axis-missing",
                    f"runtime proof axis is missing: {axis_id}",
                    "runtime_proof_axes",
                )
            )
            continue
        if axis.get("status") != "bounded-supported":
            diagnostics.append(
                _diag(
                    "runtime-proof-axis-not-supported",
                    f"runtime proof axis must be bounded-supported: {axis_id}",
                    f"runtime_proof_axes.{axis_id}.status",
                )
            )
        if axis.get("support_claim_published") is not False:
            diagnostics.append(
                _diag(
                    "runtime-proof-axis-overclaimed",
                    f"runtime proof axis must not publish the umbrella support claim: {axis_id}",
                    f"runtime_proof_axes.{axis_id}.support_claim_published",
                )
            )
        evidence = _list(axis.get("evidence"))
        if not evidence:
            diagnostics.append(
                _diag(
                    "runtime-proof-axis-evidence-missing",
                    f"runtime proof axis lacks evidence paths: {axis_id}",
                    f"runtime_proof_axes.{axis_id}.evidence",
                )
            )
            continue
        for index, evidence_path in enumerate(evidence):
            if not _path_exists(evidence_path):
                diagnostics.append(
                    _diag(
                        "runtime-proof-axis-evidence-missing",
                        f"runtime proof axis evidence path is missing: {axis_id}",
                        f"runtime_proof_axes.{axis_id}.evidence[{index}]",
                    )
                )


def _validate_unsupported_runtime_boundaries(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    boundaries = {
        _safe_str(boundary.get("boundary_id")): boundary
        for boundary in (
            _object(item) for item in _list(payload.get("unsupported_runtime_boundaries"))
        )
        if _safe_str(boundary.get("boundary_id"))
    }
    if not boundaries:
        diagnostics.append(
            _diag(
                "unsupported-runtime-boundary-missing",
                "object-model debugger proof must declare unsupported runtime boundaries",
                "unsupported_runtime_boundaries",
            )
        )
        return

    for boundary_id in sorted(REQUIRED_UNSUPPORTED_RUNTIME_BOUNDARIES):
        boundary = boundaries.get(boundary_id)
        if boundary is None:
            diagnostics.append(
                _diag(
                    "unsupported-runtime-boundary-missing",
                    f"unsupported runtime boundary is missing: {boundary_id}",
                    "unsupported_runtime_boundaries",
                )
            )
            continue
        if boundary.get("fail_closed") is not True:
            diagnostics.append(
                _diag(
                    "unsupported-runtime-boundary-open",
                    f"unsupported runtime boundary must fail closed: {boundary_id}",
                    f"unsupported_runtime_boundaries.{boundary_id}.fail_closed",
                )
            )
        if boundary.get("public_claim") is not False:
            diagnostics.append(
                _diag(
                    "unsupported-runtime-boundary-overclaimed",
                    f"unsupported runtime boundary must not publish support: {boundary_id}",
                    f"unsupported_runtime_boundaries.{boundary_id}.public_claim",
                )
            )
        evidence = _list(boundary.get("evidence"))
        if not evidence:
            diagnostics.append(
                _diag(
                    "unsupported-runtime-boundary-evidence-missing",
                    f"unsupported runtime boundary lacks evidence paths: {boundary_id}",
                    f"unsupported_runtime_boundaries.{boundary_id}.evidence",
                )
            )
            continue
        for index, evidence_path in enumerate(evidence):
            if not _path_exists(evidence_path):
                diagnostics.append(
                    _diag(
                        "unsupported-runtime-boundary-evidence-missing",
                        f"unsupported runtime boundary evidence path is missing: {boundary_id}",
                        f"unsupported_runtime_boundaries.{boundary_id}.evidence[{index}]",
                    )
                )


def _validate_boundaries(payload: dict[str, Any], diagnostics: list[Diagnostic]) -> None:
    if payload.get("support_claim_published") is not False:
        diagnostics.append(
            _diag(
                "support-claim-overclaimed",
                "object-model debugger proof must not publish the reserved umbrella support claim",
                "support_claim_published",
            )
        )
    _validate_runtime_proof_axes(payload, diagnostics)
    _validate_unsupported_runtime_boundaries(payload, diagnostics)
    boundaries = _object(payload.get("boundaries"))
    required_false = (
        "uses_private_testing_snapshots_as_public_truth",
        "promotes_umbrella",
        "creates_objective_c2_compatibility_claim",
    )
    for key in required_false:
        if boundaries.get(key) is not False:
            diagnostics.append(_diag("boundary-overclaimed", f"boundary must remain false: {key}", f"boundaries.{key}"))
    if boundaries.get("requires_checked_source_map_and_line_table") is not True:
        diagnostics.append(
            _diag(
                "boundary-missing",
                "contract must require checked source-map and native line-table evidence",
                "boundaries.requires_checked_source_map_and_line_table",
            )
        )
    if boundaries.get("requires_debugger_grade_umbrella_readiness") is not True:
        diagnostics.append(
            _diag(
                "boundary-missing",
                "contract must require debugger-grade umbrella readiness glue",
                "boundaries.requires_debugger_grade_umbrella_readiness",
            )
        )


def _validate_debugger_umbrella_negative_source(
    source: dict[str, Any],
    *,
    contract_payloads: dict[str, dict[str, Any]],
    current_payload: dict[str, Any],
    diagnostics: list[Diagnostic],
    path: str,
) -> None:
    contract_key = _safe_str(source.get("contract_key"))
    case_id = _safe_str(source.get("case_id"))
    expected_code = _safe_str(source.get("diagnostic_code"))
    if not contract_key or not case_id:
        diagnostics.append(
            _diag(
                "debugger-umbrella-negative-case-missing",
                "umbrella negative proof source must name a contract key and case id",
                path,
            )
        )
        return

    payload = current_payload if contract_key == "object_model_debugger_contract" else contract_payloads.get(contract_key, {})
    if not payload:
        diagnostics.append(
            _diag(
                "debugger-umbrella-contract-missing",
                f"umbrella negative proof references missing contract: {contract_key}",
                path,
            )
        )
        return

    if contract_key == "debug_source_map_negative_cases":
        case = _records_by_case_id(payload.get("cases")).get(case_id)
        if case is None:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-missing",
                    f"debug source-map negative case is missing: {case_id}",
                    path,
                )
            )
        elif expected_code and _safe_str(case.get("expected_code")) != expected_code:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-drift",
                    f"debug source-map negative diagnostic drifted: {case_id}",
                    path,
                )
            )
        return

    if contract_key == "inline_frame_source_map_contract":
        case = _records_by_case_id(payload.get("fail_closed_diagnostics")).get(case_id)
        if case is None:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-missing",
                    f"inline-frame source-map negative case is missing: {case_id}",
                    path,
                )
            )
        elif expected_code and _safe_str(case.get("diagnostic_code")) != expected_code:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-drift",
                    f"inline-frame source-map negative diagnostic drifted: {case_id}",
                    path,
                )
            )
        return

    if contract_key == "lldb_protocol_contract":
        case = _records_by_case_id(payload.get("rejects")).get(case_id)
        if case is None:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-missing",
                    f"LLDB protocol reject case is missing: {case_id}",
                    path,
                )
            )
        elif expected_code and _safe_str(case.get("diagnostic_code")) != expected_code:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-drift",
                    f"LLDB protocol reject diagnostic drifted: {case_id}",
                    path,
                )
            )
        return

    if contract_key == "typed_keypath_debugger_metadata_contract":
        invariant = _safe_str(source.get("invariant"))
        invariants = set(_safe_str(item) for item in _list(payload.get("fail_closed_invariants")))
        if not invariant or invariant not in invariants:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-missing",
                    f"typed keypath fail-closed invariant is missing: {case_id}",
                    path,
                )
            )
        policy_field = _safe_str(source.get("policy_field"))
        expected_policy_value = source.get("expected_policy_value")
        policy = _object(payload.get("debugger_evidence_policy"))
        if policy_field and policy.get(policy_field) != expected_policy_value:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-typed-keypath-fallback-enabled",
                    f"typed keypath debugger policy drifted: {policy_field}",
                    path,
                )
            )
        return

    if contract_key == "object_model_debugger_contract":
        case = _records_by_case_id(current_payload.get("negative_cases")).get(case_id)
        if case is None:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-missing",
                    f"object-model debugger negative case is missing: {case_id}",
                    path,
                )
            )
        elif expected_code and _safe_str(case.get("expected_code")) != expected_code:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-drift",
                    f"object-model debugger negative diagnostic drifted: {case_id}",
                    path,
                )
            )
        return

    diagnostics.append(
        _diag(
            "debugger-umbrella-contract-missing",
            f"umbrella negative proof uses unknown contract key: {contract_key}",
            path,
        )
    )


def _validate_debugger_umbrella_readiness(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    readiness = _object(payload.get("debugger_grade_umbrella_readiness"))
    if not readiness:
        diagnostics.append(
            _diag(
                "debugger-umbrella-readiness-missing",
                "object-model debugger proof must publish debugger-grade umbrella readiness glue",
                "debugger_grade_umbrella_readiness",
            )
        )
        return

    if readiness.get("contract_id") != DEBUGGER_UMBRELLA_READINESS_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "debugger-umbrella-contract-id",
                f"debugger umbrella readiness contract_id must be {DEBUGGER_UMBRELLA_READINESS_CONTRACT_ID}",
                "debugger_grade_umbrella_readiness.contract_id",
            )
        )
    issue_refs = {_safe_int(item) for item in _list(readiness.get("issue_refs")) if _safe_int(item)}
    for issue_id in sorted(REQUIRED_DEBUGGER_UMBRELLA_ISSUES - issue_refs):
        diagnostics.append(
            _diag(
                "debugger-umbrella-issue-link-missing",
                f"debugger umbrella readiness must link roadmap issue {issue_id}",
                "debugger_grade_umbrella_readiness.issue_refs",
            )
        )
    if readiness.get("capability_id") != "runtime.object-model.full-realization":
        diagnostics.append(
            _diag(
                "debugger-umbrella-capability-id",
                "debugger umbrella readiness must remain tied to runtime.object-model.full-realization",
                "debugger_grade_umbrella_readiness.capability_id",
            )
        )
    if readiness.get("public_status") != "reserved" or readiness.get("support_claim_published") is not False:
        diagnostics.append(
            _diag(
                "debugger-umbrella-overclaimed",
                "debugger umbrella readiness must not promote the reserved object-model umbrella",
                "debugger_grade_umbrella_readiness.public_status",
            )
        )

    boundary = _object(readiness.get("umbrella_boundary"))
    for key, expected in (
        ("promotes_umbrella", False),
        ("generated_only_maps_can_promote", False),
        ("typed_keypath_fallback_allowed", False),
        ("lldb_protocol_drift_allowed", False),
        ("object_debug_identity_mismatch_allowed", False),
    ):
        if boundary.get(key) is not expected:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-overclaimed",
                    f"debugger umbrella boundary drifted: {key}",
                    f"debugger_grade_umbrella_readiness.umbrella_boundary.{key}",
                )
            )

    contract_payloads: dict[str, dict[str, Any]] = {}
    required_contracts = _object(readiness.get("required_contracts"))
    for contract_key, expected_contract_id in sorted(REQUIRED_DEBUGGER_UMBRELLA_CONTRACTS.items()):
        contract = _object(required_contracts.get(contract_key))
        path = _safe_str(contract.get("path"))
        if contract.get("contract_id") != expected_contract_id:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-contract-drift",
                    f"debugger umbrella contract id drifted: {contract_key}",
                    f"debugger_grade_umbrella_readiness.required_contracts.{contract_key}.contract_id",
                )
            )
        if not path or path.startswith(("tmp/", "tmp\\")):
            diagnostics.append(
                _diag(
                    "debugger-umbrella-contract-missing",
                    f"debugger umbrella contract path is missing or temp-only: {contract_key}",
                    f"debugger_grade_umbrella_readiness.required_contracts.{contract_key}.path",
                )
            )
            continue
        _, contract_payload, load_diagnostics = _load_json(path)
        diagnostics.extend(
            _diag(
                "debugger-umbrella-contract-missing",
                diagnostic.message,
                f"debugger_grade_umbrella_readiness.required_contracts.{contract_key}.path",
            )
            for diagnostic in load_diagnostics
        )
        if contract_payload:
            contract_payloads[contract_key] = contract_payload
            if contract_payload.get("contract_id") != expected_contract_id:
                diagnostics.append(
                    _diag(
                        "debugger-umbrella-contract-drift",
                        f"debugger umbrella referenced contract payload drifted: {contract_key}",
                        f"debugger_grade_umbrella_readiness.required_contracts.{contract_key}.path",
                    )
                )

    rows = {
        _safe_str(row.get("capability_id")): row
        for row in (_object(item) for item in _list(readiness.get("bounded_capability_rows")))
        if _safe_str(row.get("capability_id"))
    }
    for capability_id, expected_status in sorted(REQUIRED_DEBUGGER_UMBRELLA_ROWS.items()):
        row = rows.get(capability_id)
        if row is None:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-row-missing",
                    f"debugger umbrella bounded capability row is missing: {capability_id}",
                    "debugger_grade_umbrella_readiness.bounded_capability_rows",
                )
            )
            continue
        if row.get("recommended_status") != expected_status:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-row-drift",
                    f"debugger umbrella bounded capability row status drifted: {capability_id}",
                    f"debugger_grade_umbrella_readiness.bounded_capability_rows.{capability_id}.recommended_status",
                )
            )
        if row.get("support_claim_published") is not False:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-overclaimed",
                    f"debugger umbrella bounded row must not publish support directly: {capability_id}",
                    f"debugger_grade_umbrella_readiness.bounded_capability_rows.{capability_id}.support_claim_published",
                )
            )

    typed_policy = _object(readiness.get("typed_keypath_policy"))
    if typed_policy.get("fallback_interpretation_allowed") is not False:
        diagnostics.append(
            _diag(
                "debugger-umbrella-typed-keypath-fallback-enabled",
                "typed keypath umbrella readiness must reject fallback interpretation",
                "debugger_grade_umbrella_readiness.typed_keypath_policy.fallback_interpretation_allowed",
            )
        )
    typed_payload = contract_payloads.get("typed_keypath_debugger_metadata_contract", {})
    typed_required_fields = set(
        _safe_str(item) for item in _list(typed_payload.get("required_descriptor_fields"))
    )
    for field in _list(typed_policy.get("required_descriptor_fields")):
        required_field = _safe_str(field)
        if required_field and required_field not in typed_required_fields:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-typed-keypath-field-missing",
                    f"typed keypath descriptor field is missing from the source contract: {required_field}",
                    "debugger_grade_umbrella_readiness.typed_keypath_policy.required_descriptor_fields",
                )
            )

    negative_cases = _records_by_case_id(readiness.get("fail_closed_negative_cases"))
    for case_id in sorted(REQUIRED_DEBUGGER_UMBRELLA_NEGATIVE_CASES - set(negative_cases)):
        diagnostics.append(
            _diag(
                "debugger-umbrella-negative-case-missing",
                f"debugger umbrella fail-closed negative case is missing: {case_id}",
                "debugger_grade_umbrella_readiness.fail_closed_negative_cases",
            )
        )
    for index, item in enumerate(_list(readiness.get("fail_closed_negative_cases"))):
        case = _object(item)
        path = f"debugger_grade_umbrella_readiness.fail_closed_negative_cases[{index}]"
        case_id = _safe_str(case.get("case_id"))
        case_issues = {_safe_int(issue) for issue in _list(case.get("issue_refs")) if _safe_int(issue)}
        if case_id and {8202, 8212} - case_issues:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-issue-link-missing",
                    f"debugger umbrella negative case must stay linked to #8202/#8212: {case_id}",
                    path,
                )
            )
        sources = _list(case.get("sources"))
        if not sources:
            diagnostics.append(
                _diag(
                    "debugger-umbrella-negative-case-missing",
                    f"debugger umbrella negative case lacks source-backed proofs: {case_id}",
                    path,
                )
            )
            continue
        for source_index, source_item in enumerate(sources):
            _validate_debugger_umbrella_negative_source(
                _object(source_item),
                contract_payloads=contract_payloads,
                current_payload=payload,
                diagnostics=diagnostics,
                path=f"{path}.sources[{source_index}]",
            )


def _build_production_probe_artifacts(probe: dict[str, Any]) -> ProductionProbeArtifacts:
    from objc3c_editor_tooling.input_loading import load_editor_tooling_inputs, run_frontend_compile
    from objc3c_editor_tooling.model import build_editor_tooling_model
    from objc3c_editor_tooling.paths import paths_for_source, resolve_source
    from objc3c_editor_tooling.publication import publish_editor_tooling_surface

    source_fixture = _safe_str(probe.get("source_fixture"))
    paths = paths_for_source(resolve_source(source_fixture))
    compile_result = run_frontend_compile(paths)
    if not compile_result.summary_available:
        raise RuntimeError(
            "frontend production probe did not publish compile summary "
            f"(exit={compile_result.returncode})"
        )
    inputs = load_editor_tooling_inputs(paths)
    model = build_editor_tooling_model(paths, inputs)
    publish_editor_tooling_surface(paths=paths, inputs=inputs, model=model)
    return ProductionProbeArtifacts(
        summary=inputs.summary,
        manifest=inputs.manifest,
        source_graph=model.source_graph,
        artifact_inspector=model.artifact_inspector,
        debug_map=model.debug,
    )


def _validate_production_probe_contract(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> dict[str, Any]:
    probe = _object(payload.get("production_artifact_probe"))
    if not probe:
        diagnostics.append(
            _diag(
                "production-probe-missing",
                "object-model debugger proof must declare a production artifact probe",
                "production_artifact_probe",
            )
        )
        return {}

    if probe.get("contract_id") != PRODUCTION_PROBE_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "production-probe-contract-id",
                f"production artifact probe contract_id must be {PRODUCTION_PROBE_CONTRACT_ID}",
                "production_artifact_probe.contract_id",
            )
        )
    source_fixture = _safe_str(probe.get("source_fixture"))
    if not source_fixture:
        diagnostics.append(
            _diag(
                "production-source-fixture-missing",
                "production artifact probe must name the integrated object-model source fixture",
                "production_artifact_probe.source_fixture",
            )
        )
    elif source_fixture.startswith(("tmp/", "tmp\\")) or not _path_exists_in_repo(source_fixture):
        diagnostics.append(
            _diag(
                "production-source-fixture-invalid",
                "production artifact probe source fixture must be checked in and outside tmp",
                "production_artifact_probe.source_fixture",
            )
        )

    public_command = _safe_str(probe.get("public_command"))
    if "validate-object-model-debugger-proof" not in public_command:
        diagnostics.append(
            _diag(
                "production-public-command-missing",
                "production artifact probe must be replayable through validate-object-model-debugger-proof",
                "production_artifact_probe.public_command",
            )
        )
    if probe.get("runs_canonical_frontend") is not True:
        diagnostics.append(
            _diag(
                "production-frontend-not-required",
                "production artifact probe must require the canonical frontend path",
                "production_artifact_probe.runs_canonical_frontend",
            )
        )
    artifact_kinds = set(_safe_str(item) for item in _list(probe.get("required_artifact_kinds")))
    for artifact_kind in sorted(REQUIRED_PRODUCTION_ARTIFACT_KINDS - artifact_kinds):
        diagnostics.append(
            _diag(
                "production-artifact-kind-missing",
                f"production artifact probe must require artifact kind: {artifact_kind}",
                "production_artifact_probe.required_artifact_kinds",
            )
        )
    for minimums_key in (
        "runtime_inventory_minimums",
        "object_model_source_identity_minimums",
        "source_map_native_line_table_minimums",
    ):
        if not _object(probe.get(minimums_key)):
            diagnostics.append(
                _diag(
                    "production-probe-minimums-missing",
                    f"production artifact probe must declare {minimums_key}",
                    f"production_artifact_probe.{minimums_key}",
                )
            )
    if _object(probe.get("debug_map_boundary")).get("full_source_map_publication") != "fail-closed":
        diagnostics.append(
            _diag(
                "production-debug-map-boundary-missing",
                "production artifact probe must keep full source-map publication fail-closed",
                "production_artifact_probe.debug_map_boundary.full_source_map_publication",
            )
        )
    if _object(probe.get("debug_map_boundary")).get("statement_stepping") != "fail-closed":
        diagnostics.append(
            _diag(
                "production-debug-map-boundary-missing",
                "production artifact probe must keep statement stepping fail-closed",
                "production_artifact_probe.debug_map_boundary.statement_stepping",
            )
        )
    if _object(probe.get("debug_map_boundary")).get("native_debug_info_evidence") != "required":
        diagnostics.append(
            _diag(
                "production-debug-map-boundary-missing",
                "production artifact probe must require emitted object native debug-info evidence",
                "production_artifact_probe.debug_map_boundary.native_debug_info_evidence",
            )
        )
    return probe


def _validate_artifact_inspector_compatibility_contract(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> dict[str, Any]:
    compatibility = _object(payload.get("artifact_inspector_compatibility"))
    if not compatibility:
        diagnostics.append(
            _diag(
                "artifact-inspector-compatibility-contract",
                "object-model debugger proof must declare artifact-inspector compatibility",
                "artifact_inspector_compatibility",
            )
        )
        return {}
    expected = {
        "contract_id": "objc3c.object_model.debugger_artifact_inspector.compatibility.v1",
        "required_contract_id": "objc3c.developer.tooling.artifact.inspector.v1",
        "required_support_class": "compile-artifact-inspector",
        "required_runtime_inventory_reflection_abi_version": "manifest-derived-runtime-metadata",
        "object_model_source_map_native_line_table": "required",
        "full_source_map_publication": "fail-closed",
    }
    for key, expected_value in expected.items():
        if compatibility.get(key) != expected_value:
            diagnostics.append(
                _diag(
                    "artifact-inspector-compatibility-contract",
                    f"artifact-inspector compatibility drifted: {key}",
                    f"artifact_inspector_compatibility.{key}",
                )
            )
    if compatibility.get("requires_runtime_inventory") is not True:
        diagnostics.append(
            _diag(
                "artifact-inspector-compatibility-contract",
                "artifact-inspector compatibility must require runtime inventory",
                "artifact_inspector_compatibility.requires_runtime_inventory",
            )
        )
    if compatibility.get("requires_debug_map_link") is not True:
        diagnostics.append(
            _diag(
                "artifact-inspector-compatibility-contract",
                "artifact-inspector compatibility must require a debug-map link",
                "artifact_inspector_compatibility.requires_debug_map_link",
            )
        )
    return compatibility


def _validate_statement_step_reservation_contract(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> dict[str, Any]:
    contract = _object(payload.get("statement_step_reservation_contract"))
    path = "statement_step_reservation_contract"
    if not contract:
        diagnostics.append(
            _diag(
                "statement-step-reservation-contract-missing",
                "object-model debugger proof must declare the reserved statement-step contract",
                path,
            )
        )
        return {}
    expected = {
        "contract_id": STATEMENT_STEP_RESERVATION_CONTRACT_ID,
        "status": "reserved",
        "required_candidate_status": REQUIRED_STEPPING_STATUS,
    }
    for key, expected_value in expected.items():
        if contract.get(key) != expected_value:
            diagnostics.append(
                _diag(
                    "statement-step-reservation-contract-drift",
                    f"statement-step reservation contract drifted: {key}",
                    f"{path}.{key}",
                )
            )
    if contract.get("fail_closed") is not True:
        diagnostics.append(
            _diag(
                "statement-step-reservation-contract-drift",
                "statement-step reservation contract must fail closed",
                f"{path}.fail_closed",
            )
        )
    if contract.get("requires_exact_source_native_line_anchor") is not True:
        diagnostics.append(
            _diag(
                "statement-step-reservation-contract-drift",
                "statement-step reservation must require exact source/native line anchors",
                f"{path}.requires_exact_source_native_line_anchor",
            )
        )
    if contract.get("requires_source_map_row_identity_alignment") is not True:
        diagnostics.append(
            _diag(
                "statement-step-reservation-contract-drift",
                "statement-step reservation must require source-map/native-row identity alignment",
                f"{path}.requires_source_map_row_identity_alignment",
            )
        )
    if contract.get("requires_emitted_native_debug_info") is not True:
        diagnostics.append(
            _diag(
                "statement-step-reservation-contract-drift",
                "statement-step reservation must require emitted native debug-info evidence",
                f"{path}.requires_emitted_native_debug_info",
            )
        )
    blockers = {
        _safe_str(item)
        for item in _list(contract.get("blocked_by"))
        if _safe_str(item)
    }
    if REQUIRED_STEPPING_BLOCKER not in blockers:
        diagnostics.append(
            _diag(
                "statement-step-reservation-contract-drift",
                "statement-step reservation must keep runtime trace integration as the blocker",
                f"{path}.blocked_by",
            )
        )
    return contract


def _validate_count_at_least(
    actual: object,
    minimum: object,
    diagnostics: list[Diagnostic],
    *,
    code: str,
    message: str,
    path: str,
) -> None:
    if not isinstance(actual, int) or isinstance(actual, bool):
        actual = 0
    if not isinstance(minimum, int) or isinstance(minimum, bool):
        minimum = 0
    if actual < minimum:
        diagnostics.append(_diag(code, f"{message}: expected >= {minimum}, got {actual}", path))


def _validate_native_debug_info_evidence(
    evidence: dict[str, Any],
    diagnostics: list[Diagnostic],
    *,
    path: str,
) -> None:
    if not evidence:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-missing",
                "production source-map/native-line-table publication must carry native debug-info artifact evidence",
                path,
            )
        )
        return
    if evidence.get("contract_id") != PRODUCTION_NATIVE_DEBUG_INFO_EVIDENCE_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-contract-id",
                "production native debug-info evidence contract id drifted",
                f"{path}.contract_id",
            )
        )
    if not _safe_str(evidence.get("evidence_id")):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must publish an evidence id",
                f"{path}.evidence_id",
            )
        )
    if evidence.get("object_artifact_present") is not True:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must inspect the emitted object artifact",
                f"{path}.object_artifact_present",
            )
        )
    if not _safe_str(evidence.get("object_path")) or not _safe_str(evidence.get("object_sha256")):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must publish object path and digest",
                path,
            )
        )
    if not _list(evidence.get("object_section_names")):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must include emitted object section names",
                f"{path}.object_section_names",
            )
        )
    if evidence.get("emitted_native_debug_info_supported") is not True:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-unavailable",
                "production native debug-info evidence must confirm emitted native debug info",
                f"{path}.emitted_native_debug_info_supported",
            )
        )
    if evidence.get("native_line_table_supported") is not True:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-unavailable",
                "production native debug-info evidence must confirm emitted native line-table sections",
                f"{path}.native_line_table_supported",
            )
        )
    if not _list(evidence.get("native_debug_sections")):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must list emitted native debug sections",
                f"{path}.native_debug_sections",
            )
        )
    if not _list(evidence.get("native_line_table_sections")):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must list emitted native line-table sections",
                f"{path}.native_line_table_sections",
            )
        )
    _validate_count_at_least(
        evidence.get("native_debug_section_count"),
        1,
        diagnostics,
        code="production-native-debug-info-evidence-incomplete",
        message="production native debug-info evidence lacks native debug sections",
        path=f"{path}.native_debug_section_count",
    )
    _validate_count_at_least(
        evidence.get("native_line_table_section_count"),
        1,
        diagnostics,
        code="production-native-debug-info-evidence-incomplete",
        message="production native debug-info evidence lacks native line-table sections",
        path=f"{path}.native_line_table_section_count",
    )
    if evidence.get("llvm_debug_metadata_present") is not True:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must confirm instruction-level LLVM DI locations",
                f"{path}.llvm_debug_metadata_present",
            )
        )
    _validate_count_at_least(
        evidence.get("llvm_debug_location_count"),
        1,
        diagnostics,
        code="production-native-debug-info-evidence-incomplete",
        message="production native debug-info evidence lacks instruction-level LLVM debug locations",
        path=f"{path}.llvm_debug_location_count",
    )
    if evidence.get("statement_stepping_supported") is not False:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-overclaimed",
                "production native debug-info evidence must not claim statement stepping",
                f"{path}.statement_stepping_supported",
            )
        )
    if evidence.get("fail_closed") is not True:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-overclaimed",
                "production native debug-info evidence must keep statement stepping fail-closed",
                f"{path}.fail_closed",
            )
        )
    blocked_by = set(_safe_str(item) for item in _list(evidence.get("blocked_by")))
    if REQUIRED_STEPPING_BLOCKER not in blocked_by:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must keep runtime statement-stepping integration blocked",
                f"{path}.blocked_by",
            )
        )
    retired_blockers = {
        "native-object-lacks-debug-info-section",
        "native-object-lacks-debug-line-section",
        "compiler-ir-lacks-llvm-di-locations",
    }
    if blocked_by & retired_blockers:
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-stale-blocker",
                "production native debug-info evidence must clear missing-section and missing-DI blockers once native debug metadata is emitted",
                f"{path}.blocked_by",
            )
        )
    if not _safe_str(evidence.get("fail_closed_reason")):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-incomplete",
                "production native debug-info evidence must carry a fail-closed reason",
                f"{path}.fail_closed_reason",
            )
        )


def _validate_fail_closed_publication_boundaries(
    publication: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    boundaries = {
        _safe_str(_object(boundary).get("capability_id")): _object(boundary)
        for boundary in _list(publication.get("fail_closed_boundaries"))
    }
    emitted_boundary = boundaries.get("emittedNativeDebugInfo", {})
    if publication.get("emitted_native_debug_info_supported") is True:
        if (
            emitted_boundary.get("status") != "supported"
            or emitted_boundary.get("fail_closed") is not False
        ):
            diagnostics.append(
                _diag(
                    "production-source-map-publication-boundary-missing",
                    "production source-map/native-line-table publication must mark emittedNativeDebugInfo supported",
                    "production_artifact_probe.debug_map.object_model_source_identity.source_map_native_line_table_publication.fail_closed_boundaries",
                )
            )
    elif (
        emitted_boundary.get("status") != "reserved"
        or emitted_boundary.get("fail_closed") is not True
    ):
        diagnostics.append(
            _diag(
                "production-source-map-publication-boundary-missing",
                "production source-map/native-line-table publication must keep emittedNativeDebugInfo reserved when native debug info is absent",
                "production_artifact_probe.debug_map.object_model_source_identity.source_map_native_line_table_publication.fail_closed_boundaries",
            )
        )
    boundary = boundaries.get("statementLevelStepping", {})
    if boundary.get("status") != "reserved" or boundary.get("fail_closed") is not True:
        diagnostics.append(
            _diag(
                "production-source-map-publication-boundary-missing",
                "production source-map/native-line-table publication must keep statementLevelStepping reserved",
                "production_artifact_probe.debug_map.object_model_source_identity.source_map_native_line_table_publication.fail_closed_boundaries",
            )
        )


def _validate_production_source_map_publication_payload(
    source_identity: dict[str, Any],
    *,
    records: list[Any],
    rows: list[Any],
    required_kinds: set[str],
    minimums: dict[str, Any],
    expected_source: str,
    diagnostics: list[Diagnostic],
) -> None:
    path = (
        "production_artifact_probe.debug_map.object_model_source_identity"
        ".source_map_native_line_table_publication"
    )
    publication = _object(source_identity.get("source_map_native_line_table_publication"))
    if not publication:
        diagnostics.append(
            _diag(
                "production-source-map-publication-missing",
                "production object-model source identity must publish a source-map/native-line-table surface",
                path,
            )
        )
        return

    if publication.get("contract_id") != PRODUCTION_SOURCE_MAP_PUBLICATION_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "production-source-map-publication-contract-id",
                "production source-map/native-line-table contract id drifted",
                f"{path}.contract_id",
            )
        )
    if publication.get("supported") is not True:
        diagnostics.append(
            _diag(
                "production-source-map-publication-unavailable",
                "production source-map/native-line-table publication must be supported",
                f"{path}.supported",
            )
        )
    if publication.get("publication_model") != "canonical-frontend-manifest-source-map-native-line-table":
        diagnostics.append(
            _diag(
                "production-source-map-publication-model-drift",
                "production source-map/native-line-table publication model drifted",
                f"{path}.publication_model",
            )
        )
    if _safe_str(publication.get("source_path")).replace("\\", "/") != expected_source:
        diagnostics.append(
            _diag(
                "production-source-map-publication-source-drift",
                "production source-map/native-line-table source path drifted",
                f"{path}.source_path",
            )
        )
    if not _safe_str(publication.get("source_graph_digest")):
        diagnostics.append(
            _diag(
                "production-source-map-publication-source-graph-missing",
                "production source-map/native-line-table publication must carry the source graph digest",
                f"{path}.source_graph_digest",
            )
        )

    _validate_native_debug_info_evidence(
        _object(publication.get("native_debug_info_evidence")),
        diagnostics,
        path=f"{path}.native_debug_info_evidence",
    )

    for key in ("source_map_publication_supported", "native_line_table_publication_supported"):
        if publication.get(key) is not True:
            diagnostics.append(
                _diag(
                    "production-source-map-publication-incomplete",
                    f"production source-map/native-line-table publication must set {key}",
                    f"{path}.{key}",
                )
            )
    if publication.get("emitted_native_debug_info_supported") is not True:
        diagnostics.append(
            _diag(
                "production-source-map-publication-incomplete",
                "production source-map/native-line-table publication must claim emitted_native_debug_info_supported only after emitted native sections are proven",
                f"{path}.emitted_native_debug_info_supported",
            )
        )
    if publication.get("statement_stepping_supported") is not False:
        diagnostics.append(
            _diag(
                "production-source-map-publication-overclaimed",
                "production source-map/native-line-table publication must not claim statement_stepping_supported",
                f"{path}.statement_stepping_supported",
            )
        )

    record_ids = {
        _safe_str(_object(record).get("source_map_record_id"))
        for record in records
        if _safe_str(_object(record).get("source_map_record_id"))
    }
    row_ids = {
        _safe_str(_object(row).get("row_id"))
        for row in rows
        if _safe_str(_object(row).get("row_id"))
    }
    published_record_ids = {
        _safe_str(item)
        for item in _list(publication.get("source_map_record_ids"))
        if _safe_str(item)
    }
    published_row_ids = {
        _safe_str(item)
        for item in _list(publication.get("native_line_table_row_ids"))
        if _safe_str(item)
    }
    if publication.get("source_map_record_count") != len(record_ids) or published_record_ids != record_ids:
        diagnostics.append(
            _diag(
                "production-source-map-publication-record-drift",
                "production source-map publication record ids drifted from source identity records",
                f"{path}.source_map_record_ids",
            )
        )
    if publication.get("native_line_table_row_count") != len(row_ids) or published_row_ids != row_ids:
        diagnostics.append(
            _diag(
                "production-source-map-publication-row-drift",
                "production native line-table publication row ids drifted from source identity rows",
                f"{path}.native_line_table_row_ids",
            )
        )
    _validate_count_at_least(
        publication.get("source_map_record_count"),
        minimums.get("source_map_records"),
        diagnostics,
        code="production-source-map-publication-incomplete",
        message="production source-map publication lacks source-map records",
        path=f"{path}.source_map_record_count",
    )
    _validate_count_at_least(
        publication.get("native_line_table_row_count"),
        minimums.get("native_line_table_rows"),
        diagnostics,
        code="production-source-map-publication-incomplete",
        message="production native line-table publication lacks rows",
        path=f"{path}.native_line_table_row_count",
    )

    publication_required_kinds = set(
        _safe_str(item)
        for item in _list(minimums.get("required_identity_kinds"))
        if _safe_str(item)
    ) or required_kinds
    published_kinds = {
        _safe_str(item)
        for item in _list(publication.get("required_identity_kinds_present"))
        if _safe_str(item)
    }
    if publication_required_kinds - published_kinds:
        diagnostics.append(
            _diag(
                "production-source-map-publication-kind-missing",
                "production source-map/native-line-table publication is missing required identity kinds",
                f"{path}.required_identity_kinds_present",
            )
        )
    _validate_fail_closed_publication_boundaries(publication, diagnostics)


def _validate_production_source_identity_payload(
    debug_map: dict[str, Any],
    probe: dict[str, Any],
    step_reservation: dict[str, Any],
    expected_source: str,
    diagnostics: list[Diagnostic],
) -> None:
    source_identity = _object(debug_map.get("object_model_source_identity"))
    if not source_identity:
        diagnostics.append(
            _diag(
                "production-source-identity-missing",
                "production debug map must publish object-model source identity records",
                "production_artifact_probe.debug_map.object_model_source_identity",
            )
        )
        return

    if source_identity.get("contract_id") != PRODUCTION_SOURCE_IDENTITY_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "production-source-identity-contract-id",
                "production object-model source identity contract id drifted",
                "production_artifact_probe.debug_map.object_model_source_identity.contract_id",
            )
        )
    if source_identity.get("supported") is not True:
        diagnostics.append(
            _diag(
                "production-source-identity-unavailable",
                "production object-model source identity must be supported by the canonical manifest",
                "production_artifact_probe.debug_map.object_model_source_identity.supported",
            )
        )
    if source_identity.get("runs_on_canonical_frontend_manifest") is not True:
        diagnostics.append(
            _diag(
                "production-source-identity-not-compiler-owned",
                "production object-model source identity must come from the canonical frontend manifest",
                "production_artifact_probe.debug_map.object_model_source_identity.runs_on_canonical_frontend_manifest",
            )
        )
    if _safe_str(source_identity.get("source_path")).replace("\\", "/") != expected_source:
        diagnostics.append(
            _diag(
                "production-source-identity-source-drift",
                "production object-model source identity source path drifted",
                "production_artifact_probe.debug_map.object_model_source_identity.source_path",
            )
        )

    required_true = (
        "source_map_records_supported",
        "native_line_table_projection_supported",
        "source_map_publication_supported",
        "native_line_table_publication_supported",
        "method_stepping_candidates_supported",
        "native_debug_info_emitted",
    )
    for key in required_true:
        if source_identity.get(key) is not True:
            diagnostics.append(
                _diag(
                    "production-source-identity-incomplete",
                    f"production object-model source identity must publish {key}",
                    f"production_artifact_probe.debug_map.object_model_source_identity.{key}",
                )
            )
    required_false = (
        "full_source_map_publication",
        "runtime_debug_trace_statement_stepping",
    )
    for key in required_false:
        if source_identity.get(key) is not False:
            diagnostics.append(
                _diag(
                    "production-source-identity-overclaimed",
                    f"production object-model source identity must keep reserved boundary false: {key}",
                    f"production_artifact_probe.debug_map.object_model_source_identity.{key}",
                )
            )
    native_debug_info_evidence = _object(source_identity.get("native_debug_info_evidence"))
    _validate_native_debug_info_evidence(
        native_debug_info_evidence,
        diagnostics,
        path="production_artifact_probe.debug_map.object_model_source_identity.native_debug_info_evidence",
    )
    native_debug_info_evidence_id = _safe_str(
        native_debug_info_evidence.get("evidence_id")
    )
    if (
        native_debug_info_evidence_id
        and source_identity.get("native_debug_info_evidence_id") != native_debug_info_evidence_id
    ):
        diagnostics.append(
            _diag(
                "production-native-debug-info-evidence-drift",
                "production source identity native debug-info evidence id drifted",
                "production_artifact_probe.debug_map.object_model_source_identity.native_debug_info_evidence_id",
            )
        )

    minimums = _object(probe.get("object_model_source_identity_minimums"))
    publication_minimums = _object(probe.get("source_map_native_line_table_minimums"))
    records = _list(source_identity.get("source_map_records"))
    rows = _list(source_identity.get("native_line_table_rows"))
    stepping_candidates = _list(source_identity.get("stepping_candidates"))
    _validate_count_at_least(
        source_identity.get("source_map_record_count"),
        minimums.get("source_map_records"),
        diagnostics,
        code="production-source-identity-incomplete",
        message="production source identity lacks source-map records",
        path="production_artifact_probe.debug_map.object_model_source_identity.source_map_record_count",
    )
    _validate_count_at_least(
        len(records),
        minimums.get("source_map_records"),
        diagnostics,
        code="production-source-identity-incomplete",
        message="production source identity lacks concrete source-map records",
        path="production_artifact_probe.debug_map.object_model_source_identity.source_map_records",
    )
    _validate_count_at_least(
        source_identity.get("native_line_table_row_count"),
        minimums.get("native_line_table_rows"),
        diagnostics,
        code="production-source-identity-incomplete",
        message="production source identity lacks native line-table projection rows",
        path="production_artifact_probe.debug_map.object_model_source_identity.native_line_table_row_count",
    )
    _validate_count_at_least(
        len(rows),
        minimums.get("native_line_table_rows"),
        diagnostics,
        code="production-source-identity-incomplete",
        message="production source identity lacks concrete native line-table rows",
        path="production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows",
    )
    _validate_count_at_least(
        source_identity.get("stepping_candidate_count"),
        minimums.get("stepping_candidates"),
        diagnostics,
        code="production-source-identity-incomplete",
        message="production source identity lacks method stepping candidates",
        path="production_artifact_probe.debug_map.object_model_source_identity.stepping_candidate_count",
    )

    required_kinds = set(_safe_str(item) for item in _list(minimums.get("required_identity_kinds")))
    if not required_kinds:
        required_kinds = set(REQUIRED_IDENTITY_KINDS)
    present_kinds = set(_safe_str(item) for item in _list(source_identity.get("required_identity_kinds_present")))
    record_kinds = {
        _safe_str(_object(record).get("runtime_identity_kind"))
        for record in records
    }
    missing_kinds = sorted(required_kinds - present_kinds)
    missing_record_kinds = sorted(required_kinds - record_kinds)
    if missing_kinds or missing_record_kinds:
        diagnostics.append(
            _diag(
                "production-source-identity-kind-missing",
                "production object-model source identity is missing required runtime identity kinds",
                "production_artifact_probe.debug_map.object_model_source_identity.required_identity_kinds_present",
            )
        )

    row_ids = {
        _safe_str(_object(row).get("row_id"))
        for row in rows
        if _safe_str(_object(row).get("row_id"))
    }
    rows_by_id = {
        _safe_str(_object(row).get("row_id")): _object(row)
        for row in rows
        if _safe_str(_object(row).get("row_id"))
    }
    if len(rows_by_id) != len(rows):
        diagnostics.append(
            _diag(
                "production-source-identity-row-drift",
                "production native line-table row ids must be unique",
                "production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows",
            )
        )
    records_by_id: dict[str, dict[str, Any]] = {}
    record_ids: set[str] = set()
    for index, record_item in enumerate(records):
        record = _object(record_item)
        record_id = _safe_str(record.get("source_map_record_id"))
        row_id = _safe_str(record.get("native_line_table_row_id"))
        record_ids.add(record_id)
        identity_kind = _safe_str(record.get("runtime_identity_kind"))
        records_by_id[record_id] = record
        if (
            not record_id
            or identity_kind not in required_kinds
            or row_id not in row_ids
        ):
            diagnostics.append(
                _diag(
                    "production-source-identity-record-invalid",
                    "production object-model source identity record must link a required kind to a line-table row",
                    f"production_artifact_probe.debug_map.object_model_source_identity.source_map_records.{index}",
                )
            )
        if _safe_str(record.get("source_truth_kind")) != REQUIRED_SOURCE_TRUTH_KIND:
            diagnostics.append(
                _diag(
                    "production-source-identity-record-stale",
                    "production source-map record must be compiler-owned manifest source truth",
                    f"production_artifact_probe.debug_map.object_model_source_identity.source_map_records.{index}.source_truth_kind",
                )
            )
        expected_record_kinds = _source_map_record_kinds_for_identity(identity_kind)
        if (
            expected_record_kinds
            and _safe_str(record.get("source_map_record_kind")) not in expected_record_kinds
        ):
            diagnostics.append(
                _diag(
                    "production-source-identity-record-stale",
                    "production source-map record kind drifted from its runtime identity kind",
                    f"production_artifact_probe.debug_map.object_model_source_identity.source_map_records.{index}.source_map_record_kind",
                )
            )
        if _safe_str(record.get("source_path")).replace("\\", "/") != expected_source:
            diagnostics.append(
                _diag(
                    "production-source-identity-source-drift",
                    "production source identity record source path drifted",
                    f"production_artifact_probe.debug_map.object_model_source_identity.source_map_records.{index}.source_path",
                )
            )
        if not _positive_int(record.get("line")) or not _positive_int(record.get("column")):
            diagnostics.append(
                _diag(
                    "production-source-identity-record-stale",
                    "production source-map record must carry exact source line and column anchors",
                    f"production_artifact_probe.debug_map.object_model_source_identity.source_map_records.{index}",
                )
            )
        if not _safe_str(record.get("display_name")):
            diagnostics.append(
                _diag(
                    "production-source-identity-record-stale",
                    "production source-map record must carry a runtime identity display name",
                    f"production_artifact_probe.debug_map.object_model_source_identity.source_map_records.{index}.display_name",
                )
            )
    for index, row_item in enumerate(rows):
        row = _object(row_item)
        linked_record = records_by_id.get(_safe_str(row.get("source_map_record_id")))
        if linked_record is None:
            diagnostics.append(
                _diag(
                    "production-source-identity-row-unlinked",
                    "production native line-table projection row must link back to a source-map record",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}",
                )
            )
        else:
            for key in ("runtime_identity_kind", "source_path", "line", "column"):
                row_value = row.get(key)
                record_value = linked_record.get(key)
                if key == "source_path":
                    row_value = _safe_str(row_value).replace("\\", "/")
                    record_value = _safe_str(record_value).replace("\\", "/")
                if row_value != record_value:
                    diagnostics.append(
                        _diag(
                            "production-source-identity-row-drift",
                            "production native line-table row must exactly mirror its source-map anchor",
                            f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.{key}",
                        )
                    )
            if _safe_str(linked_record.get("native_line_table_row_id")) != _safe_str(row.get("row_id")):
                diagnostics.append(
                    _diag(
                        "production-source-identity-row-drift",
                        "production source-map record and native line-table row must point at each other",
                        f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.row_id",
                    )
                )
        if _safe_str(row.get("native_line_table_model")) != REQUIRED_NATIVE_LINE_TABLE_MODEL:
            diagnostics.append(
                _diag(
                    "production-source-identity-row-stale",
                    "production native line-table row must use the canonical publication model",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.native_line_table_model",
                )
            )
        if row.get("native_debug_info_emitted") is not True:
            diagnostics.append(
                _diag(
                    "production-source-identity-incomplete",
                    "production native line-table projection row must link to emitted native debug info",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.native_debug_info_emitted",
                )
            )
        if row.get("native_line_table_emitted") is not True:
            diagnostics.append(
                _diag(
                    "production-source-identity-incomplete",
                    "production native line-table projection row must confirm emitted native line-table support",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.native_line_table_emitted",
                )
            )
        if (
            native_debug_info_evidence_id
            and row.get("native_debug_info_evidence_id") != native_debug_info_evidence_id
        ):
            diagnostics.append(
                _diag(
                    "production-native-debug-info-evidence-drift",
                    "production native line-table row must link the native debug-info evidence id",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.native_debug_info_evidence_id",
                )
            )
        if (
            native_debug_info_evidence_id
            and row.get("native_line_table_evidence_id") != native_debug_info_evidence_id
        ):
            diagnostics.append(
                _diag(
                    "production-native-debug-info-evidence-drift",
                    "production native line-table row must link the native line-table evidence id",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.native_line_table_evidence_id",
                )
            )
        if not _safe_str(row.get("native_debug_info_blocker")):
            diagnostics.append(
                _diag(
                    "production-native-debug-info-evidence-incomplete",
                    "production native line-table row must carry the native debug-info blocker",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.native_debug_info_blocker",
                )
            )
        if _safe_str(row.get("runtime_identity_kind")) == "method" and not _safe_str(row.get("expected_native_symbol")):
            diagnostics.append(
                _diag(
                    "production-source-identity-row-stale",
                    "production method native line-table row must carry the expected native symbol",
                    f"production_artifact_probe.debug_map.object_model_source_identity.native_line_table_rows.{index}.expected_native_symbol",
                )
            )
    required_candidate_status = _safe_str(
        step_reservation.get("required_candidate_status")
    ) or REQUIRED_STEPPING_STATUS
    for index, candidate_item in enumerate(stepping_candidates):
        candidate = _object(candidate_item)
        candidate_record = records_by_id.get(_safe_str(candidate.get("source_map_record_id")))
        candidate_row = rows_by_id.get(_safe_str(candidate.get("native_line_table_row_id")))
        if candidate_record is None:
            diagnostics.append(
                _diag(
                    "production-source-identity-stepping-unlinked",
                    "production method stepping candidate must link back to a source-map record",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}",
                )
            )
        if candidate_row is None:
            diagnostics.append(
                _diag(
                    "production-source-identity-stepping-unlinked",
                    "production method stepping candidate must link back to a native line-table row",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.native_line_table_row_id",
                )
            )
        if candidate_record is not None and candidate_row is not None:
            if _safe_str(candidate_record.get("runtime_identity_kind")) != "method":
                diagnostics.append(
                    _diag(
                        "production-source-identity-stepping-anchor-drift",
                        "production statement-step candidate must be backed by a method source-map record",
                        f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.source_map_record_id",
                    )
                )
            if candidate_row.get("source_map_record_id") != candidate.get("source_map_record_id"):
                diagnostics.append(
                    _diag(
                        "production-source-identity-stepping-anchor-drift",
                        "production statement-step candidate source-map and native-line row links drifted",
                        f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.native_line_table_row_id",
                    )
                )
            for key in ("source_path", "line", "column"):
                candidate_value = candidate.get(key)
                row_value = candidate_row.get(key)
                if key == "source_path":
                    candidate_value = _safe_str(candidate_value).replace("\\", "/")
                    row_value = _safe_str(row_value).replace("\\", "/")
                if candidate_value != row_value:
                    diagnostics.append(
                        _diag(
                            "production-source-identity-stepping-anchor-drift",
                            "production statement-step candidate must carry exact source/native line anchors from its row",
                            f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.{key}",
                        )
                    )
            for key in ("owner_name", "selector"):
                if _safe_str(candidate.get(key)) != _safe_str(candidate_record.get(key)):
                    diagnostics.append(
                        _diag(
                            "production-source-identity-stepping-anchor-drift",
                            "production statement-step candidate identity drifted from its method record",
                            f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.{key}",
                        )
                    )
        if candidate.get("status") != required_candidate_status:
            diagnostics.append(
                _diag(
                    "production-source-identity-overclaimed",
                    "production method stepping candidate must stay in the reserved native-line-table-ready state",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.status",
                )
            )
        if not _safe_str(candidate.get("runtime_debug_trace_step_id")):
            diagnostics.append(
                _diag(
                    "production-source-identity-stepping-anchor-drift",
                    "production method stepping candidate must reserve a runtime debug trace step id",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.runtime_debug_trace_step_id",
                )
            )
        if (
            candidate.get("native_debug_info_emitted") is not True
            or candidate.get("native_line_table_emitted") is not True
        ):
            diagnostics.append(
                _diag(
                    "production-source-identity-incomplete",
                    "production method stepping candidate must carry emitted native debug and line-table evidence before stepping integration",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}",
                )
            )
        if (
            native_debug_info_evidence_id
            and candidate.get("native_debug_info_evidence_id") != native_debug_info_evidence_id
        ):
            diagnostics.append(
                _diag(
                    "production-native-debug-info-evidence-drift",
                    "production stepping candidate must link the native debug-info evidence id",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.native_debug_info_evidence_id",
                )
            )
        candidate_blockers = set(
            _safe_str(item) for item in _list(candidate.get("blocked_by"))
        )
        if REQUIRED_STEPPING_BLOCKER not in candidate_blockers:
            diagnostics.append(
                _diag(
                    "production-native-debug-info-evidence-incomplete",
                    "production stepping candidate must keep runtime statement-stepping integration blocked",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.blocked_by",
                )
            )
        if candidate_blockers & {
            "native-object-lacks-debug-info-section",
            "native-object-lacks-debug-line-section",
            "compiler-ir-lacks-llvm-di-locations",
        }:
            diagnostics.append(
                _diag(
                    "production-native-debug-info-evidence-stale-blocker",
                    "production stepping candidate must clear missing native debug blockers once debug sections are emitted",
                    f"production_artifact_probe.debug_map.object_model_source_identity.stepping_candidates.{index}.blocked_by",
                )
            )

    _validate_production_source_map_publication_payload(
        source_identity,
        records=records,
        rows=rows,
        required_kinds=required_kinds,
        minimums=publication_minimums,
        expected_source=expected_source,
        diagnostics=diagnostics,
    )


def _validate_production_probe_artifacts(
    probe: dict[str, Any],
    compatibility: dict[str, Any],
    step_reservation: dict[str, Any],
    artifacts: ProductionProbeArtifacts,
    diagnostics: list[Diagnostic],
) -> None:
    expected_source = _repo_path_text(_safe_str(probe.get("source_fixture")))
    minimums = _object(probe.get("runtime_inventory_minimums"))

    summary = artifacts.summary
    summary_paths = _object(summary.get("paths"))
    if summary.get("success") is not True or summary.get("status") != 0:
        diagnostics.append(
            _diag(
                "production-compile-failed",
                "production artifact probe must compile the object-model fixture successfully",
                "production_artifact_probe.compile_summary",
            )
        )
    if _safe_str(summary.get("input_path")).replace("\\", "/") != expected_source:
        diagnostics.append(
            _diag(
                "production-source-drift",
                "production compile summary input path drifted from the object-model fixture",
                "production_artifact_probe.compile_summary.input_path",
            )
        )
    for key in ("manifest", "ir", "object", "runtime_metadata_binary"):
        if not _path_exists_in_repo(summary_paths.get(key)):
            diagnostics.append(
                _diag(
                    "production-artifact-missing",
                    f"production compile summary did not publish artifact: {key}",
                    f"production_artifact_probe.compile_summary.paths.{key}",
                )
            )

    manifest = artifacts.manifest
    if _safe_str(manifest.get("source")).replace("\\", "/") != expected_source:
        diagnostics.append(
            _diag(
                "production-manifest-source-drift",
                "production manifest source drifted from the object-model fixture",
                "production_artifact_probe.manifest.source",
            )
        )
    runtime_records = _object(manifest.get("runtime_metadata_source_records"))
    if runtime_records.get("deterministic") is not True:
        diagnostics.append(
            _diag(
                "production-runtime-records-not-deterministic",
                "production manifest runtime metadata source records must be deterministic",
                "production_artifact_probe.manifest.runtime_metadata_source_records",
            )
        )
    manifest_counts = {
        "class_records": len(_list(manifest.get("interfaces"))),
        "protocol_records": len(_list(manifest.get("protocols"))),
        "category_records": len(_list(manifest.get("categories"))),
        "property_records": len(_list(runtime_records.get("properties"))),
        "ivar_records": len(_list(runtime_records.get("ivars"))),
        "method_records": len(_list(runtime_records.get("methods"))),
    }
    for key, actual in manifest_counts.items():
        _validate_count_at_least(
            actual,
            minimums.get(key),
            diagnostics,
            code="production-runtime-inventory-incomplete",
            message=f"production manifest lacks required object-model {key}",
            path=f"production_artifact_probe.manifest.{key}",
        )

    source_graph = artifacts.source_graph
    if source_graph.get("contract_id") != "objc3c.developer.tooling.source.graph.v1":
        diagnostics.append(
            _diag(
                "production-source-graph-contract-id",
                "production source graph contract id drifted",
                "production_artifact_probe.source_graph.contract_id",
            )
        )
    if source_graph.get("available") is not True or not _safe_str(source_graph.get("source_graph_digest")):
        diagnostics.append(
            _diag(
                "production-source-graph-unavailable",
                "production source graph must be available and digest-backed",
                "production_artifact_probe.source_graph",
            )
        )
    if _safe_str(source_graph.get("source_path")).replace("\\", "/") != expected_source:
        diagnostics.append(
            _diag(
                "production-source-graph-source-drift",
                "production source graph source path drifted from the object-model fixture",
                "production_artifact_probe.source_graph.source_path",
            )
        )
    _validate_count_at_least(
        source_graph.get("node_count"),
        minimums.get("source_graph_nodes"),
        diagnostics,
        code="production-source-graph-incomplete",
        message="production source graph lacks required object-model nodes",
        path="production_artifact_probe.source_graph.node_count",
    )

    artifact_inspector = artifacts.artifact_inspector
    if artifact_inspector.get("contract_id") != compatibility.get("required_contract_id"):
        diagnostics.append(
            _diag(
                "production-artifact-inspector-contract-id",
                "production artifact inspector contract id drifted",
                "production_artifact_probe.artifact_inspector.contract_id",
            )
        )
    if artifact_inspector.get("supported") is not True:
        diagnostics.append(
            _diag(
                "production-artifact-inspector-unavailable",
                "production artifact inspector must be supported for the object-model fixture",
                "production_artifact_probe.artifact_inspector.supported",
            )
        )
    if artifact_inspector.get("support_class") != compatibility.get("required_support_class"):
        diagnostics.append(
            _diag(
                "production-artifact-inspector-fail-closed",
                "production artifact inspector must have a ready compile artifact inventory",
                "production_artifact_probe.artifact_inspector.support_class",
            )
        )
    inventory_validation = _object(artifact_inspector.get("inventory_validation"))
    if inventory_validation.get("inventory_ready") is not True or inventory_validation.get("fail_closed") is True:
        diagnostics.append(
            _diag(
                "production-artifact-inventory-not-ready",
                "production artifact inspector inventory must be ready and non-fail-closed",
                "production_artifact_probe.artifact_inspector.inventory_validation",
            )
        )
    runtime_inventory = _object(artifact_inspector.get("runtime_inventory"))
    if runtime_inventory.get("available") is not True:
        diagnostics.append(
            _diag(
                "production-runtime-inventory-unavailable",
                "production artifact inspector must publish runtime inventory",
                "production_artifact_probe.artifact_inspector.runtime_inventory",
            )
        )
    expected_reflection_abi = _safe_str(
        compatibility.get("required_runtime_inventory_reflection_abi_version")
    )
    if not _safe_str(runtime_inventory.get("reflection_abi_version")):
        diagnostics.append(
            _diag(
                "production-runtime-abi-version-missing",
                "production runtime inventory must expose the reflection ABI version source",
                "production_artifact_probe.artifact_inspector.runtime_inventory.reflection_abi_version",
            )
        )
    elif expected_reflection_abi and runtime_inventory.get("reflection_abi_version") != expected_reflection_abi:
        diagnostics.append(
            _diag(
                "production-runtime-abi-version-drift",
                "production runtime inventory reflection ABI version source drifted",
                "production_artifact_probe.artifact_inspector.runtime_inventory.reflection_abi_version",
            )
        )
    for key in ("class_record_count", "protocol_record_count", "category_record_count", "property_record_count", "method_record_count"):
        minimum_key = key.replace("_count", "s")
        _validate_count_at_least(
            runtime_inventory.get(key),
            minimums.get(minimum_key),
            diagnostics,
            code="production-runtime-inventory-incomplete",
            message=f"production artifact inspector lacks required object-model {minimum_key}",
            path=f"production_artifact_probe.artifact_inspector.runtime_inventory.{key}",
        )
    artifact_links = _object(artifact_inspector.get("artifact_links"))
    for key in ("manifest_link", "ir_link", "runtime_metadata_link", "source_graph_link", "debug_map_link"):
        if not _path_exists_in_repo(artifact_links.get(key)):
            diagnostics.append(
                _diag(
                    "production-artifact-link-missing",
                    f"production artifact inspector link is missing or stale: {key}",
                    f"production_artifact_probe.artifact_inspector.artifact_links.{key}",
                )
            )

    debug_map = artifacts.debug_map
    if debug_map.get("contract_id") != "objc3c.developer.tooling.debug.map.surface.v1":
        diagnostics.append(
            _diag(
                "production-debug-map-contract-id",
                "production debug-map contract id drifted",
                "production_artifact_probe.debug_map.contract_id",
            )
        )
    if debug_map.get("supported") is not True or debug_map.get("object_artifact_present") is not True:
        diagnostics.append(
            _diag(
                "production-debug-map-unavailable",
                "production debug map must stay tied to the emitted object artifact",
                "production_artifact_probe.debug_map",
            )
        )
    if debug_map.get("source_map_supported") is not False:
        diagnostics.append(
            _diag(
                "production-debug-map-overclaimed",
                "production debug map must not claim full source-map support before line-table emission lands",
                "production_artifact_probe.debug_map.source_map_supported",
            )
        )
    if debug_map.get("statement_level_stepping") is not False:
        diagnostics.append(
            _diag(
                "production-debug-map-overclaimed",
                "production debug map must not claim statement stepping before debugger integration lands",
                "production_artifact_probe.debug_map.statement_level_stepping",
            )
        )
    _validate_native_debug_info_evidence(
        _object(debug_map.get("native_debug_info_evidence")),
        diagnostics,
        path="production_artifact_probe.debug_map.native_debug_info_evidence",
    )
    _validate_count_at_least(
        debug_map.get("declaration_breakpoint_anchor_count"),
        minimums.get("declaration_breakpoint_anchors"),
        diagnostics,
        code="production-debug-map-incomplete",
        message="production debug map lacks declaration breakpoint anchors",
        path="production_artifact_probe.debug_map.declaration_breakpoint_anchor_count",
    )
    _validate_production_source_identity_payload(
        debug_map,
        probe,
        step_reservation,
        expected_source,
        diagnostics,
    )


def _validate_production_artifact_probe(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
    *,
    run_production_probe: bool,
) -> None:
    probe = _validate_production_probe_contract(payload, diagnostics)
    compatibility = _validate_artifact_inspector_compatibility_contract(
        payload, diagnostics
    )
    step_reservation = _validate_statement_step_reservation_contract(
        payload, diagnostics
    )
    if not probe or not run_production_probe:
        return
    try:
        artifacts = _build_production_probe_artifacts(probe)
    except Exception as exc:  # pragma: no cover - deterministic guard for local toolchain failures.
        diagnostics.append(
            _diag(
                "production-probe-run-failed",
                f"unable to run production artifact probe: {type(exc).__name__}: {exc}",
                "production_artifact_probe",
            )
        )
        return
    _validate_production_probe_artifacts(
        probe, compatibility, step_reservation, artifacts, diagnostics
    )


def validate_contract_path(
    path: Path | str = DEFAULT_CONTRACT_PATH,
    *,
    run_production_probe: bool = False,
) -> ValidationResult:
    contract_path, payload, load_diagnostics = _load_json(path)
    diagnostics: list[Diagnostic] = [*load_diagnostics]

    if payload.get("contract_id") != CONTRACT_ID:
        diagnostics.append(_diag("contract-id", f"contract_id must be {CONTRACT_ID}", "contract_id"))
    if payload.get("issue") != 8198:
        diagnostics.append(_diag("issue-id", "contract must be owned by issue 8198", "issue"))
    roadmap_issue_links = {
        _safe_int(item)
        for item in _list(payload.get("roadmap_issue_links"))
        if _safe_int(item)
    }
    for issue_id in sorted(REQUIRED_ROADMAP_ISSUE_LINKS - roadmap_issue_links):
        diagnostics.append(
            _diag(
                "roadmap-issue-link-missing",
                f"object-model debugger proof must link roadmap issue {issue_id}",
                "roadmap_issue_links",
            )
        )
    if payload.get("capability_id") != "runtime.object-model.full-realization":
        diagnostics.append(
            _diag(
                "capability-id",
                "contract must target runtime.object-model.full-realization",
                "capability_id",
            )
        )
    if payload.get("public_status") != "reserved":
        diagnostics.append(_diag("public-status", "object-model debugger proof must keep the umbrella reserved", "public_status"))

    source_map_bundle_path = _contract_path(
        payload,
        "source_map_bundle",
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "developer_tooling"
        / "debug_source_maps"
        / "positive.json",
    )
    debugger_replay_path = _contract_path(
        payload,
        "debugger_replay",
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "developer_tooling"
        / "debugger_integration"
        / "replay.json",
    )
    debug_anchor_path = _contract_path(
        payload,
        "debug_anchor_identity_contract",
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "object_model_closure"
        / "debug_anchor_identity_replay_contract.json",
    )

    try:
        source_bundle = load_bundle(source_map_bundle_path)
        source_result = validate_bundle(source_bundle)
    except Exception as exc:  # pragma: no cover - deterministic guard for corrupt external paths.
        diagnostics.append(
            _diag(
                "source-map-bundle-invalid",
                f"unable to load source-map bundle: {type(exc).__name__}: {exc}",
                display_path(source_map_bundle_path),
            )
        )
        source_bundle = None
    else:
        diagnostics.extend(
            _diag(f"source-map:{diagnostic.code}", diagnostic.message, diagnostic.path)
            for diagnostic in source_result.diagnostics
        )

    replay_result = validate_replay_path(debugger_replay_path)
    diagnostics.extend(
        _diag(f"debugger-replay:{diagnostic.code}", diagnostic.message, diagnostic.path)
        for diagnostic in replay_result.diagnostics
    )
    _, replay_payload, replay_load_diagnostics = _load_json(debugger_replay_path)
    diagnostics.extend(
        _diag(f"debugger-replay:{diagnostic.code}", diagnostic.message, diagnostic.path)
        for diagnostic in replay_load_diagnostics
    )

    _, debug_anchor_payload, debug_anchor_diagnostics = _load_json(debug_anchor_path)
    diagnostics.extend(
        _diag(f"debug-anchor:{diagnostic.code}", diagnostic.message, diagnostic.path)
        for diagnostic in debug_anchor_diagnostics
    )
    if debug_anchor_payload.get("contract_id") != DEBUG_ANCHOR_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "debug-anchor-contract-id",
                f"debug-anchor contract_id must be {DEBUG_ANCHOR_CONTRACT_ID}",
                "debug_anchor_identity_contract",
            )
        )

    _validate_reflection_abi_governance(payload, debug_anchor_payload, diagnostics)
    _validate_debug_anchor_contract(payload, debug_anchor_payload, diagnostics)
    _validate_boundaries(payload, diagnostics)
    _validate_debugger_umbrella_readiness(payload, diagnostics)
    _validate_production_artifact_probe(
        payload,
        diagnostics,
        run_production_probe=run_production_probe,
    )

    if source_bundle is not None:
        source_maps = {entry.entry_id: entry for entry in source_bundle.source_maps}
        debug_maps = {entry.entry_id: entry for entry in source_bundle.debug_maps}
        line_rows = {row.row_id: row for row in source_bundle.native_line_tables}
        source_record_kinds = {entry.record_kind for entry in source_bundle.source_maps}
        _validate_required_sets(payload, replay_payload, source_record_kinds, diagnostics)
        _validate_source_backed_debug_anchors(
            payload,
            source_maps=source_maps,
            debug_maps=debug_maps,
            line_rows=line_rows,
            debug_anchor_queries=_debug_anchor_queries(debug_anchor_payload),
            diagnostics=diagnostics,
        )
        _validate_artifact_links(
            payload,
            source_maps=source_maps,
            debug_maps=debug_maps,
            line_rows=line_rows,
            debug_anchor_queries=_debug_anchor_queries(debug_anchor_payload),
            contract_value_records=_contract_value_records(payload),
            commands=_replay_commands(replay_payload),
            diagnostics=diagnostics,
        )

    return ValidationResult(
        ok=not diagnostics,
        diagnostics=tuple(diagnostics),
        contract_path=display_path(contract_path),
        source_map_bundle_path=display_path(source_map_bundle_path),
        debugger_replay_path=display_path(debugger_replay_path),
    )
