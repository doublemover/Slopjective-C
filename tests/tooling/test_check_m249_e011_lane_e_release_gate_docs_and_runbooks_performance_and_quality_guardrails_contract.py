from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_e011_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_contract.py"
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
    assert payload["mode"] == "m249-e011-lane-e-release-gate-docs-runbooks-performance-and-quality-guardrails-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 55
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_e011() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-E011/")


def test_contract_fails_closed_when_expectations_drop_c006_dependency_token(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m249_lane_e_release_gate_docs_and_runbooks_performance_and_quality_guardrails_e011_expectations.md"
    )
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependency token `M249-C006` is mandatory for lane-C edge-case expansion and robustness readiness chaining.",
            "Dependency token `M249-C099` is mandatory for lane-C edge-case expansion and robustness readiness chaining.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E011-DOC-EXP-07" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_chain_drops_e010_readiness(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_e011_lane_e_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_RUNNER_SCRIPT.read_text(encoding="utf-8"),
            "check:objc3c:m249-e010-lane-e-readiness",
            "check:objc3c:m249-e999-lane-e-readiness",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--runner-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E011-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_d009_readiness_token(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_once(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m249-d009-lane-d-readiness": ',
            '"check:objc3c:m249-d999-lane-d-readiness": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E011-PKG-07" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_mandatory_scope_wording_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m249_e011_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "milestone optimization improvements as mandatory scope inputs.",
            "milestone optimization improvements as optional scope inputs.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E011-DOC-PKT-11" for failure in payload["failures"])


def test_contract_fails_closed_when_e010_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_m249_e010_checker.py"
    exit_code = contract.run(
        [
            "--e010-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E011-DEP-E010-ARG-01" for failure in payload["failures"])
