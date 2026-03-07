from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m251_e001_runnable_runtime_foundation_gate_and_evidence_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m251-e001-runnable-runtime-foundation-gate-evidence-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["smoke_positive_runtime_fixture_count"] > 0
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m251_e001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m251/M251-E001/")


def test_contract_fails_closed_when_expectations_doc_drops_d003_dependency(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m251_e001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace("`M251-D003`", "`M251-D099`"),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-E001-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_package_lane_e_readiness_drops_upstream_lane(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            "npm run check:objc3c:m251-c003-lane-c-readiness && ",
            "",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M251-E001-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_d003_smoke_runtime_library_drifts(tmp_path: Path) -> None:
    drift_smoke = tmp_path / "summary.json"
    payload = json.loads(contract.DEFAULT_D003_SMOKE_SUMMARY.read_text(encoding="utf-8"))
    payload["runtime_library"] = "tests/tooling/runtime/objc3_msgsend_i32_shim.c"
    drift_smoke.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "contract_summary.json"
    exit_code = contract.run(["--d003-smoke-summary", str(drift_smoke), "--summary-out", str(summary_out)])

    assert exit_code == 1
    contract_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert contract_payload["ok"] is False
    assert any(failure["check_id"] == "M251-E001-D003-SMOKE-LIB" for failure in contract_payload["failures"])
