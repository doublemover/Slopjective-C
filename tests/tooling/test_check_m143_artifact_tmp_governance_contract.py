from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m143_artifact_tmp_governance_contract.py"
PACKAGE_JSON = ROOT / "package.json"
SPEC = importlib.util.spec_from_file_location(
    "check_m143_artifact_tmp_governance_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m143-artifact-tmp-governance-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 20
    assert payload["checks_passed"] == payload["checks_total"]


def test_contract_fails_closed_when_cli_doc_default_out_dir_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    cli_fragment_drift = tmp_path / "10-cli.md"
    cli_fragment_drift.write_text(
        (
            ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md"
        ).read_text(encoding="utf-8").replace(
            "Default `--out-dir`: `tmp/artifacts/compilation/objc3c-native`",
            "Default `--out-dir`: `artifacts/compilation/objc3c-native`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["cli_fragment"] = cli_fragment_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M143-DOC-CLI-01"
        for failure in payload["failures"]
    )


def test_package_wires_m143_closeout_scripts() -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]

    assert "check:objc3c:library-cli-parity:source:m143" in scripts
    assert "test:objc3c:m143-artifact-governance" in scripts
    assert "check:compiler-closeout:m143" in scripts
    assert "check:compiler-closeout:m143" in scripts["check:task-hygiene"]
    assert "tests/tooling/test_objc3c_parser_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_parser_ast_builder_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_sema_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_sema_pass_manager_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_frontend_types_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_lowering_contract.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_ir_emitter_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]


def test_contract_fails_closed_when_package_drops_parser_ast_lane_a_coverage(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]
    scripts["test:objc3c:m143-artifact-governance"] = scripts[
        "test:objc3c:m143-artifact-governance"
    ].replace(
        " tests/tooling/test_objc3c_parser_extraction.py",
        "",
        1,
    )

    package_drift = tmp_path / "package.json"
    package_drift.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["package_json"] = package_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    summary = json.loads(summary_out.read_text(encoding="utf-8"))
    assert summary["ok"] is False
    assert any(
        "test_objc3c_parser_extraction.py" in failure["detail"]
        for failure in summary["failures"]
    )


def test_contract_fails_closed_when_sema_lane_b_default_run_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    sema_script_drift = tmp_path / "check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1"
    sema_script_drift.write_text(
        (
            ROOT / "scripts" / "check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1"
        ).read_text(encoding="utf-8").replace(
            '$defaultRunId = "m143-sema-type-system-default"',
            '$defaultRunId = "lane-b-nondeterministic"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["sema_tmp_governance_script"] = sema_script_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    summary = json.loads(summary_out.read_text(encoding="utf-8"))
    assert summary["ok"] is False
    assert any(
        failure["check_id"] == "M143-TMP-06B"
        for failure in summary["failures"]
    )


def test_contract_fails_closed_when_lane_c_typed_abi_default_run_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    typed_abi_script_drift = tmp_path / "check_objc3c_typed_abi_replay_proof.ps1"
    typed_abi_script_drift.write_text(
        (
            ROOT / "scripts" / "check_objc3c_typed_abi_replay_proof.ps1"
        ).read_text(encoding="utf-8").replace(
            '$defaultRunId = "m143-lane-c-typed-abi-default"',
            '$defaultRunId = "lane-c-nondeterministic"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["typed_abi_tmp_governance_script"] = typed_abi_script_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    summary = json.loads(summary_out.read_text(encoding="utf-8"))
    assert summary["ok"] is False
    assert any(
        failure["check_id"] == "M143-TMP-08B"
        for failure in summary["failures"]
    )
