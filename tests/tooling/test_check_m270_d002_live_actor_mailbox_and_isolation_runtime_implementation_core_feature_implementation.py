from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
CHECKER = ROOT / "scripts" / "check_m270_d002_live_actor_mailbox_and_isolation_runtime_implementation_core_feature_implementation.py"
SUMMARY = ROOT / "tmp" / "reports" / "m270" / "M270-D002" / "live_actor_mailbox_runtime_summary.json"


def test_m270_d002_checker_passes() -> None:
    completed = subprocess.run([sys.executable, str(CHECKER)], cwd=ROOT, capture_output=True, text=True, check=False)
    assert completed.returncode == 0, completed.stdout + completed.stderr
    assert SUMMARY.exists()
    text = SUMMARY.read_text(encoding="utf-8")
    assert '"ok": true' in text
    assert '"contract_id": "objc3c-part7-live-actor-mailbox-and-isolation-runtime/m270-d002-v1"' in text

