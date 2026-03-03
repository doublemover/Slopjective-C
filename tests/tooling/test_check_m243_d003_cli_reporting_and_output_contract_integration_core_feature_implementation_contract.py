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
    / "check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp() -> None:
    args = contract.parse_args([])
    assert str(args.summary_out).replace("\\", "/").startswith("tmp/reports/m243/M243-D003/")


def test_contract_fails_closed_when_contract_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = (
        tmp_path
        / "m243_cli_reporting_and_output_contract_integration_core_feature_implementation_d003_expectations.md"
    )
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-core-feature-implementation/m243-d003-v1`",
            "Contract ID: `objc3c-cli-reporting-output-contract-integration-core-feature-implementation/m243-d003-drift`",
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
    assert any(failure["check_id"] == "M243-D003-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_core_surface_forces_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_cli_reporting_output_contract_core_feature_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["core_surface_header"].read_text(encoding="utf-8")
        + "\nsurface.core_feature_impl_ready = true;\n",
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["core_surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] in {"M243-D003-SUR-08", "M243-D003-FORB-01"}
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_package_readiness_chain_drops_d002(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m243-d003-lane-d-readiness": '
            '"npm run check:objc3c:m243-d002-lane-d-readiness '
            "&& npm run check:objc3c:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract "
            '&& npm run test:tooling:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract"',
            '"check:objc3c:m243-d003-lane-d-readiness": '
            '"npm run check:objc3c:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract '
            '&& npm run test:tooling:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract"',
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
    assert any(failure["check_id"] == "M243-D003-CFG-03" for failure in payload["failures"])
