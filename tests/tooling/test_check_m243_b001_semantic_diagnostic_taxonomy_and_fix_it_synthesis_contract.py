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
    / "check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract",
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
    assert payload["mode"] == "m243-semantic-diagnostic-taxonomy-and-fixit-synthesis-freeze-contract-b001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 20
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_b001_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-freeze/m243-b001-v1`",
            "Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-freeze/m243-b001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-B001-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_scaffold_drops_deterministic_diagnostics_param(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_scaffold = tmp_path / "objc3_sema_pass_flow_scaffold.h"
    drift_scaffold.write_text(
        contract.ARTIFACTS["sema_scaffold"].read_text(encoding="utf-8").replace(
            "    bool deterministic_semantic_diagnostics,",
            "    bool semantic_diagnostics_flag_removed,",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_scaffold"] = drift_scaffold
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-B001-SCF-03" for failure in payload["failures"])


def test_contract_fails_closed_when_pipeline_drops_semantic_diagnostics_projection(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_typed_sema_to_lowering_contract_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["typed_surface"].read_text(encoding="utf-8").replace(
            "pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics &&",
            "pipeline_result.sema_parity_surface.semantic_diagnostics_dropped &&",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["typed_surface"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-B001-TYP-01" for failure in payload["failures"])
