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
    / "check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_c003_lowering_runtime_diagnostics_surfacing_core_feature_implementation_contract",
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
        == "m243-lowering-runtime-diagnostics-surfacing-core-feature-implementation-contract-c003-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 32
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_lowering_runtime_diagnostics_surfacing_core_feature_implementation_c003_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-core-feature-implementation/m243-c003-v1`",
            "Contract ID: `objc3c-lowering-runtime-diagnostics-surfacing-core-feature-implementation/m243-c003-drift`",
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
    assert any(failure["check_id"] == "M243-C003-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_core_feature_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8").replace(
            "  surface.core_feature_impl_ready =\n"
            "      surface.stage_diagnostics_bus_consistent &&\n"
            "      surface.parse_readiness_surface_ready &&\n"
            "      surface.diagnostics_surfacing_scaffold_ready &&\n"
            "      surface.parser_diagnostic_surface_consistent &&\n"
            "      surface.parser_diagnostic_code_surface_deterministic &&\n"
            "      surface.semantic_diagnostics_deterministic &&\n"
            "      surface.diagnostics_hardening_consistent &&\n"
            "      surface.diagnostics_hardening_ready &&\n"
            "      surface.replay_keys_ready &&\n"
            "      surface.lowering_pipeline_ready;",
            "  surface.core_feature_impl_ready = true;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["core_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M243-C003-SUR-05", "M243-C003-FORB-01"}
        for failure in payload["failures"]
    )
