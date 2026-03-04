from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m249_e008_lane_e_release_gate_docs_and_runbooks_recovery_and_determinism_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_e008_lane_e_release_gate_docs_and_runbooks_recovery_and_determinism_hardening_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_e008_lane_e_release_gate_docs_and_runbooks_recovery_and_determinism_hardening_contract.py"
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
    assert payload["mode"] == "m249-e008-lane-e-release-gate-docs-runbooks-recovery-and-determinism-hardening-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_e008() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-E008/")


def test_contract_fails_closed_when_expectations_drop_c008_dependency_token(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m249_lane_e_release_gate_docs_and_runbooks_recovery_and_determinism_hardening_e008_expectations.md"
    )
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependency token `M249-C008` is mandatory for lane-C recovery and determinism hardening readiness chaining.",
            "Dependency token `M249-C099` is mandatory for lane-C recovery and determinism hardening readiness chaining.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E008-DOC-EXP-08" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_chain_drops_b008_readiness(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_e008_lane_e_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_RUNNER_SCRIPT.read_text(encoding="utf-8"),
            "check:objc3c:m249-b008-lane-b-readiness",
            "check:objc3c:m249-b099-lane-b-readiness",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--runner-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E008-RUN-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_c008_readiness_token(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_once(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m249-c008-lane-c-readiness": ',
            '"check:objc3c:m249-c099-lane-c-readiness": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E008-PKG-04" for failure in payload["failures"])


def test_contract_fails_closed_when_e006_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_m249_e007_checker.py"
    exit_code = contract.run(
        [
            "--e006-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E008-DEP-E006-ARG-01" for failure in payload["failures"])
