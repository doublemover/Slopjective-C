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
    / "check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m244_a012_interop_surface_syntax_and_declaration_forms_cross_lane_integration_sync_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m244-a012-interop-surface-syntax-and-declaration-forms-cross-lane-integration-sync-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 44
    assert payload["checks_total"] == payload["checks_passed"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m244_a012() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m244/M244-A012/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    payload = json.loads(stdout)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m244_a012_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M244-A011`, `M244-B007`, `M244-C007`, `M244-D004`, `M244-E006`",
            "Dependencies: `M244-A011`, `M244-B007`, `M244-C007`, `M244-D099`, `M244-E006`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M244-A012-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_lane_contract_file_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    lane_contracts = list(contract.LANE_CONTRACTS)
    for idx, lane_contract in enumerate(lane_contracts):
        if lane_contract.lane_task == "D004":
            lane_contracts[idx] = contract.LaneContract(
                lane_task=lane_contract.lane_task,
                contract_id=lane_contract.contract_id,
                relative_path=Path("docs/contracts/missing_m244_runtime_link_bridge_path_core_feature_expansion_d004_expectations.md"),
                doc_link_check_id=lane_contract.doc_link_check_id,
                lane_contract_check_id=lane_contract.lane_contract_check_id,
            )
            break
    monkeypatch.setattr(contract, "LANE_CONTRACTS", tuple(lane_contracts))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M244-A012-LANE-D004-00" for failure in payload["failures"])


def test_contract_fails_closed_when_package_chain_drops_a011_readiness(tmp_path: Path) -> None:
    drift_pkg = tmp_path / "package.json"
    package_payload = json.loads(contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = package_payload["scripts"]
    scripts["check:objc3c:m244-a012-lane-a-readiness"] = scripts[
        "check:objc3c:m244-a012-lane-a-readiness"
    ].replace(
        "npm run check:objc3c:m244-a011-lane-a-readiness && ",
        "",
        1,
    )
    drift_pkg.write_text(json.dumps(package_payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_pkg), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M244-A012-CFG-06" for failure in payload["failures"])
