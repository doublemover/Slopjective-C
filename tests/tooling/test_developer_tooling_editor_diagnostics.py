from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_editor_tooling.diagnostic_bridge import build_lsp_diagnostic_transport  # noqa: E402
from objc3c_editor_tooling.model import build_language_server_payload  # noqa: E402


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "editor_diagnostic_bridge_contract.json"
)
POLICY_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "developer_tooling"
    / "language_server_capability_publication_policy.json"
)


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def load_contract_diagnostics(contract: dict[str, Any]) -> list[dict[str, Any]]:
    diagnostics: list[dict[str, Any]] = []
    for path_text in contract["diagnostic_case_paths"]:
        case = load_json(ROOT / str(path_text))
        expect = case["expect"]
        assert isinstance(expect, dict)
        for diagnostic in expect["diagnostics"]:
            assert isinstance(diagnostic, dict)
            diagnostics.append(diagnostic)
    return diagnostics


def test_lsp_diagnostic_transport_preserves_fixits_and_recovery_boundaries() -> None:
    contract = load_json(CONTRACT_PATH)
    diagnostics = load_contract_diagnostics(contract)

    transport = build_lsp_diagnostic_transport(str(contract["source_uri"]), diagnostics)

    assert (
        transport["contract_id"]
        == "objc3c.developer.tooling.lsp.diagnostic.transport.v1"
    )
    assert transport["publish_method"] == "textDocument/publishDiagnostics"
    assert transport["diagnostic_count"] == contract["expected_diagnostic_count"]
    assert transport["code_action_count"] == contract["expected_code_action_count"]
    assert (
        transport["machine_applicable_fixit_count"]
        == contract["expected_machine_applicable_fixit_count"]
    )
    assert (
        transport["recovery_boundary_count"]
        == contract["expected_recovery_boundary_count"]
    )
    assert sorted(
        action["diagnostic_code"] for action in transport["code_actions"]
    ) == contract["expected_code_action_codes"]
    assert sorted(
        recovery["diagnostic_code"] for recovery in transport["recovery_boundaries"]
    ) == contract["expected_recovery_codes"]
    assert sorted(
        {recovery["phase"] for recovery in transport["recovery_boundaries"]}
    ) == contract["expected_recovery_phases"]
    assert all(
        recovery["deterministic"] is True
        and recovery["accepts_invalid_program"] is False
        and recovery["recovery_counts_as_success"] is False
        for recovery in transport["recovery_boundaries"]
    )

    optional_action = next(
        action
        for action in transport["code_actions"]
        if action["diagnostic_code"] == "O3C004"
    )
    edit = optional_action["edit"]["changes"][str(contract["source_uri"])][0]
    assert edit["newText"] == "Optional"
    assert edit["range"] == {
        "start": {"line": 2, "character": 15},
        "end": {"line": 2, "character": 23},
    }


def test_language_server_payload_advertises_code_actions_only_with_fixits() -> None:
    contract = load_json(CONTRACT_PATH)
    diagnostics = load_contract_diagnostics(contract)

    payload = build_language_server_payload(
        {"observability": {"status_name": "diagnostics"}},
        None,
        [],
        None,
        source_path=str(contract["source_uri"]),
        diagnostic_entries=diagnostics,
    )

    assert "publishDiagnostics" in payload["supported_capability_ids"]
    assert "codeAction" in payload["supported_capability_ids"]
    assert "codeAction" not in payload["unpublished_capability_ids"]
    assert payload["capability_statuses"]["codeAction"] == {
        "supported": True,
        "support_class": "diagnostics-fixit-backed",
        "evidence": "diagnostics-json-fixits",
        "evidence_ids": ["diagnostics-json-fixits"],
        "fail_closed": False,
        "unpublished_reason": "",
    }
    assert (
        payload["diagnostic_transport"]["recovery_acceptance_policy"]
        == "recovery metadata is diagnostic context only and never turns an invalid program into a success"
    )

    no_fixit_payload = build_language_server_payload(
        {"observability": {"status_name": "ok"}},
        None,
        [],
        None,
        source_path=str(contract["source_uri"]),
        diagnostic_entries=[],
    )
    assert "codeAction" not in no_fixit_payload["supported_capability_ids"]
    assert "codeAction" in no_fixit_payload["unpublished_capability_ids"]
    assert no_fixit_payload["capability_statuses"]["codeAction"]["supported"] is False
    assert no_fixit_payload["capability_statuses"]["codeAction"]["fail_closed"] is True


def test_language_server_unpublished_rows_follow_actual_capability_statuses() -> None:
    payload = build_language_server_payload(
        {"observability": {"status_name": "OK"}},
        None,
        [],
        {"available": False, "package_count": 0},
        source_path="tests/tooling/fixtures/native/hello.objc3",
        diagnostic_entries=[],
    )

    assert payload["supported_capability_ids"] == ["publishDiagnostics"]
    assert {
        "documentSymbol",
        "workspaceSymbol",
        "definition",
        "references",
        "rename",
        "semanticTokens",
        "codeAction",
        "statementLevelStepping",
    }.issubset(set(payload["unpublished_capability_ids"]))
    for capability_id in payload["unpublished_capability_ids"]:
        assert payload["capability_statuses"][capability_id]["supported"] is False
        assert payload["capability_statuses"][capability_id]["fail_closed"] is True


def test_language_server_rows_publish_evidence_or_fail_closed() -> None:
    contract = load_json(CONTRACT_PATH)
    diagnostics = load_contract_diagnostics(contract)
    workspace_index = {
        "available": True,
        "package_count": 2,
        "guardrails": {"ok": True},
    }

    payload = build_language_server_payload(
        {"observability": {"status_name": "OK"}},
        "tmp/artifacts/module.manifest.json",
        [{"name": "main", "kind": "function", "line": 1, "column": 1}],
        workspace_index,
        source_path=str(contract["source_uri"]),
        diagnostic_entries=diagnostics,
    )

    for capability_id in payload["supported_capability_ids"]:
        status = payload["capability_statuses"][capability_id]
        assert status["supported"] is True
        assert status["fail_closed"] is False
        assert status["evidence_ids"]
    for capability_id in ("references", "rename", "semanticTokens", "statementLevelStepping"):
        status = payload["capability_statuses"][capability_id]
        assert status["supported"] is False
        assert status["fail_closed"] is True
        assert not status["evidence_ids"]
        assert capability_id in payload["unpublished_capability_ids"]


def test_language_server_policy_documents_fixit_backed_code_action_boundary() -> None:
    policy = load_json(POLICY_PATH)
    conditional = {
        item["capability"]: item
        for item in policy["conditional_capability_classes"]
        if isinstance(item, dict)
    }

    assert "codeAction" in policy["unpublished_capability_classes"]
    assert conditional["codeAction"] == {
        "capability": "codeAction",
        "published_when": "diagnostics-json-fixits",
        "support_class": "diagnostics-fixit-backed",
        "unpublished_without": "machine-applicable diagnostic fix-it ranges",
    }
