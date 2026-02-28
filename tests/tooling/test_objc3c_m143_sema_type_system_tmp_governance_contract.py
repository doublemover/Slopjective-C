from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEMA_TMP_GOVERNANCE_SCRIPT = (
    ROOT / "scripts" / "check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1"
)
SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
CONTRACT_DOC = ROOT / "docs" / "contracts" / "artifact_tmp_governance_expectations.md"
PACKAGE_JSON = ROOT / "package.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_sema_tmp_governance_script_defaults_are_deterministic_and_tmp_governed() -> None:
    script_text = _read(SEMA_TMP_GOVERNANCE_SCRIPT)

    assert '$suiteRoot = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract"' in script_text
    assert '$defaultRunId = "m143-sema-type-system-default"' in script_text
    assert "return $DefaultRunId" in script_text
    assert '$runDirRel = "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/$runId"' in script_text
    assert "$env:OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID" in script_text
    assert "Get-Date -Format" not in script_text


def test_lane_b_docs_and_contract_publish_deterministic_tmp_path_rules() -> None:
    assert "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/" in _read(
        SEMANTICS_FRAGMENT
    )
    assert "m143-sema-type-system-default" in _read(SEMANTICS_FRAGMENT)
    assert "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json" in _read(
        ARTIFACTS_FRAGMENT
    )
    assert "m143-sema-type-system-default" in _read(ARTIFACTS_FRAGMENT)
    assert "m143-sema-type-system-default" in _read(TESTS_FRAGMENT)
    assert "OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID" in _read(TESTS_FRAGMENT)
    assert "| `M143-B001` |" in _read(CONTRACT_DOC)
    assert "m143-sema-type-system-default" in _read(CONTRACT_DOC)


def test_m143_governance_test_script_includes_lane_b_contract_tests() -> None:
    package_payload = json.loads(_read(PACKAGE_JSON))
    test_script = package_payload["scripts"]["test:objc3c:m143-artifact-governance"]

    assert "tests/tooling/test_objc3c_sema_extraction.py" in test_script
    assert "tests/tooling/test_objc3c_sema_pass_manager_extraction.py" in test_script
    assert "tests/tooling/test_objc3c_frontend_types_extraction.py" in test_script
    assert "tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py" in test_script
