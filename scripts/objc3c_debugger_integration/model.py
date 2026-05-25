"""Fail-closed LLDB/debugger replay model for Objective-C 3 source maps."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_debug_maps import load_bundle, validate_bundle
from objc3c_tooling.paths import ROOT, display_path, repo_rel, resolve_repo_path

CONTRACT_ID = "objc3c.debugger-integration.replay.v1"
VALIDATION_CONTRACT_ID = "objc3c.debugger-integration.validation.v1"
PLAN_CONTRACT_ID = "objc3c.debugger-integration.stepping-plan.v1"
LLDB_PROTOCOL_CONTRACT_ID = "objc3c.lldb-plugin.protocol.v1"
SUPPORTED_COMPILER_ID = "objc3c.compiler.objc3c-3.0.debug-info.v1"
DEFAULT_FIXTURE_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "debugger_integration"
    / "replay.json"
)

SUPPORTED_STEP_KINDS = frozenset(
    {"statement", "function", "method", "message-send", "property-access", "runtime-helper-call"}
)
SUPPORTED_STEP_OPERATIONS = frozenset({"step-in", "step-over", "step-out"})
REQUIRED_OBJECT_MODEL_STEP_CONTEXTS = frozenset(
    {"method-call", "property-accessor", "category-method", "protocol-method-body", "reflection-probe-call"}
)
SUPPORTED_RUNTIME_STEP_CONTEXTS = REQUIRED_OBJECT_MODEL_STEP_CONTEXTS | frozenset({"statement", "function-call"})
STEP_KIND_COMMAND_IDS = {
    "statement": "step-statement",
    "function": "step-function",
    "method": "step-method",
    "message-send": "step-message-send",
    "property-access": "step-property-accessor",
    "runtime-helper-call": "step-runtime-helper-call",
}
STEP_OPERATION_COMMAND_IDS = {
    "step-in": "step-in",
    "step-out": "step-out",
}
SUPPORTED_COMMANDS = frozenset(
    {
        "objc3 metadata load",
        "objc3 class list",
        "objc3 selector list",
        "objc3 class inspect",
        "objc3 object inspect",
        "objc3 stdlib text inspect",
        "objc3 stdlib collection inspect",
        "objc3 source-location",
        "objc3 async lanes",
        "objc3 runtime-debug trace",
        "objc3 step statement",
        "objc3 step function",
        "objc3 step method",
        "objc3 step message-send",
        "objc3 step property-accessor",
        "objc3 step runtime-helper-call",
        "objc3 step in",
        "objc3 step out",
        "objc3 explain optimized-frame",
    }
)
REQUIRED_COMMAND_IDS = frozenset(
    {
        "load-module-metadata",
        "list-classes",
        "list-selectors",
        "inspect-class",
        "inspect-object",
        "inspect-stdlib-text",
        "inspect-stdlib-collection",
        "show-source-location",
        "show-async-lanes",
        "show-runtime-debug-trace",
        "step-statement",
        "step-function",
        "step-method",
        "step-message-send",
        "step-property-accessor",
        "step-runtime-helper-call",
        "step-in",
        "step-out",
        "explain-optimized-frame",
    }
)
SUPPORTED_VALUE_KINDS = frozenset(
    {
        "class-metadata",
        "selector-metadata",
        "object-handle",
        "stdlib-text-handle",
        "stdlib-collection-handle",
        "async-actor-lanes",
        "error-bridge-state",
        "runtime-debug-trace",
    }
)
UNSUPPORTED_REASONS = frozenset(
    {
        "missing-debug-info",
        "optimized-away",
        "unsupported-handle",
        "malformed-runtime-metadata",
        "unsupported-plugin-command",
        "source-map-object-digest-mismatch",
        "generated-only-source-map",
        "stale-compiler-id",
        "missing-inline-frame-chain",
        "unsupported-runtime-metadata",
        "optimized-transform-map-missing",
        "unsupported-step-operation",
        "missing-step-operation-coverage",
        "missing-object-model-step-context",
        "private-snapshot-only-evidence",
    }
)
SUPPORTED_RUNTIME_METADATA_KINDS = frozenset(
    {
        "class",
        "selector",
        "object",
        "stdlib-text",
        "stdlib-collection",
        "async-actor-lanes",
        "error-bridge",
        "runtime-debug-trace",
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
    replay_path: str
    source_map_bundle_path: str

    def to_payload(self) -> dict[str, object]:
        return {
            "ok": self.ok,
            "contract_id": VALIDATION_CONTRACT_ID,
            "replay_path": self.replay_path,
            "source_map_bundle_path": self.source_map_bundle_path,
            "diagnostics": [diagnostic.to_payload() for diagnostic in self.diagnostics],
        }


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


def _tuple_str(value: object) -> tuple[str, ...]:
    if not isinstance(value, list):
        return ()
    return tuple(item for item in value if isinstance(item, str))


def _load_replay(path: Path | str) -> tuple[Path, dict[str, Any], tuple[Diagnostic, ...]]:
    replay_path = resolve_repo_path(path)
    try:
        raw = replay_path.read_text(encoding="utf-8")
    except OSError as exc:
        return replay_path, {}, (
            _diag("replay-read-failed", f"unable to read debugger replay fixture: {exc}", display_path(replay_path)),
        )
    try:
        payload = json.loads(raw)
    except json.JSONDecodeError as exc:
        return replay_path, {}, (
            _diag("replay-json-invalid", f"invalid JSON at {exc.lineno}:{exc.colno}: {exc.msg}", display_path(replay_path)),
        )
    if not isinstance(payload, dict):
        return replay_path, {}, (_diag("schema-type", "debugger replay fixture must be a JSON object", "$"),)
    return replay_path, payload, ()


def _source_map_bundle_path(payload: dict[str, Any]) -> Path:
    raw = _safe_str(payload.get("source_map_bundle"))
    return resolve_repo_path(raw) if raw else DEFAULT_FIXTURE_PATH


def validate_replay_path(path: Path | str = DEFAULT_FIXTURE_PATH) -> ValidationResult:
    replay_path, payload, load_diagnostics = _load_replay(path)
    diagnostics: list[Diagnostic] = [*load_diagnostics]
    bundle_path = _source_map_bundle_path(payload)

    if payload.get("contract_id") != CONTRACT_ID:
        diagnostics.append(_diag("contract-id", f"contract_id must be {CONTRACT_ID}", "contract_id"))

    try:
        source_bundle = load_bundle(bundle_path)
        source_result = validate_bundle(source_bundle)
    except Exception as exc:  # pragma: no cover - deterministic guard for corrupt external paths.
        diagnostics.append(
            _diag(
                "source-map-bundle-invalid",
                f"unable to load source-map bundle: {type(exc).__name__}: {exc}",
                display_path(bundle_path),
            )
        )
        return ValidationResult(
            ok=False,
            diagnostics=tuple(diagnostics),
            replay_path=display_path(replay_path),
            source_map_bundle_path=display_path(bundle_path),
        )

    diagnostics.extend(
        _diag(
            f"source-map:{diagnostic.code}",
            diagnostic.message,
            diagnostic.path,
        )
        for diagnostic in source_result.diagnostics
    )

    commands = _validate_commands(payload, diagnostics)
    source_maps = {entry.entry_id: entry for entry in source_bundle.source_maps}
    debug_maps_by_source = {entry.source_map_entry_id: entry for entry in source_bundle.debug_maps}
    line_rows = {row.row_id: row for row in source_bundle.native_line_tables}
    debug_config = _object(payload.get("debug_configuration"))
    debug_config_supported = _debug_config_supported(debug_config, diagnostics)
    protocol_contract = _validate_protocol_contract(payload, source_bundle, diagnostics)

    _validate_stepping_records(
        payload,
        commands,
        source_maps,
        debug_maps_by_source,
        line_rows,
        source_bundle.native_debug_info.evidence_id,
        debug_config_supported,
        protocol_contract,
        diagnostics,
    )
    _validate_value_inspection(payload, commands, protocol_contract, diagnostics)
    _validate_negative_cases(payload, diagnostics)

    return ValidationResult(
        ok=not diagnostics,
        diagnostics=tuple(diagnostics),
        replay_path=display_path(replay_path),
        source_map_bundle_path=display_path(bundle_path),
    )


def _debug_config_supported(debug_config: dict[str, Any], diagnostics: list[Diagnostic]) -> bool:
    profile = _safe_str(debug_config.get("profile"))
    optimization = _safe_str(debug_config.get("optimization"))
    native_debug_info = _safe_str(debug_config.get("native_debug_info"))
    artifact_digest_status = _safe_str(debug_config.get("artifact_digest_status"))
    supported = (
        profile == "debug"
        and optimization in {"none", "debug-preserved"}
        and native_debug_info == "present"
        and artifact_digest_status == "matches-source-map"
    )
    if not supported:
        diagnostics.append(
            _diag(
                "debug-configuration-unsupported",
                "supported stepping requires a debug profile, preserved optimization mode, native debug info, and a source-map-matched artifact digest",
                "debug_configuration",
            )
        )
    return supported


def _validate_protocol_contract(
    payload: dict[str, Any],
    source_bundle: Any,
    diagnostics: list[Diagnostic],
) -> dict[str, Any]:
    contract = _object(payload.get("protocol_contract"))
    compiler_identity = _object(source_bundle.payload.get("compiler_identity"))
    if contract.get("contract_id") != LLDB_PROTOCOL_CONTRACT_ID:
        diagnostics.append(
            _diag(
                "lldb-protocol-contract-id",
                f"LLDB protocol contract_id must be {LLDB_PROTOCOL_CONTRACT_ID}",
                "protocol_contract.contract_id",
            )
        )
    expected_compiler_id = _safe_str(contract.get("compiler_identity_id"))
    source_compiler_id = _safe_str(compiler_identity.get("compiler_id"))
    if expected_compiler_id != SUPPORTED_COMPILER_ID or source_compiler_id != expected_compiler_id:
        diagnostics.append(
            _diag(
                "lldb-protocol-stale-compiler-id",
                "LLDB protocol compiler id must match the source-map compiler identity",
                "protocol_contract.compiler_identity_id",
            )
        )
    if contract.get("rejects_generated_only_maps") is not True:
        diagnostics.append(
            _diag(
                "lldb-protocol-generated-only-map",
                "LLDB protocol must reject generated-only source maps",
                "protocol_contract.rejects_generated_only_maps",
            )
        )
    if contract.get("requires_inline_frame_chains") is not True:
        diagnostics.append(
            _diag(
                "lldb-protocol-inline-frame-chain-missing",
                "LLDB protocol must require inline-frame chains for optimized frames",
                "protocol_contract.requires_inline_frame_chains",
            )
        )
    if contract.get("requires_step_over_emitted_native_debug_info") is not True:
        diagnostics.append(
            _diag(
                "lldb-protocol-step-over-native-debug-info-missing",
                "LLDB protocol must require step-over records to consume emitted native debug info",
                "protocol_contract.requires_step_over_emitted_native_debug_info",
            )
        )
    required_operations = set(_tuple_str(contract.get("requires_step_operations")))
    if required_operations != SUPPORTED_STEP_OPERATIONS:
        diagnostics.append(
            _diag(
                "lldb-protocol-step-operation-coverage-missing",
                "LLDB protocol must require step-in, step-over, and step-out over emitted native debug info",
                "protocol_contract.requires_step_operations",
            )
        )
    required_contexts = set(_tuple_str(contract.get("requires_object_model_step_contexts")))
    if required_contexts != REQUIRED_OBJECT_MODEL_STEP_CONTEXTS:
        diagnostics.append(
            _diag(
                "lldb-protocol-object-model-context-missing",
                "LLDB protocol must require method-call, property-accessor, category-method, protocol-method-body, and reflection-probe stepping contexts",
                "protocol_contract.requires_object_model_step_contexts",
            )
        )
    if contract.get("rejects_private_snapshot_only_evidence") is not True:
        diagnostics.append(
            _diag(
                "lldb-protocol-private-snapshot-rejection-missing",
                "LLDB protocol must reject private snapshot-only debugger stepping evidence",
                "protocol_contract.rejects_private_snapshot_only_evidence",
            )
        )
    if contract.get("requires_native_debug_info_evidence_links") is not True:
        diagnostics.append(
            _diag(
                "lldb-protocol-native-debug-info-evidence-missing",
                "LLDB protocol must require every supported step to link emitted native debug-info evidence",
                "protocol_contract.requires_native_debug_info_evidence_links",
            )
        )
    inline_chain_ids = {chain.chain_id for chain in source_bundle.inline_debug_chains}
    for chain_id in _tuple_str(contract.get("required_inline_frame_chain_ids")):
        if chain_id not in inline_chain_ids:
            diagnostics.append(
                _diag(
                    "lldb-protocol-inline-frame-chain-missing",
                    f"LLDB protocol references missing inline-frame chain: {chain_id}",
                    "protocol_contract.required_inline_frame_chain_ids",
                )
            )
    supported_metadata = set(_tuple_str(contract.get("supported_runtime_metadata_kinds")))
    if not supported_metadata or supported_metadata - SUPPORTED_RUNTIME_METADATA_KINDS:
        diagnostics.append(
            _diag(
                "lldb-protocol-unsupported-runtime-metadata",
                "LLDB protocol lists unsupported runtime metadata kinds",
                "protocol_contract.supported_runtime_metadata_kinds",
            )
        )
    if contract.get("requires_supported_runtime_metadata") is not True:
        diagnostics.append(
            _diag(
                "lldb-protocol-unsupported-runtime-metadata",
                "LLDB protocol must reject unsupported runtime metadata",
                "protocol_contract.requires_supported_runtime_metadata",
            )
        )
    return contract


def _expected_command_id(step_kind: str, step_operation: str) -> str:
    return STEP_OPERATION_COMMAND_IDS.get(step_operation, STEP_KIND_COMMAND_IDS.get(step_kind, ""))


def _validate_commands(payload: dict[str, Any], diagnostics: list[Diagnostic]) -> dict[str, dict[str, Any]]:
    commands: dict[str, dict[str, Any]] = {}
    for index, item in enumerate(_list(_object(payload.get("lldb_plugin")).get("commands"))):
        command = _object(item)
        command_id = _safe_str(command.get("command_id"))
        plugin_command = _safe_str(command.get("plugin_command"))
        path = f"lldb_plugin.commands[{index}]"
        if not command_id:
            diagnostics.append(_diag("schema-missing-field", "LLDB command lacks command_id", path))
            continue
        if command_id in commands:
            diagnostics.append(_diag("lldb-command-duplicate", f"duplicate LLDB command id: {command_id}", path))
        commands[command_id] = command
        if plugin_command not in SUPPORTED_COMMANDS:
            diagnostics.append(_diag("lldb-command-unsupported", f"unsupported LLDB command: {plugin_command}", path))
        if command.get("replayable") is not True:
            diagnostics.append(_diag("lldb-command-not-replayable", f"LLDB command must be replayable: {command_id}", path))

    missing = sorted(REQUIRED_COMMAND_IDS - set(commands))
    for command_id in missing:
        diagnostics.append(_diag("lldb-command-missing", f"required LLDB command is missing: {command_id}", command_id))
    return commands


def _validate_stepping_records(
    payload: dict[str, Any],
    commands: dict[str, dict[str, Any]],
    source_maps: dict[str, Any],
    debug_maps_by_source: dict[str, Any],
    line_rows: dict[str, Any],
    native_debug_info_evidence_id: str,
    debug_config_supported: bool,
    protocol_contract: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    records = _list(_object(payload.get("stepping_plan")).get("records"))
    supported_seen: set[str] = set()
    operation_seen: set[str] = set()
    object_model_context_seen: set[str] = set()
    for index, item in enumerate(records):
        record = _object(item)
        path = f"stepping_plan.records[{index}]"
        status = _safe_str(record.get("status"))
        step_kind = _safe_str(record.get("step_kind"))
        step_operation = _safe_str(record.get("step_operation"))
        runtime_context_kind = _safe_str(record.get("runtime_context_kind"))
        entry_id = _safe_str(record.get("source_map_entry_id"))
        entry = source_maps.get(entry_id)

        if status == "unsupported":
            _validate_unsupported_step(record, path, commands, source_maps, debug_maps_by_source, protocol_contract, diagnostics)
            continue
        if status != "supported":
            diagnostics.append(_diag("stepping-status-invalid", "stepping record status must be supported or unsupported", path))
            continue
        supported_seen.add(step_kind)
        operation_seen.add(step_operation)
        if runtime_context_kind in REQUIRED_OBJECT_MODEL_STEP_CONTEXTS:
            object_model_context_seen.add(runtime_context_kind)
        if step_kind not in SUPPORTED_STEP_KINDS:
            diagnostics.append(_diag("stepping-kind-unsupported", f"unsupported stepping kind: {step_kind}", path))
        if step_operation not in SUPPORTED_STEP_OPERATIONS:
            diagnostics.append(
                _diag(
                    "stepping-operation-unsupported",
                    f"unsupported stepping operation: {step_operation}",
                    path,
                )
            )
        if runtime_context_kind not in SUPPORTED_RUNTIME_STEP_CONTEXTS:
            diagnostics.append(
                _diag(
                    "stepping-runtime-context-unsupported",
                    f"unsupported stepping runtime context: {runtime_context_kind}",
                    path,
                )
            )
        if not _safe_str(record.get("statement_unit_id")) or not _safe_str(record.get("runtime_context_id")):
            diagnostics.append(
                _diag(
                    "stepping-context-anchor-missing",
                    "supported stepping records must name a statement unit and runtime context anchor",
                    path,
                )
            )
        if protocol_contract.get("rejects_private_snapshot_only_evidence") is True and (
            record.get("private_snapshot_only") is True
            or _safe_str(record.get("artifact_scope")) != "public-production-artifact"
        ):
            diagnostics.append(
                _diag(
                    "private-snapshot-only-evidence",
                    "supported stepping records must be backed by public production artifacts, not private snapshots",
                    path,
                )
            )
        if not debug_config_supported:
            diagnostics.append(
                _diag(
                    "stepping-debug-config-unsupported",
                    "supported stepping cannot be claimed for an unsupported debug configuration",
                    path,
                )
            )
        if entry is None:
            diagnostics.append(_diag("source-map-entry-missing", f"stepping record references missing source map: {entry_id}", path))
            continue
        if protocol_contract.get("rejects_generated_only_maps") is True and entry.generated:
            diagnostics.append(
                _diag(
                    "lldb-protocol-generated-only-map",
                    f"LLDB stepping cannot use generated-only source maps: {entry_id}",
                    path,
                )
            )
        if entry.record_kind != step_kind:
            diagnostics.append(
                _diag("stepping-source-kind-mismatch", f"stepping kind does not match source-map kind: {entry_id}", path)
            )
        debug_map = debug_maps_by_source.get(entry_id)
        if debug_map is None:
            diagnostics.append(_diag("debug-map-entry-missing", f"stepping record lacks debug-map entry: {entry_id}", path))
        elif _safe_str(record.get("debug_map_entry_id")) != debug_map.entry_id:
            diagnostics.append(_diag("debug-map-entry-missing", f"stepping record debug-map id drifted from source map: {entry_id}", path))
        if _safe_str(record.get("source_digest")) != entry.source_digest:
            diagnostics.append(_diag("source-digest-stale", f"stepping record source digest drifted from source map: {entry_id}", path))
        if (
            not entry.source_range.is_valid()
            or _safe_int(record.get("source_line")) != entry.source_range.line
            or _safe_int(record.get("source_column")) != entry.source_range.column
            or _safe_int(record.get("source_end_line")) != entry.source_range.end_line
            or _safe_int(record.get("source_end_column")) != entry.source_range.end_column
            or not _safe_str(record.get("source_span_id"))
        ):
            diagnostics.append(_diag("stepping-anchor-missing", f"stepping record lacks a valid source line anchor: {entry_id}", path))
        if _safe_str(record.get("source_file")) != entry.source_file:
            diagnostics.append(_diag("stepping-anchor-missing", f"stepping record source file does not match source map: {entry_id}", path))
        if _safe_str(record.get("object_debug_line_anchor")) != entry.object_debug_line_anchor:
            diagnostics.append(_diag("stepping-anchor-missing", f"stepping record lacks object/debug line anchor: {entry_id}", path))
        if _safe_str(record.get("native_symbol")) != entry.native_symbol:
            diagnostics.append(_diag("stepping-anchor-missing", f"stepping record native symbol does not match source map: {entry_id}", path))
        row_id = _safe_str(record.get("native_line_table_row_id"))
        if not row_id or row_id not in entry.native_line_table_row_ids:
            diagnostics.append(_diag("line-table-row-missing", f"stepping record lacks source-map line-table row: {entry_id}", path))
        elif row_id not in line_rows:
            diagnostics.append(_diag("line-table-row-missing", f"stepping record references missing line-table row: {row_id}", path))
        else:
            row = line_rows[row_id]
            if row.source_range != entry.source_range or row.object_debug_line_anchor != entry.object_debug_line_anchor:
                diagnostics.append(
                    _diag(
                        "stepping-anchor-missing",
                        f"stepping record native debug row drifted from source span: {entry_id}",
                        path,
                    )
                )
        if _safe_str(record.get("native_debug_info_evidence_id")) != native_debug_info_evidence_id:
            diagnostics.append(
                _diag(
                    "native-debug-info-evidence-missing",
                    f"stepping record does not link emitted native debug-info evidence: {entry_id}",
                    path,
                )
            )
        command_id = _safe_str(record.get("lldb_command_id"))
        expected_command_id = _expected_command_id(step_kind, step_operation)
        if command_id not in commands:
            diagnostics.append(_diag("lldb-command-missing", f"stepping record references missing LLDB command: {command_id}", path))
        elif command_id != expected_command_id:
            diagnostics.append(
                _diag(
                    "stepping-command-mismatch",
                    f"stepping record command does not match step kind and operation: {step_kind}/{step_operation}",
                    path,
                )
            )

    for step_kind in sorted(SUPPORTED_STEP_KINDS - supported_seen):
        diagnostics.append(_diag("stepping-record-missing", f"supported stepping record is missing: {step_kind}", step_kind))
    for step_operation in sorted(SUPPORTED_STEP_OPERATIONS - operation_seen):
        diagnostics.append(
            _diag(
                "stepping-operation-coverage-missing",
                f"supported stepping operation is missing: {step_operation}",
                step_operation,
            )
        )
    for context_kind in sorted(REQUIRED_OBJECT_MODEL_STEP_CONTEXTS - object_model_context_seen):
        diagnostics.append(
            _diag(
                "stepping-object-model-context-missing",
                f"object-model stepping context is missing: {context_kind}",
                context_kind,
            )
        )


def _validate_unsupported_step(
    record: dict[str, Any],
    path: str,
    commands: dict[str, dict[str, Any]],
    source_maps: dict[str, Any],
    debug_maps_by_source: dict[str, Any],
    protocol_contract: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    reason = _safe_str(record.get("unsupported_reason"))
    if reason not in UNSUPPORTED_REASONS:
        diagnostics.append(_diag("unsupported-reason-invalid", f"unsupported stepping reason is invalid: {reason}", path))
    if record.get("claims_stepping") is True:
        diagnostics.append(_diag("stepping-overclaimed", "unsupported stepping record must not claim stepping support", path))
    if not _safe_str(record.get("diagnostic_code")):
        diagnostics.append(_diag("diagnostic-missing", "unsupported stepping record lacks diagnostic_code", path))
    command_id = _safe_str(record.get("lldb_command_id"))
    if command_id not in commands:
        diagnostics.append(_diag("lldb-command-missing", f"unsupported stepping record references missing LLDB command: {command_id}", path))
    entry_id = _safe_str(record.get("source_map_entry_id"))
    entry = source_maps.get(entry_id)
    if entry is None:
        diagnostics.append(_diag("source-map-entry-missing", f"unsupported stepping record references missing source map: {entry_id}", path))
        return
    if protocol_contract.get("rejects_generated_only_maps") is True and entry.generated:
        diagnostics.append(
            _diag(
                "lldb-protocol-generated-only-map",
                f"LLDB unsupported step cannot fall back to generated-only source maps: {entry_id}",
                path,
            )
        )
    debug_map = debug_maps_by_source.get(entry_id)
    if debug_map is None:
        diagnostics.append(_diag("debug-map-entry-missing", f"unsupported stepping record lacks debug-map entry: {entry_id}", path))
    elif _safe_str(record.get("debug_map_entry_id")) != debug_map.entry_id:
        diagnostics.append(_diag("debug-map-entry-missing", f"unsupported stepping record debug-map id drifted from source map: {entry_id}", path))
    if _safe_str(record.get("source_digest")) != entry.source_digest:
        diagnostics.append(_diag("source-digest-stale", f"unsupported stepping record source digest drifted from source map: {entry_id}", path))
    if reason == "optimized-away":
        inline_chain_id = _safe_str(record.get("inline_frame_chain_id"))
        required_inline_chain_ids = set(_tuple_str(protocol_contract.get("required_inline_frame_chain_ids")))
        if not inline_chain_id or inline_chain_id not in required_inline_chain_ids:
            diagnostics.append(
                _diag(
                    "lldb-protocol-inline-frame-chain-missing",
                    "optimized-away stepping must name a required inline-frame chain",
                    path,
                )
            )
        transform_entry_id = _safe_str(record.get("optimization_transform_source_map_entry_id"))
        transform_entry = source_maps.get(transform_entry_id)
        if transform_entry is None or transform_entry.record_kind != "optimization-transform-edge":
            diagnostics.append(
                _diag(
                    "optimized-transform-map-missing",
                    "optimized-away stepping must link the preserved optimization transform source map",
                    path,
                )
            )


def _validate_value_inspection(
    payload: dict[str, Any],
    commands: dict[str, dict[str, Any]],
    protocol_contract: dict[str, Any],
    diagnostics: list[Diagnostic],
) -> None:
    supported_metadata = set(_tuple_str(protocol_contract.get("supported_runtime_metadata_kinds")))
    for index, item in enumerate(_list(_object(payload.get("value_inspection")).get("records"))):
        record = _object(item)
        path = f"value_inspection.records[{index}]"
        value_kind = _safe_str(record.get("value_kind"))
        runtime_metadata_kind = _safe_str(record.get("runtime_metadata_kind"))
        if value_kind not in SUPPORTED_VALUE_KINDS:
            diagnostics.append(_diag("value-inspection-kind-unsupported", f"unsupported value inspection kind: {value_kind}", path))
        if protocol_contract.get("requires_supported_runtime_metadata") is True and runtime_metadata_kind not in supported_metadata:
            diagnostics.append(
                _diag(
                    "lldb-protocol-unsupported-runtime-metadata",
                    f"unsupported LLDB runtime metadata kind: {runtime_metadata_kind}",
                    path,
                )
            )
        command_id = _safe_str(record.get("lldb_command_id"))
        if command_id not in commands:
            diagnostics.append(_diag("lldb-command-missing", f"value inspection references missing LLDB command: {command_id}", path))
        if record.get("replayable") is not True:
            diagnostics.append(_diag("value-inspection-not-replayable", f"value inspection is not replayable: {value_kind}", path))
        if not _safe_str(record.get("runtime_anchor_id")):
            diagnostics.append(_diag("runtime-anchor-missing", f"value inspection lacks runtime anchor: {value_kind}", path))


def _validate_negative_cases(payload: dict[str, Any], diagnostics: list[Diagnostic]) -> None:
    cases = _list(payload.get("negative_cases"))
    expected = {
        "missing-debug-info",
        "optimized-away-statement",
        "unsupported-handle",
        "malformed-runtime-metadata",
        "unsupported-plugin-command",
        "source-map-object-digest-mismatch",
        "generated-only-source-map",
        "stale-compiler-id",
        "missing-inline-frame-chain",
        "unsupported-runtime-metadata",
        "optimized-transform-map-missing",
        "unsupported-step-operation",
        "missing-step-operation-coverage",
        "missing-object-model-step-context",
        "private-snapshot-only-evidence",
    }
    case_ids = {_safe_str(_object(item).get("case_id")) for item in cases}
    for case_id in sorted(expected - case_ids):
        diagnostics.append(_diag("negative-case-missing", f"debugger negative case is missing: {case_id}", case_id))
    for index, item in enumerate(cases):
        case = _object(item)
        path = f"negative_cases[{index}]"
        if _safe_str(case.get("unsupported_reason")) not in UNSUPPORTED_REASONS:
            diagnostics.append(_diag("unsupported-reason-invalid", "negative case has invalid unsupported_reason", path))
        if case.get("claims_stepping") is True:
            diagnostics.append(_diag("stepping-overclaimed", "negative case must not claim stepping support", path))
        if not _safe_str(case.get("expected_diagnostic_code")):
            diagnostics.append(_diag("diagnostic-missing", "negative case lacks expected diagnostic code", path))


def generate_stepping_plan_path(path: Path | str = DEFAULT_FIXTURE_PATH) -> dict[str, object]:
    replay_path, payload, load_diagnostics = _load_replay(path)
    result = validate_replay_path(path)
    if load_diagnostics or not result.ok:
        return {
            "ok": False,
            "contract_id": PLAN_CONTRACT_ID,
            "replay_path": display_path(replay_path),
            "diagnostics": [diagnostic.to_payload() for diagnostic in result.diagnostics],
            "commands": [],
            "steps": [],
            "unsupported": [],
        }

    bundle = load_bundle(_source_map_bundle_path(payload))
    source_maps = {entry.entry_id: entry for entry in bundle.source_maps}
    line_rows = {row.row_id: row for row in bundle.native_line_tables}
    commands = {
        _safe_str(item.get("command_id")): item
        for item in (_object(item) for item in _list(_object(payload.get("lldb_plugin")).get("commands")))
    }
    steps: list[dict[str, object]] = []
    unsupported: list[dict[str, object]] = []
    for item in (_object(item) for item in _list(_object(payload.get("stepping_plan")).get("records"))):
        if item.get("status") == "unsupported":
            unsupported.append(
                {
                    "record_id": _safe_str(item.get("record_id")),
                    "unsupported_reason": _safe_str(item.get("unsupported_reason")),
                    "diagnostic_code": _safe_str(item.get("diagnostic_code")),
                }
            )
            continue
        entry = source_maps[_safe_str(item.get("source_map_entry_id"))]
        row = line_rows[_safe_str(item.get("native_line_table_row_id"))]
        command = commands[_safe_str(item.get("lldb_command_id"))]
        steps.append(
            {
                "record_id": _safe_str(item.get("record_id")),
                "step_kind": entry.record_kind,
                "step_operation": _safe_str(item.get("step_operation")),
                "statement_unit_id": _safe_str(item.get("statement_unit_id")),
                "runtime_context_kind": _safe_str(item.get("runtime_context_kind")),
                "runtime_context_id": _safe_str(item.get("runtime_context_id")),
                "artifact_scope": _safe_str(item.get("artifact_scope")),
                "lldb_command": _safe_str(command.get("plugin_command")),
                "source_file": repo_rel(resolve_repo_path(entry.source_file)),
                "source_span_id": _safe_str(item.get("source_span_id")),
                "source_line": entry.source_range.line,
                "source_column": entry.source_range.column,
                "source_end_line": entry.source_range.end_line,
                "source_end_column": entry.source_range.end_column,
                "native_symbol": entry.native_symbol,
                "source_digest": entry.source_digest,
                "debug_map_entry_id": _safe_str(item.get("debug_map_entry_id")),
                "native_line": row.native_line,
                "object_debug_line_anchor": entry.object_debug_line_anchor,
                "source_map_entry_id": entry.entry_id,
                "native_line_table_row_id": row.row_id,
                "native_debug_info_evidence_id": _safe_str(item.get("native_debug_info_evidence_id")),
            }
        )
    return {
        "ok": True,
        "contract_id": PLAN_CONTRACT_ID,
        "replay_path": display_path(replay_path),
        "source_map_bundle_path": result.source_map_bundle_path,
        "commands": [
            {
                "command_id": command_id,
                "plugin_command": _safe_str(command.get("plugin_command")),
                "script_entry": _safe_str(command.get("script_entry")),
            }
            for command_id, command in sorted(commands.items())
        ],
        "steps": steps,
        "unsupported": unsupported,
        "diagnostics": [],
    }
