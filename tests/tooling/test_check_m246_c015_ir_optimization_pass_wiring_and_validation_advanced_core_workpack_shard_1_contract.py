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
    / "check_m246_c015_ir_optimization_pass_wiring_and_validation_advanced_core_workpack_shard_1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_c015_ir_optimization_pass_wiring_and_validation_advanced_core_workpack_shard_1_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_c015_ir_optimization_pass_wiring_and_validation_advanced_core_workpack_shard_1_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all_occurrences(text: str, old: str, new: str) -> str:
    count = text.count(old)
    assert count > 0, f"expected snippet not found for drift mutation: {old}"
    return text.replace(old, new)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m246-c015-ir-optimization-pass-wiring-validation-advanced-core-workpack-shard-1-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 70
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_c015() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-C015/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m246_c015_dependency.py"),
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
        tmp_path
        / "m246_ir_optimization_pass_wiring_and_validation_advanced_core_workpack_shard_1_c015_expectations.md"
    )
    drift_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-C014`",
            "Dependencies: `M246-C099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = (
        tmp_path
        / "m246_c015_ir_optimization_pass_wiring_and_validation_advanced_core_workpack_shard_1_packet.md"
    )
    drift_packet.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5091`",
            "Issue: `#5000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_c014_dependency_anchor_drifts(tmp_path: Path) -> None:
    drift_c014_doc = (
        tmp_path
        / "m246_ir_optimization_pass_wiring_and_validation_release_candidate_and_replay_dry_run_c014_expectations.md"
    )
    drift_c014_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_C014_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-C013`",
            "Dependencies: `M246-C099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c014-expectations-doc",
            str(drift_c014_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-C014-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_c014_readiness_drops_c013_chain_anchor(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_c014_lane_c_readiness.py"
    drift_readiness.write_text(
        replace_all_occurrences(
            contract.DEFAULT_C014_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_c013_lane_c_readiness.py",
            "scripts/run_m246_c099_lane_c_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c014-readiness-script",
            str(drift_readiness),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-C014-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_c015_readiness_drops_c014_chain(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_c015_lane_c_readiness.py"
    drift_readiness.write_text(
        replace_all_occurrences(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_c014_lane_c_readiness.py",
            "scripts/run_m246_c099_lane_c_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_c015_readiness_checker_drifts_to_implementation(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_c015_lane_c_readiness.py"
    drift_readiness.write_text(
        replace_all_occurrences(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/check_m246_c015_ir_optimization_pass_wiring_and_validation_advanced_core_workpack_shard_1_contract.py",
            "scripts/check_m246_c004_ir_optimization_pass_wiring_and_validation_core_feature_implementation_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"test:objc3c:perf-budget": ',
            '"test:objc3c:perf-budget-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C015-PKG-05" for failure in payload["failures"])


