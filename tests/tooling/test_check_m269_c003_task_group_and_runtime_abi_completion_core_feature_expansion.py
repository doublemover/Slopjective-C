from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m269_c003_task_group_and_runtime_abi_completion_core_feature_expansion.py"
SUMMARY = ROOT / "tmp" / "reports" / "m269" / "M269-C003" / "task_runtime_abi_completion_summary.json"


def test_m269_c003_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-task-runtime-abi-completion/m269-c003-v1"' in text

