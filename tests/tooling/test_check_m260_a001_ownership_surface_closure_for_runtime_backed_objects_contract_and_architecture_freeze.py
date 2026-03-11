from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT = ROOT / "scripts" / "check_m260_a001_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze.py"
EXPECTATIONS_DOC = ROOT / "docs" / "contracts" / "m260_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze_a001_expectations.md"
SPEC = importlib.util.spec_from_file_location(
    "check_m260_a001_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze",
    SCRIPT,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def run_checker(*extra_args: str, summary_path: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), "--summary-out", str(summary_path), "--skip-dynamic-probes", *extra_args],
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )


def test_checker_passes() -> None:
    summary_path = ROOT / "tmp" / "reports" / "m260" / "M260-A001" / "pytest_runtime_backed_object_ownership_surface_contract_summary.json"
    result = run_checker(summary_path=summary_path)
    assert result.returncode == 0, result.stderr
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["mode"] == "m260-a001-runtime-backed-object-ownership-surface-freeze-v1"
    assert payload["contract_id"] == "objc3c-runtime-backed-object-ownership-surface-freeze/m260-a001-v1"
    assert payload["next_issue"] == "M260-A002"
    assert payload["ok"] is True
    assert payload["dynamic_probe"]["skipped"] is True


def test_checker_default_summary_out_is_under_tmp_reports() -> None:
    args = contract.parse_args([])
    summary_path = Path(args.summary_out).resolve()
    assert summary_path.is_absolute()
    assert summary_path.relative_to(ROOT).as_posix().startswith("tmp/reports/m260/M260-A001/")


def test_checker_fails_closed_when_expectations_contract_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-runtime-backed-object-ownership-surface-freeze/m260-a001-v1`",
            "Contract ID: `objc3c-runtime-backed-object-ownership-surface-freeze/m260-a001-drifted`",
            1,
        ),
        encoding="utf-8",
    )
    summary_path = tmp_path / "summary.json"
    result = run_checker("--expectations-doc", str(drift_doc), summary_path=summary_path)
    assert result.returncode == 1
    payload = json.loads(summary_path.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M260-A001-DOC-01" for failure in payload["failures"])
