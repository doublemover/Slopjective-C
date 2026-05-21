from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.artifact_inspector import build_artifact_inspector_payload
from objc3c_editor_tooling.input_loading import EditorToolingInputs
from objc3c_editor_tooling.model import extract_symbols
from objc3c_editor_tooling.paths import EditorToolingPaths, EditorToolingSource


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
    assert payload["support_class"] == "compile-artifact-inspector"
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
    assert payload["runtime_imports"]["available"] is True
    assert payload["runtime_imports"]["runtime_metadata_binary_present"] is True
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
    object_path.write_bytes(b"objc3-object-fixture")
    inputs_with_object = EditorToolingInputs(
        summary={
            **inputs.summary,
            "runtime_inspector": {
                "contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
                "available": True,
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
    assert payload["object"]["object_symbol_inventory_command"] == "llvm-nm module.obj"
    assert payload["object"]["object_section_inventory_command"] == "llvm-objdump -h module.obj"
    assert payload["inspection_commands"]["object_symbols"] == "llvm-nm module.obj"
    assert payload["inspection_commands"]["object_sections"] == "llvm-objdump -h module.obj"
