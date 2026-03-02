from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m250_b001_semantic_stability_spec_delta_closure_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m250_b001_semantic_stability_spec_delta_closure_contract",
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
    assert payload["mode"] == "m250-semantic-stability-spec-delta-closure-contract-freeze-b001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m250_semantic_stability_spec_delta_closure_contract_freeze_expectations.md"
    drift_doc.write_text(
        (
            ROOT
            / "docs"
            / "contracts"
            / "m250_semantic_stability_spec_delta_closure_contract_freeze_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-freeze/m250-b001-v1`",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-freeze/m250-b001-drift`",
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
    assert any(failure["check_id"] == "M250-B001-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_typed_surface_forces_handoff_to_true(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_typed_sema_to_lowering_contract_surface.h"
    drift_surface.write_text(
        (
            ROOT
            / "native"
            / "objc3c"
            / "src"
            / "pipeline"
            / "objc3_typed_sema_to_lowering_contract_surface.h"
        ).read_text(encoding="utf-8").replace(
            "surface.typed_handoff_key_deterministic =\n"
            "      surface.semantic_handoff_deterministic &&\n"
            "      surface.runtime_dispatch_contract_consistent &&\n"
            "      surface.lowering_boundary_ready &&\n"
            "      !surface.typed_handoff_key.empty();",
            "surface.typed_handoff_key_deterministic = true;",
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
    assert any(
        failure["check_id"] in {"M250-B001-SUR-06", "M250-B001-FORB-01"}
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_parse_readiness_forces_ready_to_true(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_readiness = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_readiness.write_text(
        (
            ROOT
            / "native"
            / "objc3c"
            / "src"
            / "pipeline"
            / "objc3_parse_lowering_readiness_surface.h"
        ).read_text(encoding="utf-8").replace(
            "surface.ready_for_lowering = diagnostics_clear &&",
            "surface.ready_for_lowering = true;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parse_readiness"] = drift_readiness
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M250-B001-REA-07", "M250-B001-FORB-02"}
        for failure in payload["failures"]
    )
