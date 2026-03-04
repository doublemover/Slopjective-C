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
    / "check_m246_d006_toolchain_integration_and_optimization_controls_edge_case_expansion_and_robustness_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_d006_toolchain_integration_and_optimization_controls_edge_case_expansion_and_robustness_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_d006_toolchain_integration_and_optimization_controls_edge_case_expansion_and_robustness_contract.py"
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
        "m246-d006-toolchain-integration-optimization-controls-edge-case-expansion-and-robustness-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_d006() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-D006/")


def test_contract_emits_json_when_requested_and_matches_summary_payload(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    stdout_payload = json.loads(stdout)
    summary_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert stdout_payload == summary_payload
    assert stdout == contract.canonical_json(stdout_payload)
    assert stdout_payload["mode"] == contract.MODE
    assert stdout_payload["ok"] is True
    assert stdout_payload["checks_total"] == stdout_payload["checks_passed"]


def test_contract_failures_are_sorted_deterministically(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_d006_expectations.md"
    drift_doc.write_text(
        replace_once(
            replace_once(
                contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
                "Canonical issue: `#6685`",
                "Canonical issue: `#6999`",
            ),
            "Dependencies: `M246-D005`",
            "Dependencies: `M246-D399`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    failures = payload["failures"]
    assert failures == sorted(
        failures,
        key=lambda failure: (failure["artifact"], failure["check_id"], failure["detail"]),
    )
    assert any(failure["check_id"] == "M246-D006-DOC-EXP-04" for failure in failures)
    assert any(failure["check_id"] == "M246-D006-DOC-EXP-05" for failure in failures)


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_d006_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-D005`",
            "Dependencies: `M246-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D006-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m246_d006_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6685`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D006-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_doc_is_not_utf8(tmp_path: Path) -> None:
    bad_doc = tmp_path / "m246_d006_expectations.md"
    bad_doc.write_bytes(b"\xff\xfe\xfd\xfc")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(bad_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M246-D006-DOC-EXP-EXISTS"
        and "unable to read required document" in failure["detail"]
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_readiness_runner_drops_d005_chain(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_d006_lane_d_readiness.py"
    drift_readiness.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_d005_lane_d_readiness.py",
            "scripts/run_m246_d004_lane_d_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D006-RDY-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d005_dependency_token_drifts(tmp_path: Path) -> None:
    drift_d005_expectations = tmp_path / "m246_d005_expectations.md"
    drift_d005_expectations.write_text(
        replace_once(
            contract.DEFAULT_D005_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-D004`",
            "Dependencies: `M246-D399`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d005-expectations-doc",
            str(drift_d005_expectations),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D006-D005-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d005_readiness_dependency_token_drifts(tmp_path: Path) -> None:
    drift_d005_readiness = tmp_path / "run_m246_d005_lane_d_readiness.py"
    drift_d005_readiness.write_text(
        replace_once(
            contract.DEFAULT_D005_READINESS_SCRIPT.read_text(encoding="utf-8"),
            'DEPENDENCY_TOKEN = "M246-D004"',
            'DEPENDENCY_TOKEN = "M246-D099"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d005-readiness-script",
            str(drift_d005_readiness),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D006-D005-RDY-01" for failure in payload["failures"])


def test_contract_fails_closed_when_d005_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d005-checker",
            str(tmp_path / "missing_d005_checker.py"),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D006-DEP-D005-ARG-01" for failure in payload["failures"])
