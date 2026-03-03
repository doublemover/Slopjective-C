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
    / "check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py"
    )
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
        == "m228-lowering-pipeline-pass-graph-release-replay-dry-run-contract-a014-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m228_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_a014_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-release-replay-dry-run/m228-a014-v1`",
            "Contract ID: `objc3c-lowering-pipeline-pass-graph-release-replay-dry-run/m228-a014-drift`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-A014-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_run_script_drops_anchor(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_script = (
        tmp_path
        / "run_m228_a014_lowering_pipeline_decomposition_pass_graph_release_replay_dry_run.ps1"
    )
    drift_script.write_text(
        contract.ARTIFACTS["run_script"].read_text(encoding="utf-8").replace(
            "manifest parse_lowering_readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready is false",
            "manifest parse_lowering_readiness.toolchain_runtime_ga_operations_docs_runbook_sync_ready_drift is false",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["run_script"] = drift_script
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-A014-RUN-15" for failure in payload["failures"])


def test_contract_fails_closed_when_package_wiring_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m228-a014-lane-a-readiness"',
            '"check:objc3c:m228-a014-lane-a-readiness-drift"',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["package_json"] = drift_package
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M228-A014-CFG-04" for failure in payload["failures"])
