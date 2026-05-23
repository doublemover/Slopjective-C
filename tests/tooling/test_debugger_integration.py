from __future__ import annotations

import json
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPTS_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.objc3c_debugger_integration import (
    generate_stepping_plan_path,
    validate_replay_path,
)
from scripts.objc3c_workflow.action_catalog import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS

FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "debugger_integration"
    / "replay.json"
)


def load_json(path: Path = FIXTURE) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def mutated_fixture(tmp_path: Path, mutation_path: list[object], value: object) -> Path:
    payload = deepcopy(load_json())
    cursor: Any = payload
    for part in mutation_path[:-1]:
        cursor = cursor[part]
    cursor[mutation_path[-1]] = value
    path = tmp_path / "debugger-replay-mutated.json"
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return path


def diagnostic_codes(path: Path) -> set[str]:
    return {diagnostic.code for diagnostic in validate_replay_path(path).diagnostics}


def test_debugger_integration_fixture_validates_lldb_commands_and_source_backed_steps() -> None:
    result = validate_replay_path(FIXTURE)

    assert result.ok is True
    assert result.diagnostics == ()

    plan = generate_stepping_plan_path(FIXTURE)
    assert plan["ok"] is True
    assert plan["contract_id"] == "objc3c.debugger-integration.stepping-plan.v1"
    assert {step["step_kind"] for step in plan["steps"]} == {
        "statement",
        "function",
        "method",
        "message-send",
        "property-access",
        "runtime-helper-call",
    }
    assert {step["step_operation"] for step in plan["steps"]} == {"step-in", "step-over", "step-out"}
    assert {
        "method-call",
        "property-accessor",
        "category-method",
        "protocol-method-body",
        "reflection-probe-call",
    } <= {step["runtime_context_kind"] for step in plan["steps"]}
    for step in plan["steps"]:
        assert step["source_file"] == "tests/tooling/fixtures/developer_tooling/debug_source_maps/source.objc3"
        assert step["source_span_id"].startswith("span.")
        assert step["statement_unit_id"].startswith("stmt.unit.")
        assert step["runtime_context_id"].startswith("runtime.anchor.")
        assert step["artifact_scope"] == "public-production-artifact"
        assert step["source_line"] > 0
        assert step["source_column"] > 0
        assert step["source_end_line"] >= step["source_line"]
        assert step["source_end_column"] > 0
        assert step["native_line"] > 0
        assert step["object_debug_line_anchor"]
        assert step["source_map_entry_id"].startswith("smap.")
        assert step["debug_map_entry_id"].startswith("dmap.")
        assert step["source_digest"] == "c415f0827bf97aad27e7a538d7ac8a3df6b0f8b43741c013145e4c09cf990f7b"
        assert step["native_line_table_row_id"].startswith("lt.")
        assert step["native_debug_info_evidence_id"] == "debugmaps.foundation.native-debug-info"


def test_debugger_integration_public_action_is_registered() -> None:
    action = ACTION_SPECS["validate-debugger-integration"]

    assert action.backend == "python:scripts/check_objc3c_debugger_integration.py"
    assert action.validation_tier == "repo"
    assert action.pass_through_args is True
    assert "validate-debugger-integration" in ACTION_HANDLERS


def test_debugger_integration_rejects_stepping_without_line_table_anchor(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "native_line_table_row_id"], "")

    assert "line-table-row-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_stepping_without_source_line_anchor(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "source_line"], 0)

    assert "stepping-anchor-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_stepping_without_object_debug_anchor(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "object_debug_line_anchor"], "")

    assert "stepping-anchor-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_stepping_source_digest_drift(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "source_digest"], "0" * 64)

    assert "source-digest-stale" in diagnostic_codes(path)


def test_debugger_integration_rejects_stepping_debug_map_id_drift(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "debug_map_entry_id"], "dmap.stale")

    assert "debug-map-entry-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_unsupported_step_operation(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "step_operation"], "reverse-step")

    assert "stepping-operation-unsupported" in diagnostic_codes(path)


def test_debugger_integration_rejects_missing_step_operation_coverage(tmp_path: Path) -> None:
    payload = load_json()
    payload["stepping_plan"]["records"] = [
        record for record in payload["stepping_plan"]["records"] if record.get("step_operation") != "step-in"
    ]
    path = tmp_path / "debugger-replay-no-step-in.json"
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    assert "stepping-operation-coverage-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_missing_object_model_step_context(tmp_path: Path) -> None:
    payload = load_json()
    payload["stepping_plan"]["records"] = [
        record
        for record in payload["stepping_plan"]["records"]
        if record.get("runtime_context_kind") != "reflection-probe-call"
    ]
    path = tmp_path / "debugger-replay-no-reflection-step.json"
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    assert "stepping-object-model-context-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_private_snapshot_only_stepping_evidence(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "artifact_scope"], "private-testing-snapshot")

    assert "private-snapshot-only-evidence" in diagnostic_codes(path)


def test_debugger_integration_rejects_step_command_kind_drift(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "lldb_command_id"], "step-method")

    assert "stepping-command-mismatch" in diagnostic_codes(path)


def test_debugger_integration_fails_closed_for_unsupported_debug_configuration(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["debug_configuration", "optimization"], "release-optimized")
    codes = diagnostic_codes(path)

    assert "debug-configuration-unsupported" in codes
    assert "stepping-debug-config-unsupported" in codes


def test_debugger_integration_rejects_artifact_digest_drift(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["debug_configuration", "artifact_digest_status"], "stale-object")
    codes = diagnostic_codes(path)

    assert "debug-configuration-unsupported" in codes
    assert "stepping-debug-config-unsupported" in codes


def test_debugger_integration_rejects_unsupported_lldb_command(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["lldb_plugin", "commands", 10, "plugin_command"], "objc3 step time-travel")

    assert "lldb-command-unsupported" in diagnostic_codes(path)


def test_debugger_integration_rejects_unsupported_record_that_claims_stepping(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 10, "claims_stepping"], True)

    assert "stepping-overclaimed" in diagnostic_codes(path)


def test_debugger_integration_rejects_generated_only_step_maps(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "source_map_entry_id"], "smap.generated_accessor")

    assert "lldb-protocol-generated-only-map" in diagnostic_codes(path)


def test_debugger_integration_rejects_stale_compiler_id(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["protocol_contract", "compiler_identity_id"], "objc3c.compiler.stale")

    assert "lldb-protocol-stale-compiler-id" in diagnostic_codes(path)


def test_debugger_integration_rejects_missing_inline_frame_chain(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["protocol_contract", "required_inline_frame_chain_ids"], ["inline.chain.missing"])

    assert "lldb-protocol-inline-frame-chain-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_protocol_without_native_debug_step_over(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["protocol_contract", "requires_step_over_emitted_native_debug_info"], False)

    assert "lldb-protocol-step-over-native-debug-info-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_optimized_step_without_transform_map(tmp_path: Path) -> None:
    path = mutated_fixture(
        tmp_path,
        ["stepping_plan", "records", 10, "optimization_transform_source_map_entry_id"],
        "smap.message_send",
    )

    assert "optimized-transform-map-missing" in diagnostic_codes(path)


def test_debugger_integration_rejects_unsupported_runtime_metadata(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["value_inspection", "records", 0, "runtime_metadata_kind"], "private-runtime")

    assert "lldb-protocol-unsupported-runtime-metadata" in diagnostic_codes(path)


def test_debugger_integration_rejects_missing_native_debug_info_evidence_link(tmp_path: Path) -> None:
    path = mutated_fixture(tmp_path, ["stepping_plan", "records", 0, "native_debug_info_evidence_id"], "")

    assert "native-debug-info-evidence-missing" in diagnostic_codes(path)


def test_debugger_integration_returns_structured_diagnostic_for_invalid_json(tmp_path: Path) -> None:
    malformed = tmp_path / "malformed.json"
    malformed.write_text("{not-json", encoding="utf-8")

    result = validate_replay_path(malformed)

    assert result.ok is False
    assert "replay-json-invalid" in {diagnostic.code for diagnostic in result.diagnostics}
