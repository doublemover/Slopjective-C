from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m249_e022_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard2_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_e022_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard2_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_e022_lane_e_release_gate_docs_and_runbooks_advanced_edge_compatibility_workpack_shard2_contract.py"
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
        "m249-e022-lane-e-release-gate-docs-runbooks-advanced-edge-compatibility-workpack-shard2-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_e022() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-E022/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_e022_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M249-E021`, `M249-A008`, `M249-B010`, `M249-C011`, `M249-D018`",
            "Dependencies: `M249-E021`, `M249-A008`, `M249-B999`, `M249-C011`, `M249-D018`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E022-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_mandatory_scope_wording_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m249_e022_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "mandatory scope inputs.",
            "optional scope inputs.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E022-DOC-PKT-07" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_no_longer_chains_e021(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_e022_lane_e_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m249_e021_lane_e_readiness.py",
            "scripts/run_m249_e010_lane_e_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E022-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifted(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    payload = json.loads(contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"))
    payload["scripts"]["check:objc3c:m249-e022-lane-e-readiness"] = (
        "python scripts/run_m249_e022_lane_e_readiness_drift.py"
    )
    drift_package.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E022-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_e021_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--e021-checker",
            str(tmp_path / "missing_e021_checker.py"),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-E022-DEP-E021-ARG-01" for failure in payload["failures"])
