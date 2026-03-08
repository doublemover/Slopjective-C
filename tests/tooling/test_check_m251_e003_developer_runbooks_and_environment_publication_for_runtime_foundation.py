from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_e003_developer_runbooks_and_environment_publication_for_runtime_foundation.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-e003-runtime-foundation-developer-runbook-environment-publication-v1"
    assert payload["contract_id"] == "objc3c-runtime-foundation-developer-runbook-environment-publication/m251-e003-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_e003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-E003/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m251_e003_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_runbook_drops_build_command(tmp_path: Path) -> None:
    drift_runbook = tmp_path / "m251_e003_runbook.md"
    drift_runbook.write_text(
        contract.DEFAULT_RUNBOOK_DOC.read_text(encoding="utf-8").replace(
            "npm run build:objc3c-native",
            "npm run build-native-drifted",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--runbook-doc", str(drift_runbook), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-E003-RUN-07" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m251-e003-lane-e-readiness": "npm run check:objc3c:m251-e002-lane-e-readiness && npm run check:objc3c:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation && npm run test:tooling:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation"',
            '"check:objc3c:m251-e003-lane-e-readiness": "npm run check:objc3c:m251-e003-developer-runbooks-and-environment-publication-for-runtime-foundation"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-E003-PKG-03" for failure in payload["failures"])
