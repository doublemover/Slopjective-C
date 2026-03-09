from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion.py"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "m255"
    / "M255-D004"
    / "protocol_category_method_resolution_summary.json"
)

SPEC = importlib.util.spec_from_file_location(
    "check_m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
MODULE = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = MODULE
SPEC.loader.exec_module(MODULE)


def run_checker(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT_PATH), *args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )


def test_static_contract_passes() -> None:
    result = run_checker("--skip-dynamic-probes")
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(result.stdout)
    assert payload["ok"] is True
    assert payload["contract_id"] == MODULE.CONTRACT_ID
    assert payload["dynamic_case"]["skipped"] is True


def test_summary_path_is_under_tmp_reports() -> None:
    result = run_checker("--skip-dynamic-probes")
    assert result.returncode == 0, result.stdout + result.stderr
    assert SUMMARY_PATH.exists()
    payload = json.loads(SUMMARY_PATH.read_text(encoding="utf-8"))
    assert payload["ok"] is True


def test_checker_fails_when_expectations_contract_drifts(tmp_path: Path) -> None:
    bad_doc = tmp_path / "expectations.md"
    text = MODULE.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8")
    bad_doc.write_text(
        text.replace(MODULE.CONTRACT_ID, "objc3c-runtime-protocol-category-method-resolution/m255-d004-bad", 1),
        encoding="utf-8",
    )
    result = run_checker("--skip-dynamic-probes", "--expectations-doc", str(bad_doc))
    assert result.returncode != 0
    payload = json.loads(result.stdout)
    assert any(finding["check_id"] == "M255-D004-DOC-EXP-02" for finding in payload["findings"])


def test_checker_fails_when_runtime_readme_drifts(tmp_path: Path) -> None:
    bad_doc = tmp_path / "README.md"
    text = MODULE.DEFAULT_RUNTIME_README.read_text(encoding="utf-8")
    bad_doc.write_text(
        text.replace(MODULE.PROTOCOL_DECLARATION_MODEL, "protocol-declaration-model-drift", 1),
        encoding="utf-8",
    )
    result = run_checker("--skip-dynamic-probes", "--runtime-readme", str(bad_doc))
    assert result.returncode != 0
    payload = json.loads(result.stdout)
    assert any(finding["check_id"] == "M255-D004-RTDOC-02" for finding in payload["findings"])


def test_dynamic_probe_passes() -> None:
    result = run_checker()
    assert result.returncode == 0, result.stdout + result.stderr
    payload = json.loads(result.stdout)
    assert payload["ok"] is True
    dynamic_case = payload["dynamic_case"]
    probe_payload = dynamic_case["probe_payload"]
    assert probe_payload["instance_first"] == MODULE.EXPECTED_INSTANCE_RESULT
    assert probe_payload["class_self"] == MODULE.EXPECTED_CLASS_RESULT
    assert probe_payload["known_class"] == MODULE.EXPECTED_CLASS_RESULT
    assert probe_payload["fallback_expected"] == MODULE.EXPECTED_FALLBACK_RESULT
    assert probe_payload["fallback_first_state"]["last_protocol_probe_count"] == MODULE.EXPECTED_PROTOCOL_PROBE_COUNT
    assert probe_payload["instance_entry"]["category_probe_count"] == MODULE.EXPECTED_CATEGORY_PROBE_COUNT
