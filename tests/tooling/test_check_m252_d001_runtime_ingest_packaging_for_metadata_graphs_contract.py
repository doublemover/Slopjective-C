from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _runner_is_stale() -> bool:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        return True
    runner_mtime = contract.DEFAULT_RUNNER_EXE.stat().st_mtime
    freshness_inputs = (
        contract.DEFAULT_AST_HEADER,
        contract.DEFAULT_FRONTEND_TYPES,
        contract.DEFAULT_FRONTEND_ARTIFACTS,
    )
    return any(path.stat().st_mtime > runner_mtime for path in freshness_inputs)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-d001-runtime-ingest-packaging-for-metadata-graphs-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-D001/")


def test_contract_fails_closed_when_expectations_drop_payload_model(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_runtime_ingest_packaging_for_metadata_graphs_contract_and_architecture_freeze_d001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`typed-handoff-plus-debug-projection-manifest-v1`",
            "`typed-handoff-plus-debug-projection-manifest-v2`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--skip-runner-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-D001-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_artifacts_drop_packaging_surface(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "objc_executable_metadata_runtime_ingest_packaging_contract",
            "objc_executable_metadata_runtime_ingest_contract",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-artifacts",
            str(drift_artifacts),
            "--skip-runner-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-D001-ART-03" for failure in payload["failures"])


def test_dynamic_probe_covers_runtime_ingest_packaging_manifest_packet(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("frontend runner is not built")
    if _runner_is_stale():
        pytest.skip("frontend runner is older than the D001 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["fixture"] == "tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3"
    assert case["manifest_path"].endswith("module.manifest.json")
    assert case["typed_handoff_replay_key"]
    assert case["debug_projection_replay_key"]
    assert case["replay_key"]
