from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m245_a011_frontend_behavior_parity_across_toolchains_integration_closeout_and_gate_signoff_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_a011() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-A011/")


def test_contract_emit_json_writes_summary_to_stdout(tmp_path: Path, capsys) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["ok"] is True
    assert payload["checks_passed"] == payload["checks_total"]
    assert json.loads(summary_out.read_text(encoding="utf-8")) == payload


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_a011_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M245-A010`",
            "Dependencies: `M245-A099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-A011-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_a010(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m245-a011-lane-a-readiness": '
            '"npm run check:objc3c:m245-a010-lane-a-readiness '
            '&& npm run check:objc3c:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract '
            '&& npm run test:tooling:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract"',
            '"check:objc3c:m245-a011-lane-a-readiness": '
            '"npm run check:objc3c:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract '
            '&& npm run test:tooling:m245-a011-frontend-behavior-parity-toolchains-integration-closeout-and-gate-signoff-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-A011-PKG-03" for failure in payload["failures"])



