from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m242_c012_expanded_source_lowering_traceability_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m242_c012_expanded_source_lowering_traceability_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m242_c012_expanded_source_lowering_traceability_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m242-c012-expanded-source-lowering-traceability-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m242_c012() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m242/M242-C012/")


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m242_expanded_source_lowering_traceability_cross_lane_integration_sync_c012_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#6366` defines canonical lane-C cross-lane integration sync scope.",
            "Issue `#9999` defines canonical lane-C cross-lane integration sync scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M242-C012-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m242-c012-lane-c-readiness": '
            '"npm run check:objc3c:m242-c012-expanded-source-lowering-traceability-contract '
            '&& npm run test:tooling:m242-c012-expanded-source-lowering-traceability-contract"',
            '"check:objc3c:m242-c012-lane-c-readiness": '
            '"npm run check:objc3c:m242-c012-expanded-source-lowering-traceability-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M242-C012-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_lowering_anchor_drifts(tmp_path: Path) -> None:
    drift_spec = tmp_path / "LOWERING_AND_RUNTIME_CONTRACTS.md"
    drift_spec.write_text(
        contract.DEFAULT_LOWERING_SPEC.read_text(encoding="utf-8").replace(
            "qualified type lowering and ABI representation governance shall preserve deterministic lane-C",
            "expanded-source lowering traceability governance shall preserve deterministic lane-C",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--lowering-spec", str(drift_spec), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M242-C012-SPC-01" for failure in payload["failures"])













