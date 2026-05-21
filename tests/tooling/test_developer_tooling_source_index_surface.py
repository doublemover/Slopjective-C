from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.model import (  # noqa: E402
    build_language_server_payload,
    build_navigation_payload,
    extract_symbols,
)
from objc3c_editor_tooling.source_index import build_source_index  # noqa: E402
from objc3c_editor_tooling.workspace_index import build_workspace_index  # noqa: E402


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"
ARTIFACT_FIXTURE_ROOT = FIXTURE_ROOT / "artifact_inspector"


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def test_source_index_derives_declarations_references_diagnostics_and_artifacts() -> None:
    source_path = "tests/tooling/fixtures/developer_tooling/artifact_inspector/source.objc3"
    manifest_path = "tests/tooling/fixtures/developer_tooling/artifact_inspector/manifest.json"
    manifest = load_json(ARTIFACT_FIXTURE_ROOT / "manifest.json")
    diagnostics = load_json(ARTIFACT_FIXTURE_ROOT / "diagnostics.json")["diagnostics"]
    summary_paths = load_json(ARTIFACT_FIXTURE_ROOT / "summary.json")["paths"]
    symbols = extract_symbols(manifest)

    source_index = build_source_index(
        source_path=source_path,
        module_name="ArtifactInspectorDemo",
        source_text=(ARTIFACT_FIXTURE_ROOT / "source.objc3").read_text(encoding="utf-8"),
        manifest_path=manifest_path,
        symbols=symbols,
        diagnostics=diagnostics,
        artifact_paths=summary_paths,
    )

    assert source_index["contract_id"] == "objc3c.developer.tooling.source.index.v1"
    assert source_index["available"] is True
    assert source_index["declaration_count"] == 4
    assert source_index["reference_count"] == 5
    assert source_index["diagnostic_anchor_count"] == 1
    assert source_index["emitted_artifact_count"] == 5
    assert len(source_index["source_index_digest"]) == 64
    assert [declaration["symbol"] for declaration in source_index["declarations"]] == [
        "globalCounter",
        "ArtifactWidget",
        "ArtifactWidget",
        "main",
    ]
    assert any(
        reference["symbol"] == "globalCounter"
        and reference["reference_kind"] == "lexical-reference"
        and reference["location"]["compiler_range"]["line"] == 10
        for reference in source_index["references"]
    )
    assert source_index["diagnostic_anchors"][0]["code"] == "O3T200"


def test_source_index_backs_hover_navigation_and_workspace_rows() -> None:
    source_path = "tests/tooling/fixtures/developer_tooling/artifact_inspector/source.objc3"
    manifest_path = "tests/tooling/fixtures/developer_tooling/artifact_inspector/manifest.json"
    manifest = load_json(ARTIFACT_FIXTURE_ROOT / "manifest.json")
    source_text = (ARTIFACT_FIXTURE_ROOT / "source.objc3").read_text(encoding="utf-8")
    symbols = extract_symbols(manifest)
    source_index = build_source_index(
        source_path=source_path,
        module_name="ArtifactInspectorDemo",
        source_text=source_text,
        manifest_path=manifest_path,
        symbols=symbols,
        diagnostics=[],
        artifact_paths={},
    )
    workspace_index = build_workspace_index(
        source_path,
        "ArtifactInspectorDemo",
        manifest_path,
        symbols,
        source_index,
    )
    language_server = build_language_server_payload(
        {"observability": {"status_name": "ok"}},
        manifest_path,
        symbols,
        workspace_index,
        source_path=source_path,
        diagnostic_entries=[],
        source_index=source_index,
    )
    navigation = build_navigation_payload(
        source_path,
        "ArtifactInspectorDemo",
        manifest_path,
        symbols,
        workspace_index,
        source_index,
    )

    assert "hover" in language_server["supported_capability_ids"]
    assert language_server["capability_statuses"]["hover"] == {
        "supported": True,
        "support_class": "source-index-backed",
        "evidence_ids": [
            "compile-manifest-declaration-coordinates",
            "source-derived-editor-index",
        ],
        "fail_closed": False,
        "unpublished_reason": "",
    }
    assert language_server["source_index_backed_hover"] is True
    assert language_server["source_index_digest"] == source_index["source_index_digest"]
    assert workspace_index["source_declaration_count"] == 4
    assert workspace_index["source_reference_count"] == 5
    assert navigation["source_index"]["source_index_digest"] == source_index["source_index_digest"]
    assert [hover["name"] for hover in navigation["hover_targets"]] == [
        "globalCounter",
        "ArtifactWidget",
        "ArtifactWidget",
        "main",
    ]

