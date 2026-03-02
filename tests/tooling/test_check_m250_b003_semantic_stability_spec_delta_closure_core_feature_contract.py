from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m250_b003_semantic_stability_spec_delta_closure_core_feature_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m250_b003_semantic_stability_spec_delta_closure_core_feature_contract",
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
    assert payload["mode"] == "m250-semantic-stability-spec-delta-closure-core-feature-contract-b003-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m250_semantic_stability_spec_delta_closure_core_feature_implementation_b003_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-implementation/m250-b003-v1`",
            "Contract ID: `objc3c-semantic-stability-spec-delta-closure-core-feature-implementation/m250-b003-drift`",
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
    assert any(failure["check_id"] == "M250-B003-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_core_feature_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_semantic_stability_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8").replace(
            "surface.core_feature_impl_ready =\n"
            "      surface.semantic_handoff_deterministic &&\n"
            "      surface.spec_delta_closed &&\n"
            "      surface.modular_split_ready &&\n"
            "      typed_parse_core_feature_consistent &&\n"
            "      parse_conformance_consistent &&\n"
            "      typed_core_feature_case_accounting_consistent &&\n"
            "      typed_core_feature_expansion_case_accounting_consistent &&\n"
            "      parse_matrix_case_count_ready &&\n"
            "      parse_corpus_case_accounting_consistent &&\n"
            "      parse_guardrails_case_accounting_consistent &&\n"
            "      typed_keys_ready;",
            "surface.core_feature_impl_ready = true;",
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
        failure["check_id"] in {"M250-B003-SUR-04", "M250-B003-FORB-01"}
        for failure in payload["failures"]
    )
