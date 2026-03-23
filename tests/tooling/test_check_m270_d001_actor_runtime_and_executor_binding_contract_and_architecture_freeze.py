from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m270_d001_actor_runtime_and_executor_binding_contract_and_architecture_freeze.py"
SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-D001" / "actor_runtime_executor_contract_summary.json"


def test_m270_d001_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-actor-runtime-and-executor-binding/m270-d001-v1"' in text
