from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_a001_lowering_pipeline_decomposition_pass_graph_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m228-a001-lowering-pipeline-decomposition-pass-graph-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_stage_order_drops_emit(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_header = tmp_path / "frontend_pipeline_contract.h"
    drift_header.write_text(
        contract.ARTIFACTS["pipeline_contract_header"].read_text(encoding="utf-8").replace(
            "StageId::Emit,",
            "StageId::EmitDrift,",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["pipeline_contract_header"] = drift_header
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-A001-CON-09" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emission_anchor_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.ARTIFACTS["artifacts_source"].read_text(encoding="utf-8").replace(
            "if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
            "if (!EmitObjc3IRTextDrift(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {",
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
    assert any(failure["check_id"] == "M228-A001-ART-05" for failure in payload["failures"])
