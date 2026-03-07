from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m236_c027_arc_style_lowering_insertion_and_cleanup_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m236_c027_arc_style_lowering_insertion_and_cleanup_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m236_c027_arc_style_lowering_insertion_and_cleanup_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m236-c027-arc-style-lowering-insertion-and-cleanup-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m236_c027() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m236/M236-C027/")


def test_contract_fails_closed_when_expectations_drop_issue_anchor(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m236_arc_style_lowering_insertion_and_cleanup_advanced_core_workpack_shard_3_c027_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Issue `#5918` defines canonical lane-C advanced core workpack (shard 3) scope.",
            "Issue `#5918` defines canonical lane-C scope.",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M236-C027-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m236-c027-lane-c-readiness": '
            '"npm run check:objc3c:m236-c027-arc-style-lowering-insertion-and-cleanup-contract '
            '&& npm run test:tooling:m236-c027-arc-style-lowering-insertion-and-cleanup-contract"',
            '"check:objc3c:m236-c027-lane-c-readiness": '
            '"npm run check:objc3c:m236-c027-arc-style-lowering-insertion-and-cleanup-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M236-C027-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_lowering_anchor_drifts(tmp_path: Path) -> None:
    drift_spec = tmp_path / "LOWERING_AND_RUNTIME_CONTRACTS.md"
    drift_spec.write_text(
        contract.DEFAULT_LOWERING_SPEC.read_text(encoding="utf-8").replace(
            "qualified type lowering and ABI representation governance shall preserve deterministic lane-C",
            "arc-style lowering insertion and cleanup governance shall preserve deterministic lane-C",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--lowering-spec", str(drift_spec), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M236-C027-SPC-01" for failure in payload["failures"])




























