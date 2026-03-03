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
    / "check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_fails_closed_when_a012_doc_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m228_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_a012_expectations.md"
    )
    drift_doc.write_text(
        (
            ROOT
            / "docs"
            / "contracts"
            / "m228_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_a012_expectations.md"
        )
        .read_text(encoding="utf-8")
        .replace(
            "objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1",
            "objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-drift",
            1,
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(contract, "CONTRACT_DOC", drift_doc)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M228-A012-LINK-D006" for failure in payload["failures"]
    )


def test_contract_fails_closed_when_lane_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_lane = (
        tmp_path
        / "m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md"
    )
    drift_lane.write_text(
        (
            ROOT
            / "docs"
            / "contracts"
            / "m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md"
        )
        .read_text(encoding="utf-8")
        .replace(
            "Contract ID: `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`",
            "Contract ID: `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-drift`",
            1,
        ),
        encoding="utf-8",
    )
    lane_contracts = list(contract.LANE_CONTRACTS)
    for idx, item in enumerate(lane_contracts):
        packet, contract_id, path = item
        if packet == "C005":
            lane_contracts[idx] = (packet, contract_id, drift_lane)
            break
    monkeypatch.setattr(contract, "LANE_CONTRACTS", tuple(lane_contracts))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M228-A012-LANE-C005"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_lane_contract_file_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    lane_contracts = list(contract.LANE_CONTRACTS)
    for idx, item in enumerate(lane_contracts):
        packet, contract_id, path = item
        if packet == "B007":
            lane_contracts[idx] = (
                packet,
                contract_id,
                tmp_path / "missing_m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md",
            )
            break
    monkeypatch.setattr(contract, "LANE_CONTRACTS", tuple(lane_contracts))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M228-A012-LANE-B007-00"
        for failure in payload["failures"]
    )
