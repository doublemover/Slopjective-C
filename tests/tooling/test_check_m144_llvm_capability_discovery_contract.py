from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m144_llvm_capability_discovery_contract.py"
PACKAGE_JSON = ROOT / "package.json"
SPEC = importlib.util.spec_from_file_location(
    "check_m144_llvm_capability_discovery_contract",
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
    assert payload["mode"] == "m144-llvm-capability-discovery-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 20
    assert payload["checks_passed"] == payload["checks_total"]


def test_contract_fails_closed_when_contract_doc_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    contract_doc_drift = tmp_path / "llvm_capability_discovery_expectations.md"
    contract_doc_drift.write_text(
        (
            ROOT / "docs" / "contracts" / "llvm_capability_discovery_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-llvm-capability-discovery-contract/m144-v1`",
            "Contract ID: `objc3c-llvm-capability-discovery-contract/m144-drifted`",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["contract_doc"] = contract_doc_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M144-DOC-CON-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_package_drops_closeout_spec_lint(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]
    scripts["check:compiler-closeout:m144"] = scripts["check:compiler-closeout:m144"].replace(
        ' && python scripts/spec_lint.py --glob "docs/contracts/llvm_capability_discovery_expectations.md"',
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
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M144-PKG-check:compiler-closeout:m144-3"
        for failure in payload["failures"]
    )


def test_package_wires_m144_closeout_contract() -> None:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = payload["scripts"]

    assert "check:compiler-closeout:m144" in scripts
    assert scripts["check:compiler-closeout:m144"] == (
        "python scripts/check_m144_llvm_capability_discovery_contract.py "
        "&& npm run test:objc3c:m144-llvm-capability-discovery "
        '&& python scripts/spec_lint.py --glob "docs/contracts/llvm_capability_discovery_expectations.md"'
    )
