from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_d002_frontend_build_invocation_modular_scaffold_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_d002_frontend_build_invocation_modular_scaffold_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-frontend-build-invocation-modular-scaffold-contract-d002-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_build_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_script = tmp_path / "build_objc3c_native.ps1"
    drift_script.write_text(
        (
            ROOT / "scripts" / "build_objc3c_native.ps1"
        ).read_text(encoding="utf-8").replace(
            "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-v1",
            "objc3c-frontend-build-invocation-modular-scaffold/m226-d002-drift",
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
    assert any(
        failure["check_id"] == "M226-D002-BLD-03"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_required_module_guard_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_wrapper = tmp_path / "objc3c_native_compile.ps1"
    drift_wrapper.write_text(
        (
            ROOT / "scripts" / "objc3c_native_compile.ps1"
        ).read_text(encoding="utf-8").replace(
            "$requiredModules = @(\"driver\", \"diagnostics-io\", \"ir\", \"lex-parse\", \"frontend-api\", \"lowering\", \"pipeline\", \"sema\")",
            "$requiredModules = @(\"driver\", \"pipeline\")",
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
    assert any(
        failure["check_id"] == "M226-D002-CMP-03"
        for failure in payload["failures"]
    )
