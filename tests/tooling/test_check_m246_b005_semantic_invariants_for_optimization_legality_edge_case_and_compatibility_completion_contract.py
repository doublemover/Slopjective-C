from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_one(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m246-b005-semantic-invariants-optimization-legality-edge-case-and-compatibility-completion-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 49
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_b005() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-B005/")


def test_contract_emit_json_writes_canonical_summary_to_stdout(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    captured = capsys.readouterr()
    assert captured.out == contract.canonical_json(payload)
    assert captured.err == ""


def test_contract_failures_are_sorted_deterministically(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    monkeypatch.setattr(
        contract,
        "EXPECTATIONS_SNIPPETS",
        (
            contract.SnippetCheck("M246-B005-DOC-EXP-Z", "__missing-z-snippet__"),
            contract.SnippetCheck("M246-B005-DOC-EXP-A", "__missing-a-snippet__"),
        ),
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert [failure["check_id"] for failure in payload["failures"]] == [
        "M246-B005-DOC-EXP-A",
        "M246-B005-DOC-EXP-Z",
    ]


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m246_b005_dependency.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m246_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_b005.md"
    )
    drift_doc.write_text(
        replace_one(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-B004`",
            "Dependencies: `M246-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-B005-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = (
        tmp_path / "m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_packet.md"
    )
    drift_packet.write_text(
        replace_one(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "- Issue: `#5064`",
            "- Issue: `#5999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-B005-DOC-PKT-06" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_chain_drops_b004_runner(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_b005_lane_b_readiness.py"
    drift_readiness.write_text(
        replace_one(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_b004_lane_b_readiness.py",
            "scripts/run_m246_b099_lane_b_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-B005-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_read_errors(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path / "m246_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_b005.md"
    )
    drift_doc.write_text(contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"), encoding="utf-8")

    original_read_text = contract.Path.read_text

    def read_text_with_error(self: Path, encoding: str = "utf-8") -> str:
        if self.resolve() == drift_doc.resolve():
            raise OSError("simulated read failure for #5064")
        return original_read_text(self, encoding=encoding)

    monkeypatch.setattr(contract.Path, "read_text", read_text_with_error)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-B005-DOC-EXP-EXISTS" for failure in payload["failures"])
