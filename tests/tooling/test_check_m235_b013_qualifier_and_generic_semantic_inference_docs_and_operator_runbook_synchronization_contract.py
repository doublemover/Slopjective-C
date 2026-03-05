from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m235_b013_qualifier_and_generic_semantic_inference_docs_and_operator_runbook_synchronization_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m235_b013_qualifier_and_generic_semantic_inference_docs_and_operator_runbook_synchronization_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m235_b013_qualifier_and_generic_semantic_inference_docs_and_operator_runbook_synchronization_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all(text: str, old: str, new: str) -> str:
    assert old in text
    replaced = text.replace(old, new)
    assert old not in replaced
    return replaced


def write_snippet_fixture(path: Path, snippets: tuple[contract.SnippetCheck, ...]) -> None:
    path.write_text("\n".join(snippet.snippet for snippet in snippets) + "\n", encoding="utf-8")


def supporting_overrides(tmp_path: Path) -> list[str]:
    b012_expectations = tmp_path / "m235_b012_expectations.md"
    write_snippet_fixture(b012_expectations, contract.B012_EXPECTATIONS_SNIPPETS)

    b012_packet = tmp_path / "m235_b012_packet.md"
    write_snippet_fixture(b012_packet, contract.B012_PACKET_SNIPPETS)

    return [
        "--b012-expectations-doc",
        str(b012_expectations),
        "--b012-packet-doc",
        str(b012_packet),
    ]


def test_contract_passes_with_synthetic_supporting_assets(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([*supporting_overrides(tmp_path), "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m235-b013-qualifier-and-generic-semantic-inference-docs-and-operator-runbook-synchronization-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 34
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m235_b013() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m235/M235-B013/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m235_b013_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M235-B012`",
            "Dependencies: `M235-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            *supporting_overrides(tmp_path),
            "--expectations-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B013-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m235_b013_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5793`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            *supporting_overrides(tmp_path),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B013-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b012_packet_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m235_b012_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_B012_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5792`",
            "Issue: `#7000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--b012-packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B013-B012-PKT-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b012_expectations_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_doc = tmp_path / "missing_b012_expectations.md"
    exit_code = contract.run(
        [
            "--b012-expectations-doc",
            str(missing_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B013-B012-DOC-EXISTS" for failure in payload["failures"])
