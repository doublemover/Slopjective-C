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
