from __future__ import annotations

import json
from copy import deepcopy
from pathlib import Path
from typing import Any

from scripts.objc3c_object_model_debugger_proof import validate_contract_path
import scripts.objc3c_object_model_debugger_proof.model as debugger_proof_model
from scripts.objc3c_workflow.action_catalog import ACTION_SPECS
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "object_model_closure"
    / "full_realization_combined_readiness_contract.json"
)
DEBUGGER_PROOF_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "object_model_closure"
    / "debugger_value_inspection_replay_contract.json"
)
UMBRELLA_READINESS_PATH = ROOT / "docs" / "support" / "umbrella_readiness.json"
PUBLIC_REFLECTION_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "runtime"
    / "public"
    / "objc3_runtime_reflection.h"
)
PUBLIC_REFLECTION_PROBE = (
    ROOT / "tests" / "tooling" / "runtime" / "public_runtime_reflection_api_probe.cpp"
)


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _repo_path(path: str) -> Path:
    assert not path.startswith(("tmp/", "tmp\\"))
    return ROOT / path


def _set_nested(payload: dict[str, Any], path: list[object], value: object) -> None:
    cursor: Any = payload
    for part in path[:-1]:
        cursor = cursor[part]
    cursor[path[-1]] = value


def test_full_realization_combined_contract_is_checked_source_evidence() -> None:
    contract = _read_json(CONTRACT_PATH)

    assert contract["issue"] == 8198
    assert contract["capability_id"] == "runtime.object-model.full-realization"
    assert contract["public_status"] == "reserved"
    assert contract["support_claim_published"] is False
    assert "support_claim" not in contract
    assert contract["combined_axes"] == [
        "class",
        "metaclass",
        "category",
        "protocol",
        "property",
        "ivar",
        "selector",
        "public-reflection",
        "registration-replay",
    ]

    for key in (
        "combined_positive_fixture",
        "combined_positive_fixture_meta",
        "debugger_value_inspection_contract",
        "public_reflection_contract",
        "public_reflection_probe",
        "runbook",
        "umbrella_readiness",
    ):
        assert _repo_path(str(contract[key])).is_file(), key

    for command in contract["public_commands"]:
        assert str(command).startswith("npm run objc3c -- ")
    assert "npm run objc3c -- validate-object-model-debugger-proof" in contract["public_commands"]


def test_object_model_debugger_proof_contract_links_artifacts_and_runtime_reflection() -> None:
    contract = _read_json(DEBUGGER_PROOF_CONTRACT_PATH)
    result = validate_contract_path(DEBUGGER_PROOF_CONTRACT_PATH)

    assert result.ok is True
    assert result.diagnostics == ()
    assert contract["issue"] == 8198
    assert contract["capability_id"] == "runtime.object-model.full-realization"
    assert contract["public_status"] == "reserved"
    assert contract["support_claim_published"] is False
    assert contract["public_command"] == "npm run objc3c -- validate-object-model-debugger-proof"
    assert contract["production_artifact_probe"] == {
        "contract_id": "objc3c.object_model.production_artifact_probe.v1",
        "source_fixture": (
            "tests/native/runtime/object_model/"
            "full_realization_combined_reflection_replay_contract.objc3"
        ),
        "public_command": "npm run objc3c -- validate-object-model-debugger-proof",
        "runs_canonical_frontend": True,
        "frontend_artifact_surface": "inspect-editor-tooling",
        "required_artifact_kinds": [
            "manifest",
            "ir",
            "object",
            "runtime-metadata-binary",
            "source-graph",
            "artifact-inspector",
            "debug-map",
        ],
        "runtime_inventory_minimums": {
            "class_records": 2,
            "protocol_records": 1,
            "category_records": 1,
            "property_records": 4,
            "ivar_records": 2,
            "method_records": 8,
            "source_graph_nodes": 6,
            "declaration_breakpoint_anchors": 6,
        },
        "object_model_source_identity_minimums": {
            "source_map_records": 6,
            "native_line_table_rows": 6,
            "stepping_candidates": 4,
            "required_identity_kinds": [
                "class",
                "category",
                "protocol",
                "property",
                "ivar",
                "method",
            ],
        },
        "source_map_native_line_table_minimums": {
            "source_map_records": 6,
            "native_line_table_rows": 6,
            "required_identity_kinds": [
                "class",
                "category",
                "protocol",
                "property",
                "ivar",
                "method",
            ],
        },
        "debug_map_boundary": {
            "full_source_map_publication": "fail-closed",
            "statement_stepping": "fail-closed",
            "native_debug_info_evidence": "required",
            "boundary_reason": (
                "the production compiler path must publish object-model source-map "
                "records and native line-table rows from the canonical manifest now, "
                "and native debug-info failure must be tied to emitted object section "
                "and IR debug-metadata evidence before debugger stepping can open"
            ),
        },
    }
    assert set(contract["required_runtime_identity_kinds"]) == {
        "class",
        "category",
        "protocol",
        "property",
        "ivar",
        "method",
    }
    assert {
        record["value_kind"]
        for record in contract["object_model_value_inspection_records"]
    } >= {
        "class-metadata",
        "category-metadata",
        "protocol-metadata",
        "property-metadata",
        "ivar-layout",
        "method-metadata",
        "selector-metadata",
    }
    assert {
        link["runtime_identity_kind"]
        for link in contract["artifact_runtime_reflection_links"]
    } >= set(contract["required_runtime_identity_kinds"])
    assert contract["reflection_abi_governance"][
        "expected_debug_anchor_abi_version"
    ] == 2
    assert contract["reflection_abi_governance"][
        "expected_debug_anchor_min_reader_abi_version"
    ] == 2
    assert contract["artifact_inspector_compatibility"][
        "required_runtime_inventory_reflection_abi_version"
    ] == "manifest-derived-runtime-metadata"
    assert contract["artifact_inspector_compatibility"][
        "object_model_source_map_native_line_table"
    ] == "required"
    assert {
        anchor["runtime_identity_kind"]
        for anchor in contract["source_backed_debug_anchors"]
    } == set(contract["required_runtime_identity_kinds"])
    assert {
        anchor["runtime_anchor_id"]
        for anchor in contract["source_backed_debug_anchors"]
    } == {
        "runtime.anchor.object_model.class",
        "runtime.anchor.object_model.category",
        "runtime.anchor.object_model.protocol",
        "runtime.anchor.object_model.property",
        "runtime.anchor.object_model.ivar",
        "runtime.anchor.object_model.method",
    }


def _fake_production_artifacts(
    *,
    debug_map_overrides: dict[str, Any] | None = None,
) -> debugger_proof_model.ProductionProbeArtifacts:
    source_path = (
        "tests/native/runtime/object_model/"
        "full_realization_combined_reflection_replay_contract.objc3"
    )
    existing_path = source_path
    source_identity = _fake_object_model_source_identity(source_path)
    native_debug_info_evidence = source_identity["native_debug_info_evidence"]
    debug_map = {
        "contract_id": "objc3c.developer.tooling.debug.map.surface.v1",
        "supported": True,
        "object_artifact_present": True,
        "source_map_supported": False,
        "statement_level_stepping": False,
        "declaration_breakpoint_anchor_count": 6,
        "native_debug_info_evidence": native_debug_info_evidence,
        "object_model_source_identity": source_identity,
    }
    debug_map.update(debug_map_overrides or {})
    return debugger_proof_model.ProductionProbeArtifacts(
        summary={
            "success": True,
            "status": 0,
            "input_path": source_path,
            "paths": {
                "manifest": existing_path,
                "ir": existing_path,
                "object": existing_path,
                "runtime_metadata_binary": existing_path,
            },
        },
        manifest={
            "source": source_path,
            "interfaces": [{"name": "RuntimeFullRoot"}, {"name": "RuntimeFullWidget"}],
            "protocols": [{"name": "RuntimeFullTraceable"}],
            "categories": [{"class_name": "RuntimeFullWidget", "category_name": "ReplayReflection"}],
            "runtime_metadata_source_records": {
                "deterministic": True,
                "properties": [{}, {}, {}, {}],
                "ivars": [{}, {}],
                "methods": [{}, {}, {}, {}, {}, {}, {}, {}],
            },
        },
        source_graph={
            "contract_id": "objc3c.developer.tooling.source.graph.v1",
            "available": True,
            "source_graph_digest": "a" * 64,
            "source_path": source_path,
            "node_count": 6,
        },
        artifact_inspector={
            "contract_id": "objc3c.developer.tooling.artifact.inspector.v1",
            "source_path": source_path,
            "supported": True,
            "support_class": "compile-artifact-inspector",
            "inventory_validation": {"inventory_ready": True, "fail_closed": False},
            "runtime_inventory": {
                "available": True,
                "reflection_abi_version": "manifest-derived-runtime-metadata",
                "class_record_count": 2,
                "protocol_record_count": 1,
                "category_record_count": 1,
                "property_record_count": 4,
                "method_record_count": 8,
            },
            "artifact_links": {
                "manifest_link": existing_path,
                "ir_link": existing_path,
                "runtime_metadata_link": existing_path,
                "source_graph_link": existing_path,
                "debug_map_link": existing_path,
            },
        },
        debug_map=debug_map,
    )


def _fake_object_model_source_identity(source_path: str) -> dict[str, Any]:
    native_debug_info_evidence = {
        "contract_id": "objc3c.object_model.production.native_debug_info_evidence.v1",
        "evidence_id": "object-model.native-debug-info.production-object-section-probe",
        "source_model": "emitted-object-section-inventory-and-ir-debug-metadata-probe",
        "object_artifact_present": True,
        "object_path": "tmp/artifacts/module.obj",
        "object_format": "coff",
        "object_sha256": "a" * 64,
        "object_section_inventory_command": "llvm-readobj --sections tmp/artifacts/module.obj",
        "object_section_names": [".text", ".rdata", ".pdata", ".xdata"],
        "native_debug_sections": [],
        "native_line_table_sections": [],
        "native_debug_section_count": 0,
        "native_line_table_section_count": 0,
        "ir_path": "tmp/artifacts/module.ll",
        "ir_debug_metadata_model": "no-llvm-di-debug-locations",
        "llvm_debug_metadata_present": False,
        "llvm_debug_location_count": 0,
        "emitted_native_debug_info_supported": False,
        "native_line_table_supported": False,
        "statement_stepping_supported": False,
        "fail_closed": True,
        "fail_closed_reason": "native object lacks debug info and debug line-table sections",
        "blocked_by": [
            "native-object-lacks-debug-info-section",
            "native-object-lacks-debug-line-section",
            "compiler-ir-lacks-llvm-di-locations",
            "runtime-debug-trace-statement-stepping-integration",
        ],
    }
    identity_kinds = [
        "class",
        "category",
        "protocol",
        "property",
        "ivar",
        "method",
        "method",
        "method",
        "method",
    ]
    source_map_records = []
    native_line_table_rows = []
    for index, identity_kind in enumerate(identity_kinds):
        source_map_record_id = f"fake.source-map.{identity_kind}.{index}"
        row_id = f"fake.native-line-table.{identity_kind}.{index}"
        source_map_records.append(
            {
                "source_map_record_id": source_map_record_id,
                "runtime_identity_kind": identity_kind,
                "source_path": source_path,
                "line": index + 1,
                "column": 1,
                "native_line_table_row_id": row_id,
            }
        )
        native_line_table_rows.append(
            {
                "row_id": row_id,
                "source_map_record_id": source_map_record_id,
                "runtime_identity_kind": identity_kind,
                "source_path": source_path,
                "line": index + 1,
                "column": 1,
                "native_debug_info_emitted": False,
                "native_debug_info_evidence_id": native_debug_info_evidence["evidence_id"],
                "native_line_table_evidence_id": native_debug_info_evidence["evidence_id"],
                "native_line_table_emitted": False,
                "native_debug_info_blocker": native_debug_info_evidence["fail_closed_reason"],
            }
        )
    stepping_candidates = [
        {
            "source_map_record_id": record["source_map_record_id"],
            "native_line_table_row_id": record["native_line_table_row_id"],
            "status": "source-identity-ready-stepping-blocked",
            "native_debug_info_evidence_id": native_debug_info_evidence["evidence_id"],
            "native_debug_info_emitted": False,
            "native_line_table_emitted": False,
            "native_debug_info_blocker": native_debug_info_evidence["fail_closed_reason"],
            "blocked_by": native_debug_info_evidence["blocked_by"],
        }
        for record in source_map_records
        if record["runtime_identity_kind"] == "method"
    ]
    source_map_publication = {
        "contract_id": "objc3c.object_model.production.source_map_native_line_table.v1",
        "supported": True,
        "source_path": source_path,
        "source_graph_digest": "a" * 64,
        "publication_model": "canonical-frontend-manifest-source-map-native-line-table",
        "source_map_publication_supported": True,
        "native_line_table_publication_supported": True,
        "emitted_native_debug_info_supported": False,
        "statement_stepping_supported": False,
        "native_debug_info_evidence": native_debug_info_evidence,
        "source_map_record_count": len(source_map_records),
        "native_line_table_row_count": len(native_line_table_rows),
        "source_map_record_ids": [
            record["source_map_record_id"] for record in source_map_records
        ],
        "native_line_table_row_ids": [
            row["row_id"] for row in native_line_table_rows
        ],
        "required_identity_kinds_present": [
            "class",
            "category",
            "protocol",
            "property",
            "ivar",
            "method",
        ],
        "fail_closed_boundaries": [
            {
                "capability_id": "emittedNativeDebugInfo",
                "status": "reserved",
                "fail_closed": True,
            },
            {
                "capability_id": "statementLevelStepping",
                "status": "reserved",
                "fail_closed": True,
            },
        ],
    }
    return {
        "contract_id": "objc3c.object_model.production.source_identity.v1",
        "supported": True,
        "source_path": source_path,
        "source_graph_digest": "a" * 64,
        "runs_on_canonical_frontend_manifest": True,
        "source_map_records_supported": True,
        "native_line_table_projection_supported": True,
        "source_map_publication_supported": True,
        "native_line_table_publication_supported": True,
        "method_stepping_candidates_supported": True,
        "full_source_map_publication": False,
        "runtime_debug_trace_statement_stepping": False,
        "native_debug_info_emitted": False,
        "native_debug_info_evidence_id": native_debug_info_evidence["evidence_id"],
        "native_debug_info_evidence": native_debug_info_evidence,
        "native_debug_info_fail_closed_reason": native_debug_info_evidence["fail_closed_reason"],
        "source_map_record_count": len(source_map_records),
        "native_line_table_row_count": len(native_line_table_rows),
        "stepping_candidate_count": len(stepping_candidates),
        "required_identity_kinds_present": [
            "class",
            "category",
            "protocol",
            "property",
            "ivar",
            "method",
        ],
        "source_map_records": source_map_records,
        "native_line_table_rows": native_line_table_rows,
        "source_map_native_line_table_publication": source_map_publication,
        "stepping_candidates": stepping_candidates,
    }


def test_object_model_debugger_proof_validates_production_artifact_probe(
    monkeypatch: Any,
) -> None:
    monkeypatch.setattr(
        debugger_proof_model,
        "_build_production_probe_artifacts",
        lambda probe: _fake_production_artifacts(),
    )

    result = debugger_proof_model.validate_contract_path(
        DEBUGGER_PROOF_CONTRACT_PATH,
        run_production_probe=True,
    )

    assert result.ok is True
    assert result.diagnostics == ()


def test_object_model_debugger_proof_rejects_production_debug_map_overclaim(
    monkeypatch: Any,
) -> None:
    monkeypatch.setattr(
        debugger_proof_model,
        "_build_production_probe_artifacts",
        lambda probe: _fake_production_artifacts(
            debug_map_overrides={"source_map_supported": True}
        ),
    )

    diagnostics = debugger_proof_model.validate_contract_path(
        DEBUGGER_PROOF_CONTRACT_PATH,
        run_production_probe=True,
    ).diagnostics

    assert "production-debug-map-overclaimed" in {
        diagnostic.code for diagnostic in diagnostics
    }


def test_object_model_debugger_proof_rejects_missing_production_source_identity(
    monkeypatch: Any,
) -> None:
    monkeypatch.setattr(
        debugger_proof_model,
        "_build_production_probe_artifacts",
        lambda probe: _fake_production_artifacts(
            debug_map_overrides={"object_model_source_identity": {}}
        ),
    )

    diagnostics = debugger_proof_model.validate_contract_path(
        DEBUGGER_PROOF_CONTRACT_PATH,
        run_production_probe=True,
    ).diagnostics

    assert "production-source-identity-missing" in {
        diagnostic.code for diagnostic in diagnostics
    }


def test_object_model_debugger_proof_rejects_missing_production_source_map_publication(
    monkeypatch: Any,
) -> None:
    source_identity = _fake_object_model_source_identity(
        "tests/native/runtime/object_model/"
        "full_realization_combined_reflection_replay_contract.objc3"
    )
    source_identity.pop("source_map_native_line_table_publication")
    monkeypatch.setattr(
        debugger_proof_model,
        "_build_production_probe_artifacts",
        lambda probe: _fake_production_artifacts(
            debug_map_overrides={"object_model_source_identity": source_identity}
        ),
    )

    diagnostics = debugger_proof_model.validate_contract_path(
        DEBUGGER_PROOF_CONTRACT_PATH,
        run_production_probe=True,
    ).diagnostics

    assert "production-source-map-publication-missing" in {
        diagnostic.code for diagnostic in diagnostics
    }


def test_object_model_debugger_proof_rejects_production_source_map_publication_overclaim(
    monkeypatch: Any,
) -> None:
    source_identity = _fake_object_model_source_identity(
        "tests/native/runtime/object_model/"
        "full_realization_combined_reflection_replay_contract.objc3"
    )
    source_identity["source_map_native_line_table_publication"][
        "statement_stepping_supported"
    ] = True
    monkeypatch.setattr(
        debugger_proof_model,
        "_build_production_probe_artifacts",
        lambda probe: _fake_production_artifacts(
            debug_map_overrides={"object_model_source_identity": source_identity}
        ),
    )

    diagnostics = debugger_proof_model.validate_contract_path(
        DEBUGGER_PROOF_CONTRACT_PATH,
        run_production_probe=True,
    ).diagnostics

    assert "production-source-map-publication-overclaimed" in {
        diagnostic.code for diagnostic in diagnostics
    }


def test_object_model_debugger_proof_public_action_is_registered() -> None:
    action = ACTION_SPECS["validate-object-model-debugger-proof"]

    assert action.backend == "python:scripts/check_objc3c_object_model_debugger_proof.py"
    assert action.validation_tier == "repo"
    assert action.pass_through_args is True
    assert "validate-object-model-debugger-proof" in ACTION_HANDLERS


def test_object_model_debugger_proof_rejects_link_drift(tmp_path: Path) -> None:
    contract = _read_json(DEBUGGER_PROOF_CONTRACT_PATH)

    for case in contract["negative_cases"]:
        mutated = deepcopy(contract)
        _set_nested(mutated, case["mutation_path"], "object-model-debugger-proof-drift")
        path = tmp_path / f"{case['case_id']}.json"
        path.write_text(json.dumps(mutated, indent=2) + "\n", encoding="utf-8")

        diagnostics = validate_contract_path(path).diagnostics
        assert case["expected_code"] in {diagnostic.code for diagnostic in diagnostics}


def test_combined_fixture_covers_object_model_reflection_and_replay_axes() -> None:
    contract = _read_json(CONTRACT_PATH)
    fixture_path = _repo_path(str(contract["combined_positive_fixture"]))
    fixture_meta = _read_json(_repo_path(str(contract["combined_positive_fixture_meta"])))
    fixture = fixture_path.read_text(encoding="utf-8")

    assert fixture_meta["owner_phase"] == "runtime"
    assert fixture_meta["fixture_kind"] == "positive"
    assert fixture_meta["boundary"]["support_claim_published"] is False
    for token in (
        "@protocol RuntimeFullTraceable",
        "@interface RuntimeFullRoot",
        "+ (i32) rootTypeValue",
        "@interface RuntimeFullWidget : RuntimeFullRoot <RuntimeFullTraceable>",
        "@property (nonatomic, getter=value, setter=setValue:) i32 value",
        "@property (readonly, getter=token) id token",
        "@interface RuntimeFullWidget (ReplayReflection)",
        "- (i32) replayValue",
    ):
        assert token in fixture


def test_public_reflection_probe_preserves_lifetime_and_source_boundaries() -> None:
    contract = _read_json(CONTRACT_PATH)
    probe = PUBLIC_REFLECTION_PROBE.read_text(encoding="utf-8")
    header = PUBLIC_REFLECTION_HEADER.read_text(encoding="utf-8")
    lifetime = contract["reflection_lifetime_contract"]

    assert lifetime["snapshot_ownership"] == "caller-owned snapshot structs"
    assert lifetime["string_lifetime"] == "runtime-owned borrowed strings"
    assert lifetime["state_source"] == "runtime-owned realized state"
    assert "owner_identity;" not in header
    assert "const char *" in header
    assert "objc3_runtime_reflection_state_snapshot state{};" in probe
    assert "objc3_runtime_reflection_class_snapshot widget_class{};" in probe
    assert "objc3_runtime_reflection_property_snapshot value_property{};" in probe
    assert "objc3_runtime_reflection_selector_snapshot indexed_selector{};" in probe
    assert "objc3_runtime_stage_registration_table_for_bootstrap" in probe
    assert "objc3_runtime_register_image(&fixture.image)" in probe

    for source_anchor in contract["source_anchors"]:
        path = _repo_path(str(source_anchor["path"]))
        assert path.is_file(), source_anchor


def test_negative_boundaries_remain_non_public_and_fail_closed() -> None:
    contract = _read_json(CONTRACT_PATH)
    probe = PUBLIC_REFLECTION_PROBE.read_text(encoding="utf-8")
    header = PUBLIC_REFLECTION_HEADER.read_text(encoding="utf-8")

    boundaries = {
        str(boundary["boundary_id"]): boundary
        for boundary in contract["negative_boundaries"]
    }
    assert set(boundaries) == {
        "private-snapshots-not-public-reflection",
        "malformed-metadata-fails-closed",
        "stale-generation-replay-does-not-promote-support",
    }

    assert "_for_testing" not in probe
    for private_symbol in (
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "objc3_runtime_copy_selector_lookup_entry_for_testing",
    ):
        assert private_symbol not in header

    assert "OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA" in header
    assert "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY" in header

    for boundary in boundaries.values():
        for evidence_path in boundary["evidence"]:
            assert _repo_path(str(evidence_path)).exists(), evidence_path


def test_umbrella_readiness_references_combined_evidence_without_closing_row() -> None:
    readiness = _read_json(UMBRELLA_READINESS_PATH)
    entries = {
        str(entry["umbrella_capability_id"]): entry
        for entry in readiness["entries"]
    }
    entry = entries["runtime.object-model.full-realization"]

    assert entry["current_state"] == "reserved"
    assert entry["readiness_state"] == "blocked"
    assert {
        blocker["blocker_id"] for blocker in entry["promotion_blockers"]
    } == {"object-model-debugger-source-identity"}

    required_paths = {
        requirement.get("path")
        for field in (
            "required_source_anchors",
            "required_positive_fixtures",
            "required_negative_fixtures",
            "required_runtime_probes",
            "required_docs",
        )
        for requirement in entry[field]
        if requirement.get("status") == "satisfied"
    }
    assert str(CONTRACT_PATH.relative_to(ROOT)).replace("\\", "/") in required_paths
    assert (
        "tests/native/runtime/object_model/"
        "full_realization_combined_reflection_replay_contract.objc3"
    ) in required_paths
    assert (
        "tests/tooling/fixtures/object_model_closure/"
        "debugger_value_inspection_replay_contract.json"
    ) in required_paths
    assert "tests/tooling/runtime/public_runtime_reflection_api_probe.cpp" in required_paths

    required_commands = {
        requirement.get("command")
        for requirement in entry["required_public_commands"]
        if requirement.get("status") == "satisfied"
    }
    assert "npm run objc3c -- validate-object-model-debugger-proof" in required_commands
