from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m258_a001_runtime_aware_import_module_surface_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m258_a001_runtime_aware_import_module_surface_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m258_a001_runtime_aware_import_module_surface_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_checker_passes_static_contract(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m258-a001-runtime-aware-import-module-surface-contract-v1"
    assert payload["contract_id"] == "objc3c-runtime-aware-import-module-surface/m258-a001-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["dynamic_probes_executed"] is False
    assert payload["failures"] == []


def test_default_summary_out_is_under_tmp_reports_m258_a001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.endswith("/tmp/reports/m258/M258-A001/runtime_aware_import_module_surface_contract_summary.json")


def test_checker_fails_when_expectations_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-runtime-aware-import-module-surface/m258-a001-v1`",
            "Contract ID: `objc3c-runtime-aware-import-module-surface/m258-a001-v9`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--skip-dynamic-probes",
        "--expectations-doc",
        str(drift_doc),
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M258-A001-DOC-EXP-02" for failure in payload["failures"])


def test_checker_fails_when_frontend_api_anchor_drifts(tmp_path: Path) -> None:
    drift_api = tmp_path / "api.h"
    drift_api.write_text(
        contract.FRONTEND_API.read_text(encoding="utf-8").replace(
            "M258-A001 runtime-aware import/module surface anchor",
            "M258-A001 runtime aware import/module surface anchor",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--skip-dynamic-probes",
        "--frontend-api",
        str(drift_api),
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M258-A001-API-01" for failure in payload["failures"])
