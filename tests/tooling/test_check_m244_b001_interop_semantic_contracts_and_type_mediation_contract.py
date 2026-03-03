from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m244_b001_interop_semantic_contracts_and_type_mediation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m244_b001_interop_semantic_contracts_and_type_mediation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m244-b001-interop-semantic-contracts-type-mediation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 60
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m244_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m244/M244-B001/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    payload = json.loads(stdout)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_dependency_token_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m244_b001_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["expectations_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: none",
            "Dependencies: `M244-B999`",
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
    assert any(failure["check_id"] == "M244-B001-DOC-03" for failure in payload["failures"])


def test_contract_fails_closed_when_typed_handoff_failure_message_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_typed = tmp_path / "objc3_typed_sema_to_lowering_contract_surface.h"
    drift_typed.write_text(
        contract.ARTIFACTS["typed_surface"].read_text(encoding="utf-8").replace(
            'surface.failure_reason = "typed handoff key is not deterministic";',
            'surface.failure_reason = "typed handoff key is unknown";',
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["typed_surface"] = drift_typed
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M244-B001-TYP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_drops_tooling_test(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m244-b001-lane-b-readiness": '
            '"npm run check:objc3c:m244-b001-interop-semantic-contracts-type-mediation-contract '
            '&& npm run test:tooling:m244-b001-interop-semantic-contracts-type-mediation-contract"',
            '"check:objc3c:m244-b001-lane-b-readiness": '
            '"npm run check:objc3c:m244-b001-interop-semantic-contracts-type-mediation-contract"',
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
    assert any(
        failure["check_id"] in {"M244-B001-PKG-03", "M244-B001-FORB-01"}
        for failure in payload["failures"]
    )

