from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m228_a003_lowering_pipeline_decomposition_pass_graph_core_feature_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m228_a003_lowering_pipeline_decomposition_pass_graph_core_feature_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_a003_lowering_pipeline_decomposition_pass_graph_core_feature_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m228-a003-lowering-pipeline-pass-graph-core-feature-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_core_feature_gate_code_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.ARTIFACTS["artifacts_source"].read_text(encoding="utf-8").replace(
            '"O3L302"',
            '"O3L3ZZ"',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["artifacts_source"] = drift_artifacts
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-A003-ART-03" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_metadata_lines_drop(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.ARTIFACTS["ir_source"].read_text(encoding="utf-8").replace(
            'out << "; lowering_pass_graph_core_feature_ready = "',
            'out << "; lowering_pass_graph_core_feature_ready_drift = "',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["ir_source"] = drift_ir
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-A003-IRS-03" for failure in payload["failures"])
