"""Fail-closed object-model debugger proof validation."""

from __future__ import annotations

import json
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


def _repo_path_text(path: Path | str) -> str:
    return display_path(resolve_repo_path(path)).replace("\\", "/")


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


def _validate_boundaries(payload: dict[str, Any], diagnostics: list[Diagnostic]) -> None:
    if payload.get("support_claim_published") is not False:
        diagnostics.append(
            _diag(
                "support-claim-overclaimed",
                "object-model debugger proof must not publish the reserved umbrella support claim",
                "support_claim_published",
            )
        )
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
    return probe


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


def _validate_production_probe_artifacts(
    probe: dict[str, Any],
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
    if artifact_inspector.get("contract_id") != "objc3c.developer.tooling.artifact.inspector.v1":
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
    if artifact_inspector.get("support_class") != "compile-artifact-inspector":
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
    if not _safe_str(runtime_inventory.get("reflection_abi_version")):
        diagnostics.append(
            _diag(
                "production-runtime-abi-version-missing",
                "production runtime inventory must expose the reflection ABI version source",
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
    _validate_count_at_least(
        debug_map.get("declaration_breakpoint_anchor_count"),
        minimums.get("declaration_breakpoint_anchors"),
        diagnostics,
        code="production-debug-map-incomplete",
        message="production debug map lacks declaration breakpoint anchors",
        path="production_artifact_probe.debug_map.declaration_breakpoint_anchor_count",
    )


def _validate_production_artifact_probe(
    payload: dict[str, Any],
    diagnostics: list[Diagnostic],
    *,
    run_production_probe: bool,
) -> None:
    probe = _validate_production_probe_contract(payload, diagnostics)
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
    _validate_production_probe_artifacts(probe, artifacts, diagnostics)


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

    _validate_boundaries(payload, diagnostics)
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
