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
    / "check_m226_d009_frontend_build_invocation_conformance_matrix_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m226_d009_frontend_build_invocation_conformance_matrix_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m226_d009_frontend_build_invocation_conformance_matrix_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-frontend-build-invocation-conformance-matrix-contract-d009-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_build_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_script = tmp_path / "build_objc3c_native.ps1"
    drift_script.write_text(
        (
            ROOT / "scripts" / "build_objc3c_native.ps1"
        ).read_text(encoding="utf-8").replace(
            "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-v1",
            "objc3c-frontend-build-invocation-conformance-matrix/m226-d009-drift",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["build_script"] = drift_script
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-D009-BLD-02" for failure in payload["failures"])


def test_contract_fails_closed_when_wrapper_assert_call_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_wrapper = tmp_path / "objc3c_native_compile.ps1"
    drift_wrapper.write_text(
        (
            ROOT / "scripts" / "objc3c_native_compile.ps1"
        ).read_text(encoding="utf-8").replace(
            "Assert-FrontendConformanceMatrix `",
            "Assert-FrontendConformanceMatrixDisabled `",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["compile_wrapper"] = drift_wrapper
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-D009-CMP-13" for failure in payload["failures"])


def test_contract_fails_closed_when_profile_gate_diagnostic_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_wrapper = tmp_path / "objc3c_native_compile.ps1"
    drift_wrapper.write_text(
        (
            ROOT / "scripts" / "objc3c_native_compile.ps1"
        ).read_text(encoding="utf-8").replace(
            "frontend conformance matrix has no acceptance row for invocation profile",
            "frontend conformance matrix profile mismatch",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["compile_wrapper"] = drift_wrapper
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-D009-CMP-07" for failure in payload["failures"])
