from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m246_e004_optimization_gate_and_perf_evidence_core_feature_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_e004_optimization_gate_and_perf_evidence_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_e004_optimization_gate_and_perf_evidence_core_feature_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    updated = text.replace(old, new, 1)
    assert updated != text
    return updated


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m246-e004-optimization-gate-perf-evidence-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 34
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_e004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-E004/")


def test_contract_emits_json_with_summary_payload_parity(
    tmp_path: Path, capsys: pytest.CaptureFixture[str]
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout_payload = json.loads(capsys.readouterr().out)
    file_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert stdout_payload == file_payload
    assert stdout_payload["mode"] == contract.MODE
    assert stdout_payload["ok"] is True


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m246_e004_contract.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_dependency_token(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m246_optimization_gate_and_perf_evidence_core_feature_expansion_e004_expectations.md"
    )
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "| `M246-C007` | Dependency token `M246-C007` is mandatory as pending seeded lane-C edge-case completion assets. |",
            "| `M246-C099` | Dependency token `M246-C099` is mandatory as pending seeded lane-C edge-case completion assets. |",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E004-DOC-EXP-06" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path / "m246_optimization_gate_and_perf_evidence_core_feature_expansion_e004_expectations.md"
    )
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Issue: `#6695`",
            "Issue: `#6699`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E004-DOC-EXP-09" for failure in payload["failures"])


def test_contract_fails_closed_on_read_errors(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    unreadable_doc = tmp_path / "unreadable_expectations.md"
    unreadable_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
        encoding="utf-8",
    )

    original_read_text = contract.Path.read_text

    def _raise_for_unreadable(path: Path, *args: object, **kwargs: object) -> str:
        if path == unreadable_doc:
            raise OSError("simulated read failure")
        return original_read_text(path, *args, **kwargs)

    monkeypatch.setattr(contract.Path, "read_text", _raise_for_unreadable)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(unreadable_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E004-DOC-EXP-EXISTS-READ" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_script_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_once(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m246-e004-lane-e-readiness": '
            '"npm run check:objc3c:m246-e003-lane-e-readiness '
            '&& npm run check:objc3c:m246-a003-lane-a-readiness '
            '&& npm run check:objc3c:m246-b004-lane-b-readiness '
            '&& npm run check:objc3c:m246-c007-lane-c-readiness '
            '&& npm run check:objc3c:m246-d003-lane-d-readiness '
            '&& npm run check:objc3c:m246-e004-optimization-gate-perf-evidence-core-feature-expansion-contract '
            '&& npm run test:tooling:m246-e004-optimization-gate-perf-evidence-core-feature-expansion-contract"',
            '"check:objc3c:m246-e004-lane-e-readiness": '
            '"npm run check:objc3c:m246-e003-optimization-gate-perf-evidence-core-feature-implementation-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-E004-PKG-03" for failure in payload["failures"])


def test_contract_failure_ordering_is_deterministic(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_expectations = tmp_path / "missing_expectations.md"
    missing_packet = tmp_path / "missing_packet.md"
    missing_package = tmp_path / "missing_package.json"

    exit_code = contract.run(
        [
            "--expectations-doc",
            str(missing_expectations),
            "--packet-doc",
            str(missing_packet),
            "--package-json",
            str(missing_package),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    failures = payload["failures"]
    assert len(failures) >= 3
    assert failures == sorted(
        failures,
        key=lambda failure: (failure["artifact"], failure["check_id"], failure["detail"]),
    )
    assert any(failure["check_id"] == "M246-E004-DOC-EXP-EXISTS" for failure in failures)
    assert any(failure["check_id"] == "M246-E004-DOC-PKT-EXISTS" for failure in failures)
    assert any(failure["check_id"] == "M246-E004-PKG-EXISTS" for failure in failures)

