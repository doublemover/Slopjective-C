from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.model import build_language_server_payload, extract_symbols  # noqa: E402
from objc3c_editor_tooling.source_index import build_source_graph, build_source_index  # noqa: E402
from objc3c_editor_tooling.workspace_index import build_workspace_index  # noqa: E402


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling" / "source_graph"


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def build_fixture_graph() -> tuple[dict[str, Any], dict[str, Any]]:
    contract = load_json(FIXTURE_ROOT / "source_graph_contract.json")
    manifest = load_json(FIXTURE_ROOT / "manifest.json")
    source_path = str(contract["source_path"])
    manifest_path = str(contract["manifest_path"])
    module_name = str(contract["module_name"])
    symbols = extract_symbols(manifest)
    source_index = build_source_index(
        source_path=source_path,
        module_name=module_name,
        source_text=(ROOT / source_path).read_text(encoding="utf-8"),
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
    return contract, build_source_graph(
        source_path=source_path,
        module_name=module_name,
        manifest_path=manifest_path,
        manifest=manifest,
        symbols=symbols,
        source_index=source_index,
        workspace_index=workspace_index,
    )


def test_source_graph_publishes_stable_nodes_edges_and_package_provenance() -> None:
    contract, graph = build_fixture_graph()
    _, rebuilt_graph = build_fixture_graph()

    assert graph["contract_id"] == "objc3c.developer.tooling.source.graph.v1"
    assert graph["available"] is True
    assert graph["source_graph_digest"] == rebuilt_graph["source_graph_digest"]
    assert [entry["symbol"] for entry in graph["declarations"]] == contract["expected_declaration_symbols"]
    assert graph["declaration_node_count"] == 2
    assert graph["package_provenance"]["local_package_id"] == f"source:{contract['source_path']}"
    assert graph["package_node_count"] >= 1
    assert graph["evidence"]["native_compiler_source_graph_present"] is True
    assert graph["evidence"]["lexical_candidates_authoritative"] is False
    assert any(edge["edge_kind"] == "compiler-source-graph-edge" for edge in graph["edges"])
    assert all(node["node_id"].startswith("sgn:") for node in graph["nodes"])


def test_source_graph_fails_closed_for_unproven_references_and_safe_rename() -> None:
    contract, graph = build_fixture_graph()

    assert graph["fail_closed"] is True
    assert [candidate["symbol"] for candidate in graph["reference_candidates"]] == (
        contract["expected_reference_candidate_symbols"]
    )
    assert graph["references"]["supported"] is False
    assert graph["references"]["fail_closed"] is True
    assert graph["rename"]["supported"] is False
    assert graph["rename"]["fail_closed"] is True
    assert graph["semantic_tokens"]["supported"] is False
    assert graph["semantic_token_count"] == 2

    diagnostic_codes = {diagnostic["code"] for diagnostic in graph["diagnostics"]}
    assert set(contract["expected_fail_closed_diagnostics"]).issubset(diagnostic_codes)
    candidate = graph["reference_candidates"][0]
    assert candidate["semantic_proof"] is False
    assert "lexical source index candidate" in candidate["fail_closed_reason"]
    assert "regex-only reference truth" in graph["evidence"]["unsupported_authoritative_claims"]


def test_language_server_uses_source_graph_status_without_overpublishing() -> None:
    _, graph = build_fixture_graph()

    payload = build_language_server_payload(
        {"observability": {"status_name": "OK"}},
        graph["evidence"]["manifest_path"],
        [
            {
                "name": declaration["symbol"],
                "kind": declaration["kind"],
                "line": declaration["definition_target"]["target_compiler_range"]["line"],
                "column": declaration["definition_target"]["target_compiler_range"]["column"],
            }
            for declaration in graph["declarations"]
        ],
        {"available": True, "package_count": 2},
        source_path=graph["source_path"],
        diagnostic_entries=[],
        source_graph=graph,
    )

    assert payload["source_graph_digest"] == graph["source_graph_digest"]
    assert payload["source_graph_backed_references"] is False
    for capability_id in ("references", "rename", "semanticTokens"):
        status = payload["capability_statuses"][capability_id]
        assert status["supported"] is False
        assert status["support_class"] == "source-graph-fail-closed"
        assert status["fail_closed"] is True
        assert status["evidence_ids"] == []
        assert capability_id in payload["unpublished_capability_ids"]
