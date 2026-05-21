from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.model import extract_symbols  # noqa: E402
from objc3c_editor_tooling.source_index import build_source_graph, build_source_index  # noqa: E402
from objc3c_editor_tooling.workspace_index import build_workspace_index  # noqa: E402
from objc3c_language_service import ObjectiveC3LanguageService, replay_requests  # noqa: E402
from objc3c_tooling.json_io import load_json_object  # noqa: E402
from scripts.objc3c_workflow.action_handler_lookup import registered_action_handler  # noqa: E402
from scripts.objc3c_workflow.public_command_api import public_workflow_action_payload  # noqa: E402


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"
SOURCE_GRAPH_ROOT = FIXTURE_ROOT / "source_graph"
LANGUAGE_SERVICE_FIXTURE = FIXTURE_ROOT / "language_service" / "request_replay.json"


def build_fixture_service() -> tuple[str, ObjectiveC3LanguageService]:
    contract = load_json_object(SOURCE_GRAPH_ROOT / "source_graph_contract.json")
    manifest = load_json_object(SOURCE_GRAPH_ROOT / "manifest.json")
    source_path = str(contract["source_path"])
    source_text = (ROOT / source_path).read_text(encoding="utf-8")
    module_name = str(contract["module_name"])
    manifest_path = str(contract["manifest_path"])
    symbols = extract_symbols(manifest)
    source_index = build_source_index(
        source_path=source_path,
        module_name=module_name,
        source_text=source_text,
        manifest_path=manifest_path,
        symbols=symbols,
        diagnostics=[],
        artifact_paths={},
    )
    workspace_index = build_workspace_index(
        source_path,
        module_name,
        manifest_path,
        symbols,
        source_index,
    )
    source_graph = build_source_graph(
        source_path=source_path,
        module_name=module_name,
        manifest_path=manifest_path,
        manifest=manifest,
        symbols=symbols,
        source_index=source_index,
        workspace_index=workspace_index,
    )
    return source_path, ObjectiveC3LanguageService(
        source_graph=source_graph,
        source_texts={source_path: source_text},
    )


def response_by_method(replay: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(response["method"]): response["result"]
        for response in replay["responses"]
        if isinstance(response, dict)
    }


def test_language_service_replay_serves_source_graph_backed_editor_features() -> None:
    _, service = build_fixture_service()
    fixture = load_json_object(LANGUAGE_SERVICE_FIXTURE)
    replay = replay_requests(service, fixture["requests"])
    responses = response_by_method(replay)

    initialize = responses["initialize"]
    assert initialize["contract_id"] == "objc3c.language_service.core.v1"
    assert initialize["capabilities"]["definitionProvider"] is True
    assert initialize["capabilities"]["referencesProvider"] is False
    assert initialize["capabilities"]["renameProvider"] is False

    assert responses["textDocument/publishDiagnostics"]["diagnostic_count"] >= 3
    assert responses["textDocument/documentSymbol"]["symbol_count"] == 2
    assert [symbol["name"] for symbol in responses["textDocument/documentSymbol"]["symbols"]] == [
        "globalCounter",
        "main",
    ]
    assert responses["textDocument/definition"]["supported"] is True
    assert responses["textDocument/definition"]["target"]["compiler_range"]["line"] == 1
    assert responses["textDocument/hover"]["contents"]["value"] == "global globalCounter"
    assert responses["workspace/symbol"]["symbol_count"] >= 1


def test_language_service_fail_closed_boundaries_and_invalidation_are_explicit() -> None:
    _, service = build_fixture_service()
    fixture = load_json_object(LANGUAGE_SERVICE_FIXTURE)
    replay = replay_requests(service, fixture["requests"])
    responses = response_by_method(replay)

    for method in fixture["expected_fail_closed_methods"]:
        assert responses[method]["supported"] is False
        assert responses[method]["fail_closed"] is True
        assert responses[method]["reason"]

    rename_reason = responses["textDocument/rename"]["reason"]
    assert "semantic reference closure" in rename_reason
    assert "collision scopes" in rename_reason
    assert "selector disambiguation" in rename_reason
    assert "generated-accessor provenance" in rename_reason
    assert "package write-boundary proofs" in rename_reason
    assert responses["textDocument/codeAction"]["required_proofs"] == [
        "compiler-owned edit provenance",
        "package write-boundary proof",
        "collision-safe workspace edit plan",
    ]
    assert "textDocument/semanticTokens/full" in replay["unsupported_methods"]
    assert replay["cache_invalidation_count"] >= 3

    did_change_key = responses["textDocument/didChange"]["cache_key"]
    did_open_key = responses["textDocument/didOpen"]["cache_key"]
    assert did_change_key["source_digest"] != did_open_key["source_digest"]
    assert did_change_key["document_version"] == 2
    assert responses["workspace/didChangeWatchedFiles"]["workspace_file_count"] >= 2


def test_language_service_public_workflow_actions_are_registered() -> None:
    inspect_payload = public_workflow_action_payload("inspect-language-service")
    validate_payload = public_workflow_action_payload("validate-language-service")

    assert inspect_payload["backend"] == "python:scripts/build_objc3c_language_service_surface.py"
    assert inspect_payload["pass_through_args"] is True
    assert registered_action_handler("inspect-language-service") is not None

    assert validate_payload["backend"] == "python:scripts/check_objc3c_language_service.py"
    assert validate_payload["pass_through_args"] is False
    assert registered_action_handler("validate-language-service") is not None
