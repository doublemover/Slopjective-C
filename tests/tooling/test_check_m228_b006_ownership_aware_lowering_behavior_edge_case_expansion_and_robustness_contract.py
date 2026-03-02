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
    / "check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py"
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
        == "m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_edge_case_gate_diagnostic_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.ARTIFACTS["artifacts_source"].read_text(encoding="utf-8").replace(
            '"O3L312"',
            '"O3L31Y"',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["artifacts_source"] = drift_artifacts
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-B006-ART-02" for failure in payload["failures"])


def test_contract_fails_closed_when_forbidden_robustness_shortcut_appears(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_scaffold = tmp_path / "objc3_ownership_aware_lowering_behavior_scaffold.h"
    drift_scaffold.write_text(
        contract.ARTIFACTS["scaffold_header"].read_text(encoding="utf-8")
        + "\nscaffold.edge_case_robustness_ready = true;\n",
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["scaffold_header"] = drift_scaffold
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-B006-FORB-01" for failure in payload["failures"])


def test_contract_fails_closed_when_planning_packet_dependency_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_packet = tmp_path / "m228_b006_packet.md"
    drift_packet.write_text(
        contract.ARTIFACTS["planning_packet"].read_text(encoding="utf-8").replace(
            "Dependencies: `M228-B005`",
            "Dependencies: `M228-B099`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["planning_packet"] = drift_packet
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-B006-PKT-04" for failure in payload["failures"])
