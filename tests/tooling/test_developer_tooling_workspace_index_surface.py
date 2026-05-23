from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.model import build_debug_payload  # noqa: E402
from objc3c_editor_tooling.workspace_index import build_workspace_index  # noqa: E402


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_workspace_index_rows_publish_checked_in_evidence_and_fail_closed_boundaries() -> None:
    contract = load_json(FIXTURE_ROOT / "workspace_semantic_navigation_contract.json")
    workspace_index = build_workspace_index(
        str(contract["source"]),
        "Hello",
        "tmp/artifacts/module.manifest.json",
        [{"name": "main", "kind": "function", "line": 1, "column": 1}],
    )

    package_ids = {package["package_id"] for package in workspace_index["packages"]}
    edges = {
        f"{edge['from_package_id']}->{edge['to_package_id']}"
        for edge in workspace_index["cross_package_edges"]
    }

    assert workspace_index["available"] is True
    assert workspace_index["fail_closed"] is False
    assert set(contract["expected_package_ids"]).issubset(package_ids)
    assert set(contract["expected_cross_package_edges"]).issubset(edges)
    assert all(
        workspace_index["guardrails"]["checks"][check] is True
        for check in contract["required_guardrail_checks"]
    )
    assert set(contract["source_truth_inputs"]).issubset(
        set(workspace_index["evidence_roots"])
    )
    assert workspace_index["unsupported_surfaces"] == contract["unsupported_surfaces"]


def test_workspace_index_without_manifest_fails_closed_for_workspace_symbols() -> None:
    workspace_index = build_workspace_index(
        "tests/tooling/fixtures/native/hello.objc3",
        "Hello",
        None,
        [],
    )

    assert workspace_index["available"] is False
    assert workspace_index["fail_closed"] is True
    assert workspace_index["retired_route_reason"] == (
        "compile produced no manifest-backed declaration surface"
    )


def test_debug_payload_reserves_stepping_and_full_source_map_rows() -> None:
    debug_payload = build_debug_payload(
        {
            "runtime_inspector": {
                "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                "dump_commands": {
                    "object_symbols": "llvm-nm module.obj",
                    "object_sections": "llvm-objdump -h module.obj",
                },
            }
        },
        "tmp/artifacts/module.obj",
        [{"name": "main", "kind": "function", "line": 1, "column": 1}],
    )

    reserved = {
        row["capability_id"]: row
        for row in debug_payload["reserved_capability_rows"]
    }

    assert debug_payload["supported"] is True
    assert debug_payload["artifact_inspection_ready"] is True
    assert "compile-manifest-declaration-coordinates" in debug_payload["evidence_roots"]
    assert "runtime-inspector-object-symbol-inventory" in debug_payload["evidence_roots"]
    assert reserved["statementLevelStepping"]["status"] == "reserved"
    assert reserved["statementLevelStepping"]["fail_closed"] is True
    assert reserved["fullSourceMapPublication"]["status"] == "reserved"
    assert reserved["fullSourceMapPublication"]["fail_closed"] is True


def test_debug_payload_publishes_object_model_source_identity_from_manifest() -> None:
    source_path = "tests/native/runtime/object_model/full_realization_combined_reflection_replay_contract.objc3"
    manifest = {
        "source": source_path,
        "interfaces": [{"name": "RuntimeFullWidget", "line": 28, "column": 1}],
        "categories": [
            {
                "class_name": "RuntimeFullWidget",
                "category_name": "ReplayReflection",
                "line": 53,
                "column": 1,
            }
        ],
        "protocols": [{"name": "RuntimeFullTraceable", "line": 6, "column": 1}],
        "runtime_metadata_source_records": {
            "properties": [
                {
                    "owner_name": "RuntimeFullWidget",
                    "property_name": "value",
                    "line": 29,
                    "column": 1,
                }
            ],
            "ivars": [
                {
                    "owner_name": "RuntimeFullWidget",
                    "ivar_name": "value",
                    "line": 29,
                    "column": 1,
                }
            ],
            "methods": [
                {
                    "owner_name": "RuntimeFullWidget",
                    "selector": "setValue:",
                    "line": 40,
                    "column": 1,
                }
            ],
        },
    }

    debug_payload = build_debug_payload(
        {
            "runtime_inspector": {
                "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                "dump_commands": {"object_symbols": "llvm-nm module.obj"},
            }
        },
        "tmp/artifacts/module.obj",
        [{"name": "RuntimeFullWidget", "kind": "interface", "line": 28, "column": 1}],
        manifest=manifest,
        source_graph={"source_graph_digest": "a" * 64},
        source_path=source_path,
        native_debug_info_evidence={
            "contract_id": "objc3c.object_model.production.native_debug_info_evidence.v1",
            "evidence_id": "object-model.native-debug-info.production-object-section-probe",
            "source_model": "emitted-object-section-inventory-and-ir-debug-metadata-probe",
            "object_artifact_present": True,
            "object_path": "tmp/artifacts/module.obj",
            "object_format": "coff",
            "object_sha256": "a" * 64,
            "object_section_inventory_command": "llvm-readobj --sections tmp/artifacts/module.obj",
            "object_section_names": [
                ".text",
                ".rdata",
                ".debug$S",
                ".debug_abbrev",
                ".debug_info",
                ".debug_str",
                ".debug_line",
            ],
            "native_debug_sections": [
                ".debug$S",
                ".debug_abbrev",
                ".debug_info",
                ".debug_str",
                ".debug_line",
            ],
            "native_line_table_sections": [".debug$S", ".debug_line"],
            "native_debug_section_count": 5,
            "native_line_table_section_count": 2,
            "ir_path": "tmp/artifacts/module.ll",
            "ir_debug_metadata_model": "llvm-di-metadata-present",
            "llvm_debug_metadata_present": True,
            "llvm_debug_location_count": 12,
            "emitted_native_debug_info_supported": True,
            "native_line_table_supported": True,
            "statement_stepping_supported": True,
            "statement_stepping_evidence_id": "object-model.statement-stepping.production-source-line-table",
            "fail_closed": False,
            "fail_closed_reason": "",
            "blocked_by": [],
        },
    )

    source_identity = debug_payload["object_model_source_identity"]
    assert "object-model-production-source-identity" in debug_payload["evidence_roots"]
    assert "object-model-production-source-map-native-line-table" in debug_payload["evidence_roots"]
    assert "object-model-production-statement-stepping" in debug_payload["evidence_roots"]
    assert "native-debug-info-artifact-evidence" in debug_payload["evidence_roots"]
    assert source_identity["contract_id"] == "objc3c.object_model.production.source_identity.v1"
    assert source_identity["native_debug_info_evidence"]["native_debug_section_count"] == 5
    assert source_identity["native_debug_info_evidence"]["native_line_table_section_count"] == 2
    assert source_identity["native_debug_info_fail_closed_reason"] == ""
    assert source_identity["source_map_records_supported"] is True
    assert source_identity["native_line_table_projection_supported"] is True
    assert source_identity["source_map_publication_supported"] is True
    assert source_identity["native_line_table_publication_supported"] is True
    assert debug_payload["source_map_supported"] is True
    assert debug_payload["statement_level_stepping"] is True
    assert source_identity["runtime_debug_trace_statement_stepping"] is True
    assert {
        candidate["status"]
        for candidate in source_identity["stepping_candidates"]
    } == {"runtime-debug-trace-statement-stepping-supported"}
    assert {
        tuple(candidate["blocked_by"])
        for candidate in source_identity["stepping_candidates"]
    } == {()}
    publication = source_identity["source_map_native_line_table_publication"]
    assert publication["contract_id"] == (
        "objc3c.object_model.production.source_map_native_line_table.v1"
    )
    assert publication["publication_model"] == (
        "canonical-frontend-manifest-source-map-native-line-table"
    )
    assert publication["source_map_publication_supported"] is True
    assert publication["native_line_table_publication_supported"] is True
    assert publication["emitted_native_debug_info_supported"] is True
    assert publication["statement_stepping_supported"] is True
    assert publication["statement_stepping_evidence_id"] == (
        "object-model.statement-stepping.production-source-line-table"
    )
    assert publication["native_debug_info_evidence"]["blocked_by"] == []
    assert set(publication["source_map_record_ids"]) == {
        record["source_map_record_id"]
        for record in source_identity["source_map_records"]
    }
    assert set(publication["native_line_table_row_ids"]) == {
        row["row_id"]
        for row in source_identity["native_line_table_rows"]
    }
    assert set(source_identity["required_identity_kinds_present"]) == {
        "class",
        "metaclass",
        "category",
        "protocol",
        "property",
        "ivar",
        "selector",
        "method",
        "reflection",
        "replay",
    }
    records_by_kind = {
        record["runtime_identity_kind"]: record
        for record in source_identity["source_map_records"]
    }
    assert records_by_kind["metaclass"]["display_name"] == "RuntimeFullWidget.metaclass"
    assert records_by_kind["selector"]["source_map_record_kind"] == "message-send"
    assert records_by_kind["reflection"]["display_name"].startswith("reflection:")
    assert records_by_kind["replay"]["display_name"] == (
        "registration-replay:RuntimeFullWidget(ReplayReflection)"
    )
    assert {
        row["expected_native_symbol"]
        for row in source_identity["native_line_table_rows"]
        if row["runtime_identity_kind"] == "method"
    } == {"objc3_method_RuntimeFullWidget_instance_setValue_"}
    assert {
        row["native_debug_info_blocker"]
        for row in source_identity["native_line_table_rows"]
    } == {""}
    assert {
        row["native_debug_info_emitted"]
        for row in source_identity["native_line_table_rows"]
    } == {True}
    assert {
        row["native_line_table_emitted"]
        for row in source_identity["native_line_table_rows"]
    } == {True}
