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
    / "check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m250_c002_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_contract",
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
        == "m250-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding-contract-c002-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 25
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m250_lowering_runtime_stability_invariant_proofs_modular_split_scaffolding_c002_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding/m250-c002-v1`",
            "Contract ID: `objc3c-lowering-runtime-stability-invariant-proofs-modular-split-scaffolding/m250-c002-drift`",
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
    assert any(failure["check_id"] == "M250-C002-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_scaffold_forces_modular_split_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_scaffold = tmp_path / "objc3_lowering_runtime_stability_invariant_scaffold.h"
    drift_scaffold.write_text(
        contract.ARTIFACTS["scaffold_header"].read_text(encoding="utf-8").replace(
            "scaffold.modular_split_ready =\n"
            "      scaffold.typed_surface_present &&\n"
            "      scaffold.parse_readiness_surface_present &&\n"
            "      scaffold.invariant_proofs_ready;",
            "scaffold.modular_split_ready = true;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["scaffold_header"] = drift_scaffold
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M250-C002-SCA-04", "M250-C002-FORB-02"}
        for failure in payload["failures"]
    )
