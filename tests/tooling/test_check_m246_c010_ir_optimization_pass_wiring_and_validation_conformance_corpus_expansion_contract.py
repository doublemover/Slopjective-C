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
    / "check_m246_c010_ir_optimization_pass_wiring_and_validation_conformance_corpus_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_c010_ir_optimization_pass_wiring_and_validation_conformance_corpus_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_c010_ir_optimization_pass_wiring_and_validation_conformance_corpus_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all_occurrences(text: str, old: str, new: str) -> str:
    count = text.count(old)
    assert count > 0, f"expected snippet not found for drift mutation: {old}"
    return text.replace(old, new)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m246-c010-ir-optimization-pass-wiring-validation-conformance-corpus-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 70
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_emit_json_writes_canonical_payload_to_stdout(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout_payload = json.loads(capsys.readouterr().out)
    file_payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert stdout_payload == file_payload
    assert stdout_payload["ok"] is True


def test_contract_default_summary_out_is_under_tmp_reports_m246_c010() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-C010/")


def test_contract_fails_closed_when_prerequisite_asset_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_assets = list(contract.PREREQUISITE_ASSETS)
    original = drift_assets[0]
    drift_assets[0] = contract.AssetCheck(
        check_id=original.check_id,
        relative_path=Path("scripts/does_not_exist_m246_c010_dependency.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", tuple(drift_assets))

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == original.check_id for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_ir_optimization_pass_wiring_and_validation_conformance_corpus_expansion_c010_expectations.md"
    drift_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-C009`",
            "Dependencies: `M246-C099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m246_c010_ir_optimization_pass_wiring_and_validation_conformance_corpus_expansion_packet.md"
    drift_packet.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5086`",
            "Issue: `#5000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_c009_dependency_anchor_drifts(tmp_path: Path) -> None:
    drift_c009_doc = (
        tmp_path / "m246_ir_optimization_pass_wiring_and_validation_conformance_matrix_implementation_c009_expectations.md"
    )
    drift_c009_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_C009_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-C008`",
            "Dependencies: `M246-C099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c009-expectations-doc",
            str(drift_c009_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-C009-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_c009_readiness_drops_c005_chain_anchor(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_c009_lane_c_readiness.py"
    drift_readiness.write_text(
        replace_all_occurrences(
            contract.DEFAULT_C009_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_c008_lane_c_readiness.py",
            "scripts/run_m246_c099_lane_c_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c009-readiness-script",
            str(drift_readiness),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-C009-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_c010_readiness_drops_c009_chain(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_c010_lane_c_readiness.py"
    drift_readiness.write_text(
        replace_all_occurrences(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m246_c009_lane_c_readiness.py",
            "scripts/run_m246_c099_lane_c_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_c010_readiness_checker_drifts_to_implementation(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_c010_lane_c_readiness.py"
    drift_readiness.write_text(
        replace_all_occurrences(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/check_m246_c010_ir_optimization_pass_wiring_and_validation_conformance_corpus_expansion_contract.py",
            "scripts/check_m246_c004_ir_optimization_pass_wiring_and_validation_core_feature_implementation_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"test:objc3c:perf-budget": ',
            '"test:objc3c:perf-budget-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-C010-PKG-05" for failure in payload["failures"])


def test_check_text_artifact_fails_closed_when_read_raises_os_error(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "drift.md"
    drift_doc.write_text("content", encoding="utf-8")
    original_read_text = contract.Path.read_text

    def failing_read_text(self: Path, *args: object, **kwargs: object) -> str:
        if self == drift_doc:
            raise OSError("simulated read failure")
        return original_read_text(self, *args, **kwargs)

    monkeypatch.setattr(contract.Path, "read_text", failing_read_text)

    checks_total, failures = contract.check_text_artifact(
        path=drift_doc,
        exists_check_id="M246-C010-TEST-EXISTS",
        snippets=(),
    )

    assert checks_total == 1
    assert len(failures) == 1
    assert failures[0].check_id == "M246-C010-TEST-EXISTS"
    assert "unable to read required document:" in failures[0].detail


def test_contract_sorts_failures_by_check_id_artifact_and_detail(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
    capsys: pytest.CaptureFixture[str],
) -> None:
    monkeypatch.setattr(contract, "check_prerequisite_assets", lambda: (0, []))

    def fake_check_text_artifact(*, path: Path, exists_check_id: str, snippets: tuple[contract.SnippetCheck, ...]):
        if exists_check_id == "M246-C010-DOC-EXP-EXISTS":
            return (
                1,
                [
                    contract.Finding(artifact="zeta.md", check_id="M246-C010-Z", detail="z detail"),
                    contract.Finding(artifact="alpha.md", check_id="M246-C010-A", detail="z detail"),
                    contract.Finding(artifact="alpha.md", check_id="M246-C010-A", detail="a detail"),
                ],
            )
        return 1, []

    monkeypatch.setattr(contract, "check_text_artifact", fake_check_text_artifact)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    failures = payload["failures"]
    assert [failure["check_id"] for failure in failures] == ["M246-C010-A", "M246-C010-A", "M246-C010-Z"]
    assert [failure["artifact"] for failure in failures] == ["alpha.md", "alpha.md", "zeta.md"]
    assert [failure["detail"] for failure in failures] == ["a detail", "z detail", "z detail"]
    stderr_lines = [line for line in capsys.readouterr().err.splitlines() if line.strip()]
    assert stderr_lines[0].startswith("[M246-C010-A] alpha.md: a detail")
    assert stderr_lines[1].startswith("[M246-C010-A] alpha.md: z detail")
    assert stderr_lines[2].startswith("[M246-C010-Z] zeta.md: z detail")
