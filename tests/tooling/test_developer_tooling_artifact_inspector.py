from __future__ import annotations

import json
import hashlib
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.artifact_inspector import build_artifact_inspector_payload
from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.model import extract_symbols
from objc3c_editor_tooling.paths import EditorToolingPaths, EditorToolingSource
from scripts.objc3c_workflow.action_catalog_developer_inspection import DEVELOPER_INSPECTION_ACTION_SPECS
from scripts.objc3c_workflow.action_handlers_developer_inspection import DEVELOPER_INSPECTION_ACTION_HANDLERS


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"
CONTRACT_PATH = FIXTURE_ROOT / "artifact_inspector_implementation_contract.json"


def load_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def fixture_paths(contract: dict[str, object]) -> EditorToolingPaths:
    source_path = ROOT / str(contract["source_path"])
    source = EditorToolingSource(
        path=source_path,
        display_path=str(contract["source_path"]),
        slug="artifact-inspector-fixture",
    )
    return EditorToolingPaths(
        source=source,
        report_dir=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture",
        artifact_dir=ROOT / "tmp" / "artifacts" / "developer-tooling" / "artifact-inspector-fixture",
        compile_summary=ROOT / str(contract["summary_path"]),
        editor_surface=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "editor-surface.json",
        language_server_capabilities=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "language-server-capabilities.json",
        navigation_index=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "navigation-index.json",
        workspace_index=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "workspace-index.json",
        source_graph=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "source-graph.json",
        artifact_inspector=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "artifact-inspector.json",
        formatter_output=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "formatter-output.json",
        formatted_source=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "formatted-source.objc3",
        debug_map=ROOT / "tmp" / "reports" / "developer-tooling" / "artifact-inspector-fixture" / "debug-map.json",
    )


def fixture_inputs(contract: dict[str, object]) -> EditorToolingInputs:
    return EditorToolingInputs(
        summary=load_json(ROOT / str(contract["summary_path"])),
        diagnostics=load_json(ROOT / str(contract["diagnostics_path"])),
        manifest=load_json(ROOT / str(contract["manifest_path"])),
        source_text=(ROOT / str(contract["source_path"])).read_text(encoding="utf-8"),
        diagnostics_path_text=str(contract["diagnostics_path"]),
        manifest_path_text=str(contract["manifest_path"]),
        object_path_text=None,
    )


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(json.dumps(payload, indent=2, sort_keys=True), encoding="utf-8")


def file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    digest.update(path.read_bytes())
    return digest.hexdigest()


def test_artifact_inspector_explains_checked_in_compiler_outputs() -> None:
    contract = load_json(CONTRACT_PATH)
    paths = fixture_paths(contract)
    inputs = fixture_inputs(contract)
    symbols = extract_symbols(inputs.manifest)
    workspace_index = {
        "available": True,
        "package_count": 2,
        "workspace_index_digest": "a" * 64,
        "package_symbols": [],
    }

    payload = build_artifact_inspector_payload(paths, inputs, symbols, workspace_index)

    assert payload["contract_id"] == "objc3c.developer.tooling.artifact.inspector.v1"
    assert payload["supported"] is True
    assert payload["support_class"] == "compile-artifact-inspector-with-fail-closed-inventory"
    assert payload["inspected_artifact_kinds"] == contract["expected_inspected_artifact_kinds"]
    assert payload["unsupported_artifact_kinds"] == contract["expected_unsupported_artifact_kinds"]
    assert [symbol["name"] for symbol in symbols] == contract["expected_symbol_names"]
    assert payload["diagnostics"]["diagnostic_codes"] == contract["expected_diagnostic_codes"]
    assert payload["diagnostics"]["severity_counts"] == {"warning": 1}
    assert payload["manifest"]["module"] == "ArtifactInspectorDemo"
    assert payload["manifest"]["declaration_count"] == 4
    assert payload["manifest"]["symbol_kind_counts"] == {
        "function": 1,
        "global": 1,
        "implementation": 1,
        "interface": 1,
    }
    assert payload["ir"]["function_definition_count"] == 1
    assert payload["ir"]["external_declaration_count"] == 1
    assert payload["ir"]["global_record_count"] == 1
    assert payload["source_graph"]["available"] is True
    assert payload["source_graph"]["declaration_node_count"] == 4
    assert payload["source_graph"]["workspace_package_count"] == 2
    assert len(payload["source_graph"]["source_graph_digest"]) == 64
    assert payload["object"]["available"] is False
    assert payload["object"]["inspection_ready"] is False
    assert payload["object"]["retired_route_reason"] == "compile summary did not publish a object artifact path"
    assert payload["runtime_imports"]["available"] is True
    assert payload["runtime_imports"]["runtime_metadata_binary_present"] is True
    assert payload["runtime_inventory"]["available"] is False
    assert payload["inventory_validation"]["inventory_ready"] is False
    assert payload["inventory_validation"]["fail_closed"] is True
    assert "missing object artifact" in payload["inventory_validation"]["fail_closed_reasons"]
    assert "missing runtime inventory" in payload["inventory_validation"]["fail_closed_reasons"]
    assert [
        entry["symbol"] for entry in payload["runtime_imports"]["runtime_imports"]
    ] == contract["required_runtime_import_symbols"]
    assert sorted(payload["inspection_commands"]) == contract["expected_inspection_command_keys"]


def test_artifact_inspector_only_publishes_object_commands_for_real_object_paths(
    tmp_path: Path,
) -> None:
    contract = load_json(CONTRACT_PATH)
    paths = fixture_paths(contract)
    inputs = fixture_inputs(contract)
    object_path = tmp_path / "module.obj"
    object_path.write_bytes(b"\x64\x86objc3-object-fixture")
    object_digest = file_sha256(object_path)
    inputs_with_object = EditorToolingInputs(
        summary={
            **inputs.summary,
            "runtime_inspector": {
                "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                "available": True,
                "object_inventory": {
                    "sha256": object_digest,
                    "symbols": [
                        {"name": "_main", "binding": "exported"},
                        {"name": "objc3_runtime_bootstrap", "binding": "imported"},
                    ],
                    "sections": [{"name": ".text"}, {"name": ".objc3_runtime"}],
                },
                "dump_commands": {
                    "object_symbols": "llvm-nm module.obj",
                    "object_sections": "llvm-objdump -h module.obj",
                },
            },
        },
        diagnostics=inputs.diagnostics,
        manifest=inputs.manifest,
        source_text=inputs.source_text,
        diagnostics_path_text=inputs.diagnostics_path_text,
        manifest_path_text=inputs.manifest_path_text,
        object_path_text=str(object_path),
    )

    payload = build_artifact_inspector_payload(
        paths,
        inputs_with_object,
        extract_symbols(inputs.manifest),
        {"available": False},
    )

    assert payload["object"]["available"] is True
    assert payload["object"]["inspection_ready"] is True
    assert payload["object"]["object_format"] == "coff"
    assert payload["object"]["digest_matches"] is True
    assert payload["object"]["symbol_count"] == 2
    assert payload["object"]["imported_runtime_helper_count"] == 1
    assert payload["object"]["object_symbol_inventory_command"] == "llvm-nm module.obj"
    assert payload["object"]["object_section_inventory_command"] == "llvm-objdump -h module.obj"
    assert payload["inspection_commands"]["object_symbols"] == "llvm-nm module.obj"
    assert payload["inspection_commands"]["object_sections"] == "llvm-objdump -h module.obj"


def test_artifact_inspector_extracts_object_runtime_package_and_link_inventory(
    tmp_path: Path,
) -> None:
    contract = load_json(CONTRACT_PATH)
    paths = fixture_paths(contract)
    inputs = fixture_inputs(contract)
    object_path = tmp_path / "module.obj"
    object_path.write_bytes(b"\x64\x86objc3-object-fixture")
    object_digest = file_sha256(object_path)
    receipt_path = tmp_path / "objc3c-install-receipt.json"
    write_json(
        receipt_path,
        {
            "operation": "install",
            "package_id": "source:artifact-inspector-demo",
            "trust": {"status": "trusted"},
        },
    )
    receipt_digest = file_sha256(receipt_path)
    summary = {
        **inputs.summary,
        "package": {
            "package_id": "source:artifact-inspector-demo",
            "abi_identity": "objc3-abi-v1",
        },
        "registry": {
            "registry_id": "local-registry-fixture",
            "package_id": "source:artifact-inspector-demo",
            "abi_identity": "objc3-abi-v1",
            "trust": {"status": "trusted"},
        },
        "artifact_provenance": {
            "generated": True,
            "source_truth_inputs": [str(contract["source_path"])],
        },
        "debug_map_path": "tests/tooling/fixtures/developer_tooling/runtime_debug_trace/debug-map.json",
        "optimization_trace_path": "tmp/artifacts/developer-tooling/optimization-trace.json",
        "package_operation_receipts": [
            {
                "path": str(receipt_path),
                "operation": "install",
                "package_id": "source:artifact-inspector-demo",
                "sha256": receipt_digest,
                "trust_status": "trusted",
            }
        ],
        "runtime_inspector": {
            "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
            "available": True,
            "object_inventory": {
                "sha256": object_digest,
                "symbols": [
                    {"name": "_main", "binding": "exported"},
                    {"name": "objc3_runtime_bootstrap", "binding": "exported"},
                    {"name": "_objc_msgSend", "binding": "imported"},
                ],
                "sections": [{"name": ".text"}, {"name": ".objc3_runtime"}],
            },
            "runtime_inventory": {
                "reflection_abi_version": "objc3-runtime-reflection-v1",
                "classes": [{"name": "ArtifactWidget"}],
                "selectors": [{"name": "run"}],
                "methods": [{"selector": "run"}],
                "properties": [{"name": "value"}],
                "protocols": [{"name": "ArtifactProtocol"}],
                "categories": [{"name": "ArtifactWidget(Trace)"}],
                "stdlib_helper_references": ["objc3_runtime_bootstrap"],
                "runtime_import_package_records": [
                    {"package_id": "stdlib:objc3.core", "symbol": "objc3_runtime_bootstrap"}
                ],
            },
            "dump_commands": {
                "object_symbols": "llvm-nm module.obj",
                "object_sections": "llvm-objdump -h module.obj",
            },
        },
    }
    inputs_with_inventory = EditorToolingInputs(
        summary=summary,
        diagnostics=inputs.diagnostics,
        manifest={
            **inputs.manifest,
            "package_id": "source:artifact-inspector-demo",
            "abi": {"identity": "objc3-abi-v1"},
            "trust": {"status": "trusted"},
        },
        source_text=inputs.source_text,
        diagnostics_path_text=inputs.diagnostics_path_text,
        manifest_path_text=inputs.manifest_path_text,
        object_path_text=str(object_path),
    )

    payload = build_artifact_inspector_payload(
        paths,
        inputs_with_inventory,
        extract_symbols(inputs.manifest),
        {"available": True, "package_count": 1, "workspace_index_digest": "a" * 64},
        {"available": True, "declaration_count": 1, "reference_count": 1, "source_index_digest": "b" * 64},
        {"contract_id": "objc3c.developer.tooling.source.graph.v1", "available": True, "source_graph_digest": "c" * 64},
    )

    assert payload["inventory_validation"] == {
        "inventory_ready": True,
        "fail_closed": False,
        "fail_closed_reasons": [],
        "unsupported_inventory_notes": [],
    }
    assert payload["object"]["object_format"] == "coff"
    assert payload["object"]["exported_runtime_helper_count"] == 1
    assert payload["object"]["imported_runtime_helper_count"] == 1
    assert payload["runtime_inventory"]["reflection_abi_version"] == "objc3-runtime-reflection-v1"
    assert payload["runtime_inventory"]["class_record_count"] == 1
    assert payload["runtime_inventory"]["class_records"] == [{"name": "ArtifactWidget"}]
    assert payload["runtime_inventory"]["selector_record_count"] == 1
    assert payload["runtime_inventory"]["selector_records"] == [{"name": "run"}]
    assert payload["package_inventory"]["package_identity"] == "source:artifact-inspector-demo"
    assert payload["package_inventory"]["abi_identity"] == "objc3-abi-v1"
    assert payload["package_inventory"]["package_operation_receipt_count"] == 1
    assert payload["artifact_links"]["source_graph_digest"] == "c" * 64
    assert payload["artifact_links"]["debug_map_link"].endswith("debug-map.json")


def test_artifact_inspector_fails_closed_for_stale_digest_identity_and_untrusted_receipt(
    tmp_path: Path,
) -> None:
    contract = load_json(CONTRACT_PATH)
    paths = fixture_paths(contract)
    inputs = fixture_inputs(contract)
    object_path = tmp_path / "module.obj"
    object_path.write_bytes(b"\x64\x86objc3-object-fixture")
    receipt_path = tmp_path / "objc3c-install-receipt.json"
    write_json(
        receipt_path,
        {
            "operation": "install",
            "package_id": "source:wrong-package",
            "trust": {"status": "untrusted"},
        },
    )
    payload = build_artifact_inspector_payload(
        paths,
        EditorToolingInputs(
            summary={
                **inputs.summary,
                "package": {
                    "package_id": "source:artifact-inspector-demo",
                    "abi_identity": "objc3-abi-v1",
                },
                "artifact_provenance": {"generated": True, "source_truth_inputs": []},
                "package_operation_receipts": [
                    {
                        "path": str(receipt_path),
                        "package_id": "source:wrong-package",
                        "trust_status": "untrusted",
                    }
                ],
                "runtime_inspector": {
                    "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                    "available": True,
                    "object_inventory": {
                        "sha256": "0" * 64,
                        "symbols": [{"name": "objc3_runtime_bootstrap", "binding": "exported"}],
                        "sections": [{"name": ".text"}],
                    },
                    "dump_commands": {
                        "object_symbols": "llvm-nm module.obj",
                        "object_sections": "llvm-objdump -h module.obj",
                    },
                },
            },
            diagnostics=inputs.diagnostics,
            manifest={
                **inputs.manifest,
                "package_id": "source:manifest-package",
                "abi": {"identity": "objc3-abi-v1"},
            },
            source_text=inputs.source_text,
            diagnostics_path_text=inputs.diagnostics_path_text,
            manifest_path_text=inputs.manifest_path_text,
            object_path_text=str(object_path),
        ),
        extract_symbols(inputs.manifest),
        {"available": False},
    )

    reasons = payload["inventory_validation"]["fail_closed_reasons"]
    assert "stale object digest" in reasons
    assert "missing runtime inventory" in reasons
    assert "package identity mismatch" in reasons
    assert "untrusted package receipt" in reasons
    assert "generated artifact missing provenance" in reasons
    assert payload["object"]["inspection_ready"] is False
    assert payload["package_inventory"]["identity_mismatch"] is True
    assert payload["package_inventory"]["untrusted_receipt_count"] == 1


def test_artifact_inspector_fails_closed_for_unsupported_object_format(tmp_path: Path) -> None:
    contract = load_json(CONTRACT_PATH)
    paths = fixture_paths(contract)
    inputs = fixture_inputs(contract)
    object_path = tmp_path / "module.obj"
    object_path.write_bytes(b"not-a-real-object")
    object_digest = file_sha256(object_path)

    payload = build_artifact_inspector_payload(
        paths,
        EditorToolingInputs(
            summary={
                **inputs.summary,
                "runtime_inspector": {
                    "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                    "available": True,
                    "object_inventory": {
                        "sha256": object_digest,
                        "symbols": [{"name": "objc3_runtime_bootstrap", "binding": "exported"}],
                        "sections": [{"name": ".text"}],
                    },
                    "runtime_inventory": {"reflection_abi_version": "objc3-runtime-reflection-v1"},
                    "dump_commands": {
                        "object_symbols": "llvm-nm module.obj",
                        "object_sections": "llvm-objdump -h module.obj",
                    },
                },
            },
            diagnostics=inputs.diagnostics,
            manifest=inputs.manifest,
            source_text=inputs.source_text,
            diagnostics_path_text=inputs.diagnostics_path_text,
            manifest_path_text=inputs.manifest_path_text,
            object_path_text=str(object_path),
        ),
        extract_symbols(inputs.manifest),
        {"available": False},
    )

    assert payload["object"]["object_format"] == "unsupported"
    assert payload["object"]["inspection_ready"] is False
    assert "unsupported object format" in payload["inventory_validation"]["fail_closed_reasons"]
    assert payload["inventory_validation"]["unsupported_inventory_notes"] == [
        "object bytes do not use a supported symbol-table format"
    ]


def test_artifact_inspector_normalizes_symbol_inventory_and_requires_digest(
    tmp_path: Path,
) -> None:
    contract = load_json(CONTRACT_PATH)
    paths = fixture_paths(contract)
    inputs = fixture_inputs(contract)
    object_path = tmp_path / "module.obj"
    object_path.write_bytes(b"\x64\x86objc3-object-fixture")

    payload = build_artifact_inspector_payload(
        paths,
        EditorToolingInputs(
            summary={
                **inputs.summary,
                "runtime_inspector": {
                    "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                    "available": True,
                    "object_inventory": {
                        "symbols": [
                            "                 U _objc_msgSend",
                            {"name": "objc3_runtime_bootstrap", "defined": True},
                            {"name": "_main", "binding": "global"},
                        ],
                        "sections": [{"name": ".objc3_runtime"}, {"name": ".text"}],
                    },
                    "runtime_inventory": {
                        "reflection_abi_version": "objc3-runtime-reflection-v1",
                        "class_records": [{"name": "ArtifactWidget"}],
                    },
                    "dump_commands": {
                        "object_symbols": "llvm-nm module.obj",
                        "object_sections": "llvm-objdump -h module.obj",
                    },
                },
            },
            diagnostics=inputs.diagnostics,
            manifest=inputs.manifest,
            source_text=inputs.source_text,
            diagnostics_path_text=inputs.diagnostics_path_text,
            manifest_path_text=inputs.manifest_path_text,
            object_path_text=str(object_path),
        ),
        extract_symbols(inputs.manifest),
        {"available": False},
    )

    assert payload["object"]["symbols"] == [
        "U _objc_msgSend",
        {"binding": "global", "name": "_main"},
        {"defined": True, "name": "objc3_runtime_bootstrap"},
    ]
    assert payload["object"]["exported_runtime_helper_count"] == 1
    assert payload["object"]["imported_runtime_helper_count"] == 1
    assert payload["object"]["inspection_ready"] is False
    assert "missing object digest expectation" in payload["inventory_validation"]["fail_closed_reasons"]


def test_inspect_artifact_public_action_is_registered() -> None:
    spec = DEVELOPER_INSPECTION_ACTION_SPECS["inspect-artifact"]

    assert spec.pass_through_args is True
    assert "--artifact-inspector-only" in spec.backend
    assert "inspect-artifact" in DEVELOPER_INSPECTION_ACTION_HANDLERS
