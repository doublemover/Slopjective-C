from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_d002_runtime_facing_type_metadata_modular_split_contract",
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
    assert payload["mode"] == "m227-d002-runtime-facing-type-metadata-modular-split-scaffold-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m227/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m227_runtime_facing_type_metadata_modular_split_d002_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"]
        .read_text(encoding="utf-8")
        .replace(
            "Contract ID: `objc3c-runtime-facing-type-metadata-modular-split-scaffold/m227-d002-v1`",
            "Contract ID: `objc3c-runtime-facing-type-metadata-modular-split-scaffold/m227-d002-drift`",
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
    assert any(
        failure["check_id"] == "M227-D002-DOC-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_sema_manager_drops_handoff_scaffold_call(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_sema_pass_manager.cpp"
    drift_source.write_text(
        contract.ARTIFACTS["sema_pass_manager_source"]
        .read_text(encoding="utf-8")
        .replace(
            "  const Objc3ParserSemaHandoffScaffold handoff = BuildObjc3ParserSemaHandoffScaffold(input);\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_pass_manager_source"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M227-D002-MGR-03", "M227-D002-SEM-03"}
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_runtime_shim_projection_is_removed(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.ARTIFACTS["frontend_artifacts_source"]
        .read_text(encoding="utf-8")
        .replace(
            "  ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff =",
            "  // drift: deterministic_runtime_shim_host_link_handoff projection removed",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["frontend_artifacts_source"] = drift_artifacts
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M227-D002-ART-05"
        for failure in payload["failures"]
    )
