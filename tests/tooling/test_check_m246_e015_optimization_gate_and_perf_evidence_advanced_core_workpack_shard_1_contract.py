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
    / "check_m246_e015_optimization_gate_and_perf_evidence_advanced_core_workpack_shard_1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_e015_optimization_gate_and_perf_evidence_advanced_core_workpack_shard_1_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_e015_optimization_gate_and_perf_evidence_advanced_core_workpack_shard_1_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_parse_args_accepts_overrides(tmp_path: Path) -> None:
    custom_expectations = tmp_path / "expectations.md"
    custom_packet = tmp_path / "packet.md"
    custom_e014_packet = tmp_path / "e014_packet.md"
    custom_b016_expectations = tmp_path / "b016_expectations.md"
    custom_d011_readiness = tmp_path / "run_m246_d011_lane_d_readiness.py"
    custom_summary = tmp_path / "summary.json"
    args = contract.parse_args(
        [
            "--expectations-doc",
            str(custom_expectations),
            "--packet-doc",
            str(custom_packet),
            "--e014-packet-doc",
            str(custom_e014_packet),
            "--b016-expectations-doc",
            str(custom_b016_expectations),
            "--d011-readiness-script",
            str(custom_d011_readiness),
            "--summary-out",
            str(custom_summary),
        ]
    )

    assert args.expectations_doc == custom_expectations
    assert args.packet_doc == custom_packet
    assert args.e014_packet_doc == custom_e014_packet
    assert args.b016_expectations_doc == custom_b016_expectations
    assert args.d011_readiness_script == custom_d011_readiness
    assert args.summary_out == custom_summary


def test_contract_default_summary_out_is_under_tmp_reports_m246_e015() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-E015/")


def test_contract_passes_on_repository_sources_and_summary_contract(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m246-e015-optimization-gate-perf-evidence-advanced-core-workpack-shard-1-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 100
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []
    assert list(payload.keys()) == ["mode", "ok", "checks_total", "checks_passed", "failures"]


def test_contract_fails_closed_when_prerequisite_dependency_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m246_e015_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])
    assert all(
        sorted(failure.keys()) == ["artifact", "check_id", "detail"] for failure in payload["failures"]
    )


def test_contract_fails_closed_when_expectations_drop_pending_c027_token(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_e015_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "| `M246-C027` | Dependency token `M246-C027` is mandatory as pending seeded lane-C advanced core workpack (shard 1) assets. |",
            "| `M246-C099` | Dependency token `M246-C099` is mandatory as pending seeded lane-C advanced core workpack (shard 1) assets. |",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E015-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_line_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m246_e015_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-E014`, `M246-A011`, `M246-B016`, `M246-C027`, `M246-D011`",
            "Dependencies: `M246-E014`, `M246-A011`, `M246-B016`, `M246-C027`, `M246-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E015-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_chain_drops_real_d011_anchor(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m246_e015_lane_e_readiness.py"
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_d011_lane_d_readiness.py",
            "scripts/run_m246_d099_lane_d_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E015-RUN-08" for failure in payload["failures"])


def test_contract_fails_closed_when_b016_dependency_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_b016_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_B016_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-B015`",
            "Dependencies: `M246-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b016-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E015-B016-DOC-03" for failure in payload["failures"])


def test_contract_drift_findings_are_deterministic_across_runs(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_e015_expectations.md"
    drift_runner = tmp_path / "run_m246_e015_lane_e_readiness.py"

    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-optimization-gate-perf-evidence-advanced-core-workpack-shard1/m246-e015-v1`",
            "Contract ID: `objc3c-optimization-gate-perf-evidence-advanced-core-workpack-shard1/m246-e015-drift`",
        ),
        encoding="utf-8",
    )
    drift_runner.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "check:objc3c:m246-c027-lane-c-readiness",
            "check:objc3c:m246-c099-lane-c-readiness",
        ),
        encoding="utf-8",
    )

    summary_one = tmp_path / "summary_one.json"
    summary_two = tmp_path / "summary_two.json"

    exit_one = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--readiness-script",
            str(drift_runner),
            "--summary-out",
            str(summary_one),
        ]
    )
    exit_two = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--readiness-script",
            str(drift_runner),
            "--summary-out",
            str(summary_two),
        ]
    )

    assert exit_one == 1
    assert exit_two == 1

    text_one = summary_one.read_text(encoding="utf-8")
    text_two = summary_two.read_text(encoding="utf-8")
    assert text_one == text_two

    payload_one = json.loads(text_one)
    payload_two = json.loads(text_two)
    assert payload_one["ok"] is False
    assert payload_two["ok"] is False
    assert payload_one["checks_total"] == payload_two["checks_total"]
    assert payload_one["checks_passed"] == payload_two["checks_passed"]
    assert payload_one["failures"] == payload_two["failures"]
