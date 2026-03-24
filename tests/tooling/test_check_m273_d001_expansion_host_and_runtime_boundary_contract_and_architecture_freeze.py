from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m273_d001_expansion_host_and_runtime_boundary_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m273" / "M273-D001" / "expansion_host_runtime_boundary_summary.json"


def test_checker_passes_static() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER), "--skip-dynamic-probes"],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"dynamic_probes_executed": false' in text


def test_checker_passes_dynamic() -> None:
    completed = subprocess.run(
        [sys.executable, str(CHECKER)],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part10-expansion-host-runtime-boundary/m273-d001-v1"' in text
