from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m234_d012_runtime_property_metadata_integration_closeout_and_gate_sign_off_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m234_d012_runtime_property_metadata_integration_closeout_and_gate_sign_off_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m234_d012_runtime_property_metadata_integration_closeout_and_gate_sign_off_contract.py"
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
        "m234-d012-runtime-property-metadata-integration-closeout-and-gate-sign-off-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m234_d012() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m234/M234-D012/")


def test_contract_summary_json_is_deterministic(tmp_path: Path) -> None:
    summary_out_a = tmp_path / "summary_a.json"
    summary_out_b = tmp_path / "summary_b.json"

    assert contract.run(["--summary-out", str(summary_out_a)]) == 0
    assert contract.run(["--summary-out", str(summary_out_b)]) == 0

    text_a = summary_out_a.read_text(encoding="utf-8")
    text_b = summary_out_b.read_text(encoding="utf-8")
    assert text_a == text_b
    payload = json.loads(text_a)
    assert text_a == contract.canonical_json(payload)


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m234_d012_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M234-D011`",
            "Dependencies: `M234-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D012-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m234_d012_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5747`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D012-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_d011_chain(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m234_d012_lane_d_readiness.py"
    drift_readiness.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m234_d011_lane_d_readiness.py",
            "scripts/run_m234_d010_lane_d_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D012-RDY-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d011_dependency_token_drifts(tmp_path: Path) -> None:
    drift_d011_expectations = tmp_path / "m234_d011_expectations.md"
    drift_d011_expectations.write_text(
        replace_once(
            contract.DEFAULT_D011_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M234-D010`",
            "Dependencies: `M234-D399`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d011-expectations-doc",
            str(drift_d011_expectations),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D012-D011-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d011_readiness_predecessor_chain_drifts(tmp_path: Path) -> None:
    drift_d011_readiness = tmp_path / "run_m234_d011_lane_d_readiness.py"
    drift_d011_readiness.write_text(
        replace_once(
            contract.DEFAULT_D011_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m234_d010_lane_d_readiness.py",
            "scripts/run_m234_d009_lane_d_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d011-readiness-script",
            str(drift_d011_readiness),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D012-D011-RDY-01" for failure in payload["failures"])


def test_contract_fails_closed_when_d011_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d011-checker",
            str(tmp_path / "missing_d011_checker.py"),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D012-DEP-D011-ARG-01" for failure in payload["failures"])
