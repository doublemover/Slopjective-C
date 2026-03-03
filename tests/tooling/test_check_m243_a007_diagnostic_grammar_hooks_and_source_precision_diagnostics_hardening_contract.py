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
    / "check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_a007_diagnostic_grammar_hooks_and_source_precision_diagnostics_hardening_contract",
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
    assert (
        payload["mode"]
        == "m243-diagnostic-grammar-hooks-and-source-precision-diagnostics-hardening-contract-a007-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 18
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_hardening_ready_field_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_types = tmp_path / "objc3_frontend_types.h"
    drift_types.write_text(
        contract.ARTIFACTS["frontend_types"].read_text(encoding="utf-8").replace(
            "bool parser_diagnostic_grammar_hooks_diagnostics_hardening_ready = false;",
            "bool parser_diagnostic_grammar_hooks_diagnostics_hardening_ready = true;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["frontend_types"] = drift_types
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-A007-TYP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_hardening_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_readiness = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_readiness.write_text(
        contract.ARTIFACTS["readiness_surface"].read_text(encoding="utf-8")
        + "\nsurface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready = true;\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["readiness_surface"] = drift_readiness
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M243-A007-RDY-03", "M243-A007-FORB-01"}
        for failure in payload["failures"]
    )
