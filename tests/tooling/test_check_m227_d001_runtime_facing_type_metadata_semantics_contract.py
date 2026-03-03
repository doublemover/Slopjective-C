from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_d001_runtime_facing_type_metadata_semantics_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_d001_runtime_facing_type_metadata_semantics_contract",
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
    assert payload["mode"] == "m227-d001-runtime-facing-type-metadata-semantics-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m227/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m227_runtime_facing_type_metadata_semantics_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"]
        .read_text(encoding="utf-8")
        .replace(
            "Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`",
            "Contract ID: `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-drift`",
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
        failure["check_id"] == "M227-D001-DOC-02"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_runtime_dispatch_symbol_drifts_between_layers(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_pipeline_contract = tmp_path / "frontend_pipeline_contract.h"
    drift_pipeline_contract.write_text(
        contract.ARTIFACTS["frontend_pipeline_contract_header"]
        .read_text(encoding="utf-8")
        .replace(
            'inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";',
            'inline constexpr const char *kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i64";',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["frontend_pipeline_contract_header"] = drift_pipeline_contract
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M227-D001-SEM-02", "M227-D001-SEM-03"}
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_ir_runtime_shim_projection_is_removed(
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
        failure["check_id"] == "M227-D001-ART-08"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_package_lane_d_readiness_drops_tooling_test(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"]
        .read_text(encoding="utf-8")
        .replace(
            '"check:objc3c:m227-d001-lane-d-readiness": '
            '"npm run check:objc3c:m227-d001-runtime-facing-type-metadata-semantics-contract '
            '&& npm run test:tooling:m227-d001-runtime-facing-type-metadata-semantics-contract"',
            '"check:objc3c:m227-d001-lane-d-readiness": '
            '"npm run check:objc3c:m227-d001-runtime-facing-type-metadata-semantics-contract"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["package_json"] = drift_package
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M227-D001-PKG-03"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_architecture_anchor_phrase_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        contract.ARTIFACTS["architecture_doc"]
        .read_text(encoding="utf-8")
        .replace(
            "architecture freeze anchors explicit lane-D contract-freeze artifacts in",
            "architecture freeze anchors explicit lane-D artifacts in",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["architecture_doc"] = drift_architecture
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M227-D001-ARC-01"
        for failure in payload["failures"]
    )
