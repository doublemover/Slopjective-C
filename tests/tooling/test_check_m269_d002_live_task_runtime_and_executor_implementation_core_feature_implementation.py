from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m269_d002_live_task_runtime_and_executor_implementation_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m269" / "M269-D002" / "live_task_runtime_integration_summary.json"


def test_m269_d002_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-live-task-runtime-integration/m269-d002-v1"' in text
