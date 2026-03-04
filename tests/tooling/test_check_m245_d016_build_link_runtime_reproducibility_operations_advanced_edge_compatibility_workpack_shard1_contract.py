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
    / "check_m245_d016_build_link_runtime_reproducibility_operations_advanced_edge_compatibility_workpack_shard1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m245_d016_build_link_runtime_reproducibility_operations_advanced_edge_compatibility_workpack_shard1_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m245_d016_build_link_runtime_reproducibility_operations_advanced_edge_compatibility_workpack_shard1_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m245-d016-build-link-runtime-reproducibility-operations-advanced-edge-compatibility-workpack-shard1-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_d016() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-D016/")


def test_contract_emit_json_stdout_has_summary_parity(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout_payload = json.loads(capsys.readouterr().out)
    summary_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert stdout_payload == summary_payload
    assert stdout_payload["mode"] == contract.MODE
    assert stdout_payload["ok"] is True


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_d016_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-D015`",
            "Dependencies: `M245-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-D016-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_d016_expectations_issue_drift.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Issue `#6667` defines canonical lane-D advanced edge compatibility workpack (shard 1) scope.",
            "Issue `#6999` defines canonical lane-D advanced edge compatibility workpack (shard 1) scope.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-D016-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_theme_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m245_d016_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Theme: `advanced edge compatibility workpack (shard 1)`",
            "Theme: `advanced core workpack (shard 1)`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-D016-DOC-PKT-05" for failure in payload["failures"])


def test_contract_fails_closed_when_dependency_artifacts_missing_and_sorted(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_d015_checker.py"
    missing_test = tmp_path / "missing_d015_test.py"
    exit_code = contract.run(
        [
            "--d015-checker",
            str(missing_checker),
            "--d015-test",
            str(missing_test),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    check_ids = [failure["check_id"] for failure in payload["failures"]]
    assert "M245-D016-DEP-D015-ARG-01" in check_ids
    assert "M245-D016-DEP-D015-ARG-02" in check_ids
    assert payload["failures"] == sorted(
        payload["failures"],
        key=lambda failure: (failure["check_id"], failure["artifact"], failure["detail"]),
    )
