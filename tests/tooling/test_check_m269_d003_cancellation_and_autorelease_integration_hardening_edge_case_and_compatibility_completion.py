from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m269_d003_cancellation_and_autorelease_integration_hardening_edge_case_and_compatibility_completion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m269" / "M269-D003" / "task_runtime_hardening_summary.json"


def test_m269_d003_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-task-runtime-hardening/m269-d003-v1"' in text
