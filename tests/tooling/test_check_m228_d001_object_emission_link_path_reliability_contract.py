from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m228_d001_object_emission_link_path_reliability_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m228_d001_object_emission_link_path_reliability_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_d001_object_emission_link_path_reliability_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m228-d001-object-emission-link-path-reliability-freeze-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_llc_missing_error_contract_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_io = tmp_path / "objc3_process.cpp"
    drift_io.write_text(
        contract.ARTIFACTS["io_process_source"].read_text(encoding="utf-8").replace(
            "llvm-direct object emission failed: llc executable not found:",
            "llvm-direct object emission failed: llc missing:",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["io_process_source"] = drift_io
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-D001-IOCPP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_anchor_route_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_anchor = tmp_path / "frontend_anchor.cpp"
    drift_anchor.write_text(
        contract.ARTIFACTS["frontend_anchor_source"].read_text(encoding="utf-8").replace(
            "RunIRCompileLLVMDirect(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error)",
            "RunIRCompileLLVMDirect_Drift(std::filesystem::path(options->llc_path), ir_out, object_out, backend_error)",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["frontend_anchor_source"] = drift_anchor
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-D001-ANC-02" for failure in payload["failures"])
