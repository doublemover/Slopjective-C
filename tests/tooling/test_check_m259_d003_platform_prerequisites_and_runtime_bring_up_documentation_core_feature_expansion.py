from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m259_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion_d003_expectations.md"
PACKAGE_JSON = ROOT / "package.json"
SPEC = importlib.util.spec_from_file_location(
    "check_m259_d003_platform_prerequisites_and_runtime_bring_up_documentation_core_feature_expansion",
    SCRIPT,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def run_checker(*extra_args: str, summary_path: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(summary_path), *extra_args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )


def test_checker_passes() -> None:
    summary_path = ROOT / "tmp" / "reports" / "m259" / "M259-D003" / "pytest_platform_prerequisites_and_runtime_bring_up_documentation_summary.json"
    result = run_checker(summary_path=summary_path)
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m259-d003-platform-prerequisites-and-runtime-bringup-v1"
    assert payload["contract_id"] == "objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1"
    assert payload["next_issue"] == "M259-E001"
    assert payload["ok"] is True


def test_checker_default_summary_out_is_under_tmp_reports() -> None:
    args = contract.parse_args([])
    summary_path = Path(args.summary_out).resolve()
    assert summary_path.is_absolute()
    assert summary_path.relative_to(ROOT).as_posix().startswith("tmp/reports/m259/M259-D003/")


def test_checker_fails_closed_when_expectations_contract_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1`",
            "Contract ID: `objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-drifted`",
            1,
        ),
        encoding="utf-8",
    )
    summary_path = tmp_path / "summary.json"
    result = run_checker("--expectations-doc", str(drift_doc), summary_path=summary_path)
    assert result.returncode == 1
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M259-D003-DOC-01" for failure in payload["failures"])


def test_checker_fails_closed_when_package_script_surface_drifts(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m259-d003-lane-d-readiness": "python scripts/run_m259_d003_lane_d_readiness.py"',
            '"check:objc3c:m259-d003-lane-d-readiness": "python scripts/run_m259_d002_lane_d_readiness.py"',
            1,
        ),
        encoding="utf-8",
    )
    summary_path = tmp_path / "summary.json"
    result = run_checker("--package-json", str(drift_package), summary_path=summary_path)
    assert result.returncode == 1
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M259-D003-PKG-04" for failure in payload["failures"])
