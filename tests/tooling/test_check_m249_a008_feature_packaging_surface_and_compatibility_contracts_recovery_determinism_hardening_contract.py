from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m249_a008_feature_packaging_surface_and_compatibility_contracts_recovery_determinism_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_a008_feature_packaging_surface_and_compatibility_contracts_recovery_determinism_hardening_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_a008_feature_packaging_surface_and_compatibility_contracts_recovery_determinism_hardening_contract.py"
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
        == "m249-a008-feature-packaging-surface-compatibility-recovery-determinism-hardening-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_a008() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-A008/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_a008_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M249-A007`",
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
    assert any(failure["check_id"] == "M249-A008-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_a007_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_a007_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_A007_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-feature-packaging-surface-compatibility-contracts-diagnostics-hardening/m249-a007-v1`",
            "Contract ID: `objc3c-feature-packaging-surface-compatibility-contracts-diagnostics-hardening/m249-a007-drift`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        ["--a007-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-A008-A007-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_metadata_anchor_drifts(tmp_path: Path) -> None:
    drift_metadata = tmp_path / "MODULE_METADATA_AND_ABI_TABLES.md"
    drift_metadata.write_text(
        contract.DEFAULT_METADATA_SPEC.read_text(encoding="utf-8").replace(
            "deterministic lane-A feature packaging core feature metadata anchors for `M249-A003`",
            "deterministic lane-A feature packaging core feature metadata anchors for `M249-A999`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--metadata-spec", str(drift_metadata), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-A008-META-01" for failure in payload["failures"])


def test_contract_fails_closed_when_runner_chain_drops_a007_check(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_a008_lane_a_readiness.py"
    drift_runner.write_text(
        contract.DEFAULT_RUNNER_SCRIPT.read_text(encoding="utf-8").replace(
            "check:objc3c:m249-a007-lane-a-readiness",
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
    assert any(failure["check_id"] == "M249-A008-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_command_drops_runner(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m249-a008-lane-a-readiness": "python scripts/run_m249_a008_lane_a_readiness.py"',
            '"check:objc3c:m249-a008-lane-a-readiness": '
            '"npm run check:objc3c:m249-a007-lane-a-readiness '
            '&& npm run check:objc3c:m249-a008-feature-packaging-surface-compatibility-recovery-determinism-hardening-contract '
            '&& npm run test:tooling:m249-a008-feature-packaging-surface-compatibility-recovery-determinism-hardening-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-A008-PKG-03" for failure in payload["failures"])
