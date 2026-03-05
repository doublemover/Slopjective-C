from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m234_d001_runtime_property_metadata_integration_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m234_d001_runtime_property_metadata_integration_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m234_d001_runtime_property_metadata_integration_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m234-d001-runtime-property-metadata-integration-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m234_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m234/M234-D001/")


def test_contract_fails_closed_when_expectations_drop_traceability_wording(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m234_runtime_property_metadata_integration_contract_and_architecture_freeze_d001_expectations.md"
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
    assert any(failure["check_id"] == "M234-D001-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drops_tooling_test(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m234-d001-lane-d-readiness": '
            '"npm run check:objc3c:m234-d001-runtime-property-metadata-integration-contract-and-architecture-freeze-contract '
            '&& npm run test:tooling:m234-d001-runtime-property-metadata-integration-contract-and-architecture-freeze-contract"',
            '"check:objc3c:m234-d001-lane-d-readiness": '
            '"npm run check:objc3c:m234-d001-runtime-property-metadata-integration-contract-and-architecture-freeze-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_metadata_anchor_token_drifts(tmp_path: Path) -> None:
    drift_metadata = tmp_path / "MODULE_METADATA_AND_ABI_TABLES.md"
    drift_metadata.write_text(
        contract.DEFAULT_METADATA_SPEC.read_text(encoding="utf-8").replace(
            "deterministic lane-D runtime property metadata integration metadata anchors for `M234-D001`",
            "deterministic lane-D runtime property metadata integration metadata anchors for `M234-D001-DRIFT`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--metadata-spec", str(drift_metadata), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-D001-META-01" for failure in payload["failures"])
