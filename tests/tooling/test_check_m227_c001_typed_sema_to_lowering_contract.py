from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_c001_typed_sema_to_lowering_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_c001_typed_sema_to_lowering_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m227_c001_typed_sema_to_lowering_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m227-c001-typed-sema-to-lowering-contract-and-architecture-freeze-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 80
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m227_c001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m227/M227-C001/")


def test_contract_fails_closed_when_pipeline_drops_type_metadata_handoff(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    pipeline_drift = tmp_path / "objc3_frontend_pipeline.cpp"
    pipeline_drift.write_text(
        contract.ARTIFACTS["pipeline_source"]
        .read_text(encoding="utf-8")
        .replace(
            "result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);",
            "result.sema_type_metadata_handoff = Objc3SemanticTypeMetadataHandoff{};",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["pipeline_source"] = pipeline_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C001-PIP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    package_drift = tmp_path / "package.json"
    package_drift.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m227-c001-lane-c-readiness": "npm run check:objc3c:m227-c001-typed-sema-to-lowering-contract && npm run test:tooling:m227-c001-typed-sema-to-lowering-contract"',
            '"check:objc3c:m227-c001-lane-c-readiness": "npm run check:objc3c:m227-c001-typed-sema-to-lowering-contract"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["package_json"] = package_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_anchor_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    architecture_drift = tmp_path / "ARCHITECTURE.md"
    architecture_drift.write_text(
        contract.ARTIFACTS["architecture_doc"]
        .read_text(encoding="utf-8")
        .replace(
            "M227 lane-C C001 typed sema-to-lowering contracts contract and architecture freeze anchors",
            "M227 lane-C C001 typed sema-to-lowering contracts freeze anchors",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["architecture_doc"] = architecture_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-C001-ARCH-01" for failure in payload["failures"])
