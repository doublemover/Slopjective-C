from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_build_install_run_workflow_and_binary_packaging_core_feature_implementation_d002_expectations.md"
PACKAGE_JSON = ROOT / "package.json"
SPEC = importlib.util.spec_from_file_location(
    "check_m259_d002_build_install_run_workflow_and_binary_packaging_core_feature_implementation",
    SCRIPT,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def run_checker(*extra_args: str, summary_path: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), "--skip-dynamic-probes", "--summary-out", str(summary_path), *extra_args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )


def test_checker_passes_static_only() -> None:
    summary_path = ROOT / "tmp" / "reports" / "m259" / "M259-D002" / "pytest_build_install_run_workflow_and_binary_packaging_summary.json"
    result = run_checker(summary_path=summary_path)
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m259-d002-build-install-run-workflow-and-binary-packaging-v1"
    assert payload["contract_id"] == "objc3c-runnable-build-install-run-package/m259-d002-v1"
    assert payload["next_issue"] == "M259-D003"
    assert payload["dynamic_probes_executed"] is False
    assert payload["ok"] is True


def test_checker_default_summary_out_is_under_tmp_reports() -> None:
    args = contract.parse_args([])
    summary_path = Path(args.summary_out).resolve()
    assert summary_path.is_absolute()
    assert summary_path.relative_to(ROOT).as_posix().startswith("tmp/reports/m259/M259-D002/")


def test_checker_fails_closed_when_expectations_contract_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-runnable-build-install-run-package/m259-d002-v1`",
            "Contract ID: `objc3c-runnable-build-install-run-package/m259-d002-drifted`",
            1,
        ),
        encoding="utf-8",
    )
    summary_path = tmp_path / "summary.json"
    result = run_checker("--expectations-doc", str(drift_doc), summary_path=summary_path)
    assert result.returncode == 1
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M259-D002-DOC-01" for failure in payload["failures"])


def test_checker_fails_closed_when_package_command_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"package:objc3c-native:runnable-toolchain": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/package_objc3c_runnable_toolchain.ps1"',
            '"package:objc3c-native:runnable-toolchain": "pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1"',
            1,
        ),
        encoding="utf-8",
    )
    summary_path = tmp_path / "summary.json"
    result = run_checker("--package-json", str(drift_package), summary_path=summary_path)
    assert result.returncode == 1
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M259-D002-PKG-01" for failure in payload["failures"])
