from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_a004_feature_packaging_surface_and_compatibility_contracts_core_feature_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m249-a004-feature-packaging-surface-compatibility-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_a004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-A004/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_a004_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M249-A003`",
            "Dependencies: `M249-A099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-A004-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_chain_drops_a003_check(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_a004_lane_a_readiness.py"
    drift_runner.write_text(
        contract.DEFAULT_RUNNER_SCRIPT.read_text(encoding="utf-8").replace(
            "check:objc3c:m249-a003-lane-a-readiness",
            "check:objc3c:m249-a099-lane-a-readiness",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--runner-script", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-A004-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_command_drops_runner(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m249-a004-lane-a-readiness": "python scripts/run_m249_a004_lane_a_readiness.py"',
            '"check:objc3c:m249-a004-lane-a-readiness": '
            '"npm run check:objc3c:m249-a004-feature-packaging-surface-compatibility-core-feature-expansion-contract '
            '&& npm run test:tooling:m249-a004-feature-packaging-surface-compatibility-core-feature-expansion-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-A004-PKG-03" for failure in payload["failures"])
