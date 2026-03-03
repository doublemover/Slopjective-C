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
    / "check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m248-c013-replay-harness-and-artifact-contracts-docs-operator-runbook-synchronization-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m248_c013() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m248/M248-C013/")


def test_contract_emits_json_when_requested(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        lane_task=original.lane_task,
        relative_path=Path("docs/contracts/does_not_exist_m248_c013.md"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_c012_dependency(
    tmp_path: Path,
) -> None:
    drift_doc = (
        tmp_path
        / "m248_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M248-C012`",
            "Dependencies: `M248-C099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-C013-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_issue_anchor_drifts(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_c013_expectations_issue_drift.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#6829` defines canonical lane-C docs and operator runbook synchronization scope.",
            "Issue `#6999` defines canonical lane-C docs and operator runbook synchronization scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-C013-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_c013_readiness_command(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_c013_expectations_readiness_drift.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`check:objc3c:m248-c013-lane-c-readiness`",
            "`check:objc3c:m248-c013-lane-c-docs-readiness`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-C013-DOC-EXP-11" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_deterministic_invariant_token(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / "m248_c013_expectations_invariant_drift.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`docs_runbook_sync_key_ready`",
            "`docs_runbook_sync_gate_ready`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-C013-DOC-EXP-08" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_drifts(tmp_path: Path) -> None:
    drift_packet = (
        tmp_path
        / "m248_c013_replay_harness_and_artifact_contracts_docs_and_operator_runbook_synchronization_packet.md"
    )
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M248-C012`",
            "Dependencies: `M248-C099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M248-C013-DOC-PKT-03" for failure in payload["failures"])
