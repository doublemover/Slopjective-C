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
    / "check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract.py"
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
        == "m228-d016-object-emission-link-path-reliability-integration-closeout-and-gate-signoff-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 65
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m228_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_d016_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-object-emission-link-path-reliability-integration-closeout-and-gate-signoff/m228-d016-v1`",
            "Contract ID: `objc3c-object-emission-link-path-reliability-integration-closeout-and-gate-signoff/m228-d016-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-D016-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_forbidden_guardrail_ready_shortcut_appears(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8")
        + "\nsurface.integration_closeout_and_gate_signoff_ready = true;\n",
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["core_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-D016-FORB-01" for failure in payload["failures"])


def test_contract_fails_closed_when_planning_packet_dependency_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_packet = (
        tmp_path
        / "m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_packet.md"
    )
    drift_packet.write_text(
        contract.ARTIFACTS["planning_packet"].read_text(encoding="utf-8").replace(
            "Dependencies: `M228-D015`",
            "Dependencies: `M228-D099`",
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
    assert any(failure["check_id"] == "M228-D016-PKT-04" for failure in payload["failures"])





