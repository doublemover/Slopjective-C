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
    / "check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m243_c007_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_contract.py"
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
        == "m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m243_c007() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m243/M243-C007/")


def test_contract_emit_json_prints_summary_payload(
    tmp_path: Path, capsys: pytest.CaptureFixture[str]
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--emit-json", "--summary-out", str(summary_out)])

    assert exit_code == 0
    captured = capsys.readouterr()
    emitted = json.loads(captured.out)
    assert emitted["mode"] == contract.MODE
    assert emitted["ok"] is True
    assert emitted["checks_total"] == emitted["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_c007_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["expectations_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: `M243-C006`",
            "Dependencies: `M243-C099`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["expectations_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-C007-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_c006(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m243-c007-lane-c-readiness": '
            '"npm run check:objc3c:m243-c006-lane-c-readiness '
            '&& npm run check:objc3c:m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract '
            '&& npm run test:tooling:m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract"',
            '"check:objc3c:m243-c007-lane-c-readiness": '
            '"npm run check:objc3c:m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract '
            '&& npm run test:tooling:m243-c007-lowering-runtime-diagnostics-surfacing-diagnostics-hardening-contract"',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["package_json"] = drift_package
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-C007-CFG-04" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_diagnostics_hardening_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = (
        tmp_path
        / "objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
    )
    drift_surface.write_text(
        contract.ARTIFACTS["surface_header"].read_text(encoding="utf-8")
        + "\nsurface.diagnostics_hardening_ready = true;\n",
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["surface_header"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-C007-FORB-01" for failure in payload["failures"])
