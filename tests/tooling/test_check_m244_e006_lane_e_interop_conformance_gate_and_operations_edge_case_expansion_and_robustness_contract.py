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
    / "check_m244_e006_lane_e_interop_conformance_gate_and_operations_edge_case_expansion_and_robustness_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m244_e006_lane_e_interop_conformance_gate_and_operations_edge_case_expansion_and_robustness_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m244_e006_lane_e_interop_conformance_gate_and_operations_edge_case_expansion_and_robustness_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m244-e006-lane-e-interop-conformance-gate-operations-edge-case-expansion-and-robustness-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m244_e006() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m244/M244-E006/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    payload = json.loads(stdout)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_token_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m244_e006_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["expectations_doc"].read_text(encoding="utf-8").replace(
            "`M244-C008`",
            "`M244-C099`",
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
    assert any(failure["check_id"] == "M244-E006-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_reference_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_packet = tmp_path / "m244_e006_packet.md"
    drift_packet.write_text(
        contract.ARTIFACTS["packet_doc"].read_text(encoding="utf-8").replace(
            "`npm run --if-present check:objc3c:m244-c008-lane-c-readiness`",
            "`npm run check:objc3c:m244-c008-lane-c-readiness`",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["packet_doc"] = drift_packet
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M244-E006-DOC-PKT-10" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_command_drops_dependency_references(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"check:objc3c:m244-e006-lane-e-readiness": '
            '"npm run check:objc3c:m244-e005-lane-e-readiness '
            '&& npm run check:objc3c:m244-a005-lane-a-readiness '
            '&& npm run --if-present check:objc3c:m244-b007-lane-b-readiness '
            '&& npm run --if-present check:objc3c:m244-c008-lane-c-readiness '
            '&& npm run --if-present check:objc3c:m244-d008-lane-d-readiness '
            '&& npm run check:objc3c:m244-e006-lane-e-interop-conformance-gate-operations-edge-case-expansion-and-robustness-contract '
            '&& npm run test:tooling:m244-e006-lane-e-interop-conformance-gate-operations-edge-case-expansion-and-robustness-contract"',
            '"check:objc3c:m244-e006-lane-e-readiness": '
            '"npm run check:objc3c:m244-e005-lane-e-readiness '
            '&& npm run check:objc3c:m244-a005-lane-a-readiness '
            '&& npm run --if-present check:objc3c:m244-b007-lane-b-readiness '
            '&& npm run check:objc3c:m244-e006-lane-e-interop-conformance-gate-operations-edge-case-expansion-and-robustness-contract '
            '&& npm run test:tooling:m244-e006-lane-e-interop-conformance-gate-operations-edge-case-expansion-and-robustness-contract"',
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
    assert any(failure["check_id"] == "M244-E006-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_diagnostics_replay_optimization_input(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.ARTIFACTS["package_json"].read_text(encoding="utf-8").replace(
            '"test:objc3c:diagnostics-replay-proof": ',
            '"test:objc3c:diagnostics-replay-proof-disabled": ',
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
    assert any(failure["check_id"] == "M244-E006-PKG-04" for failure in payload["failures"])

