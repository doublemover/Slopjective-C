from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m249_d001_installer_runtime_operations_and_support_tooling_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m249_d001_installer_runtime_operations_and_support_tooling_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m249_d001_installer_runtime_operations_and_support_tooling_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m249-d001-installer-runtime-operations-support-tooling-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 28
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-D001/")


def test_contract_fails_closed_when_expectations_drop_traceability_wording(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_installer_runtime_operations_and_support_tooling_contract_and_architecture_freeze_d001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "optimization improvements as mandatory scope inputs.",
            "improvements as mandatory scope inputs.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-D001-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drops_tooling_test(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m249-d001-lane-d-readiness": '
            '"npm run check:objc3c:m249-d001-installer-runtime-operations-support-tooling-contract '
            '&& npm run test:tooling:m249-d001-installer-runtime-operations-support-tooling-contract"',
            '"check:objc3c:m249-d001-lane-d-readiness": '
            '"npm run check:objc3c:m249-d001-installer-runtime-operations-support-tooling-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-D001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_metadata_anchor_m249_token_drifts(tmp_path: Path) -> None:
    drift_metadata = tmp_path / "MODULE_METADATA_AND_ABI_TABLES.md"
    drift_metadata.write_text(
        contract.DEFAULT_METADATA_SPEC.read_text(encoding="utf-8").replace(
            "deterministic lane-D installer/runtime operations metadata anchors for `M249-D001`",
            "deterministic lane-D installer/runtime operations metadata anchors for `M249-D001-DRIFT`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--metadata-spec", str(drift_metadata), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-D001-META-01" for failure in payload["failures"])
