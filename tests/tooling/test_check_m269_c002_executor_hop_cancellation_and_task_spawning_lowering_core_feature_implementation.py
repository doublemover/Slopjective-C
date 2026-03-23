from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m269_c002_executor_hop_cancellation_and_task_spawning_lowering_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m269" / "M269-C002" / "task_runtime_lowering_implementation_summary.json"


def test_m269_c002_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-task-runtime-lowering-implementation/m269-c002-v1"' in text
