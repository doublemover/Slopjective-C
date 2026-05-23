#!/usr/bin/env python3
"""Validate the Objective-C 3 language-service core replay fixture."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT / "scripts") not in sys.path:
    sys.path.insert(0, str(ROOT / "scripts"))

from objc3c_editor_tooling.model import extract_symbols  # noqa: E402
from objc3c_editor_tooling.source_index import build_source_graph, build_source_index  # noqa: E402
from objc3c_editor_tooling.workspace_index import build_workspace_index  # noqa: E402
from objc3c_language_service import ObjectiveC3LanguageService, replay_requests  # noqa: E402
from objc3c_tooling.json_io import load_json_object, write_json_file  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"
SOURCE_GRAPH_ROOT = FIXTURE_ROOT / "source_graph"
REQUEST_FIXTURE = FIXTURE_ROOT / "language_service" / "request_replay.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "developer-tooling" / "language-service-summary.json"


def build_fixture_source_graph() -> tuple[str, str, dict[str, Any]]:
    contract = load_json_object(SOURCE_GRAPH_ROOT / "source_graph_contract.json")
    manifest = load_json_object(SOURCE_GRAPH_ROOT / "manifest.json")
    source_path = str(contract["source_path"])
    module_name = str(contract["module_name"])
    manifest_path = str(contract["manifest_path"])
    source_text = (ROOT / source_path).read_text(encoding="utf-8")
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
    return source_path, source_text, build_source_graph(
        source_path=source_path,
        module_name=module_name,
        manifest_path=manifest_path,
        manifest=manifest,
        symbols=symbols,
        source_index=source_index,
        workspace_index=workspace_index,
    )


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def validate_replay(replay: dict[str, Any]) -> list[str]:
    failures: list[str] = []
    responses = {
        str(response.get("method", "")): response.get("result", {})
        for response in replay.get("responses", [])
        if isinstance(response, dict)
    }
    expect(responses.get("initialize", {}).get("capabilities", {}).get("definitionProvider") is True, "initialize did not publish definitionProvider", failures)
    expect(responses.get("textDocument/documentSymbol", {}).get("symbol_count") == 2, "documentSymbol did not return fixture declarations", failures)
    expect(responses.get("textDocument/definition", {}).get("supported") is True, "definition from declaration coordinate failed", failures)
    expect(responses.get("textDocument/hover", {}).get("supported") is True, "hover from declaration coordinate failed", failures)
    expect(responses.get("workspace/symbol", {}).get("symbol_count", 0) >= 1, "workspace/symbol did not index fixture symbols", failures)
    for method in (
        "textDocument/references",
        "textDocument/rename",
        "textDocument/codeAction",
        "textDocument/semanticTokens/full",
    ):
        expect(responses.get(method, {}).get("fail_closed") is True, f"{method} did not fail closed", failures)
    expect(replay.get("cache_invalidation_count", 0) >= 3, "document/workspace changes did not record invalidations", failures)
    return failures


def main() -> int:
    source_path, source_text, source_graph = build_fixture_source_graph()
    request_payload = load_json_object(REQUEST_FIXTURE)
    requests = request_payload.get("requests", [])
    if not isinstance(requests, list):
        raise ValueError("request_replay.json must contain requests")
    service = ObjectiveC3LanguageService(
        source_graph=source_graph,
        source_texts={source_path: source_text},
    )
    replay = replay_requests(
        service,
        [request for request in requests if isinstance(request, dict)],
    )
    failures = validate_replay(replay)
    summary = {
        "contract_id": "objc3c.language_service.validation.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "ok": not failures,
        "fixture": repo_rel(REQUEST_FIXTURE),
        "source_graph_fixture": repo_rel(SOURCE_GRAPH_ROOT / "source_graph_contract.json"),
        "request_count": replay.get("request_count", 0),
        "unsupported_methods": replay.get("unsupported_methods", []),
        "cache_invalidation_count": replay.get("cache_invalidation_count", 0),
        "failures": failures,
    }
    write_json_file(SUMMARY_PATH, summary)
    print(f"language-service: {summary['status']}")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
